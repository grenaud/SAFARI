# Overview

The `vg gbwt` subcommand is a pipeline for building and manipulating GBWT indexes and GBWTGraphs. This wiki page assumes VG **version 1.31.0** unless otherwise noted.

## File types

* [[GBWT|https://github.com/jltsiren/gbwt]] is an FM-index that can store a large number of similar threads (paths over node identifiers) space-efficiently. The default extensions is `.gbwt`.
* R-index is an optional structure that augments the GBWT for faster `locate()` queries. It is not currently used in the VG toolkit. The default extension is `.ri`.
* [[GBWTGraph|https://github.com/jltsiren/gbwtgraph]] is a `HandleGraph` implementation based of the graph induced by the threads stored in a GBWT index. It provides fast access to node sequences and an option to restrict graph traversals to paths supported by the indexed threads. The default extension is `.gg`.
* [[GBZ|https://github.com/jltsiren/gbwtgraph/blob/master/SERIALIZATION.md]] is a wrapper for GBWT and GBWTGraph. It requires **version 1.34.0** or later. The file format is more space-efficient than `.gg`. The default extension is `.gbz`.
* GBWT construction from GFA format may produce a segment-to-node translation table if GFA segments are too long or the segments have string names. Starting from **version 1.32.0** the translation is also stored in the GBWTGraph.

**Translation example:**
```
T	s1	1,2,3
T	s2	4
T	s3	5
T	s4	6,7
```

## Pipeline

There are several steps in the pipeline. Most steps require either one input GBWT or multiple input GBWTs. The input GBWTs are either loaded from the files specified in the input args or built in earlier steps. Option `-p` / `--progress` enables printing progress information to stderr.

1. Build GBWT indexes from an input source.
2. Merge multiple GBWT indexes.
3. Remove a sample from the GBWT index.
4. Build, augment, or replace the GBWT index with a greedy path cover algorithm.
5. Build a GBWTGraph.
6. Build an r-index.
7. Print GBWT metadata.
8. Extract threads from a GBWT index.

The possible input types for step 1 are: VCF, GFA, GBZ, embedded paths, and alignments in GAM/GAF format.

An input graph for GBWTGraph construction can be provided as a graph (option `-x` / `--xg-name`) or by using GFA or GBZ as the input type. These options are mutually exclusive.

If the pipeline produces an output GBWT, it will be written after step 4.

## Temporary files

The following steps create temporary files:

* Step 1: Construction from phased VCF files. VCF parse files usually require similar space to the compressed VCF files. If the construction produces multiple GBWT indexes, these temporary indexes will usually take similar space to the final GBWT index.
* Step 2: Parallel merging algorithm. The algorithm uses the first input GBWT as the base index. For each subsequent input GBWT, the temporary files take roughly 2-3 bytes per node (e.g. ~50 GB for 1000 paths and their reverse complements with 10 million nodes each). The files are deleted when the merging proceeds to the next input.

Temporary files are deleted when VG exits or fails in a controlled way. If VG crashes, the temporary files may remain.

Temporary directory can be specified with option `-d` / `--temp-dir`. If the option is not specified, VG tries to determine it using environment variables `TMPDIR`, `TMP`, `TEMP`, `TEMPDIR`, and `USERPROFILE` (in that order). If none of the variables is defined, VG defaults to `/tmp`.

## GBWT construction parameters

New GBWT indexes are built in steps 1 and 4. The construction uses the following computational parameters:

* `--buffer-size N`: Use a buffer of `N` million nodes for each construction job (default 100). The buffers typically require 8`N` MiB for each construction job. For best performance, buffer size should be at least an order of magnitude larger than the length of the threads.
* `--id-interval N`: Sample thread is at one out of `N` nodes (default 1024). This is a time-space trade-off. The samples use space proportional to 1/`N`, while `locate()` takes time proportional to `N`. The default is intended for 1000GP indexes, where the samples take similar space to the GBWT itself.

## Multithreading

GBWT construction from VCF input (`-v` / `--vcf-input`) and GFA input (`-G` / `--gfa-input`; from **version 1.38.0**) is based on multiple non-overlapping construction jobs. Each job uses two threads, but one thread is usually idle most of the time.

* `--num-jobs N`: Run `N` parallel construction jobs. The default is half of the number of logical CPU cores.

Parallel GBWT merging (`-b` / `--parallel`) and r-index construction (`-r` / `--r-index`) extract all paths from a GBWT index using multiple parallel search threads.

* `--num-threads N`: Use `N` parallel search threads. The default is the number of logical CPU cores.

# GBWT construction (step 1)

Construction requires an output GBWT (`-o` / `--output`).

## Construction from phased VCF files

Construction from VCF input requires an input graph (`-x` / `--xg-name`).

**Example:** 1000GP GBWT construction.
```
# Combine multiple input graphs built with `vg construct -a`.
vg index -x graph-with-alts.xg -L $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vg; done)

# Build the GBWT using 14 parallel jobs. This requires ~200 GiB memory. Construction
# time is bounded by the time required for indexing chr2.
vg gbwt -x graph-with-alts.xg -o 1000gp.gbwt --preset 1000gp --num-jobs 14 -v \
    $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vcf.gz; done)
```

Option `-v` / `--vcf-input` enables construction from phased VCF files. Input args are now interpreted as VCF files instead of GBWT files and each contig must be present in at most one VCF file. The VCF files must the ones used for building the graph or a subset of them. The input graph must contain variant information (often called alt paths) created by `vg construct` option `-a`. An XG index will store the alt paths if it is built with `vg index` option `-L`. (Note that this XG index can be several times larger than an index without alt paths.)

If the construction creates multiple jobs (see below), the resulting temporary GBWTs will automatically be merged in step 2 using the fast algorithm.

### VCF parsing options

* `--ignore-missing`: Do not warn when variants encountered in the VCF file are missing from the input graph.
* `--actual-phasing`: By default, GBWT construction treats unphased homozygous genotypes as phased. With this option, they will remain unphased and create phase breaks in the threads.
* `--force-phasing`: Randomly phase unphased genotypes.
* `--discard-overlaps`: If a haplotype contains overlapping alternate alleles and the overlap cannot be resolved, replace the second alternate allele with the reference allele instead of creating a phase break.
* `--batch-size N`: Generate threads in batches of `N` samples (default 200). Each construction job will buffer the threads generated from the current batch until the haplotype is finished or a phase break occurs. If thread length is `M` nodes, buffering `N` diploid samples typically takes around 8`MN` bytes for that job.

With options `--force-phasing` and `--discard-overlaps`, the generated haplotypes will generally be chromosome-length.

The following options are rarely necessary:

* `--sample-range X-Y`: Index only samples in range `X` to `Y` (inclusive, 0-based).
* `--rename V=P`: VCF contig `V` is the same as reference path `P` in the input graph (may repeat). By default, contig names are assumed to match path names.
* `--vcf-variants`: The names of alt paths in the input graph are based on VCF contig names instead of reference path names.
* `--vcf-region C:X-Y`: Restrict the VCF contig with name `C` to region `X-Y` (inclusive, 1-based; may repeat). This only makes sense if the graph was built with a similar restriction.
* `--exclude-sample X`: Do not index the sample with name `X` (may repeat). This is generally faster than removing the threads in step 3.

### Presets

Presets replace the default GBWT construction parameters and VCF parsing options. Preset `X` can be chosen with option `--preset X` and it can be further modified with subsequent command line arguments.

The following presets are currently available:

* `1000gp`: Generate chromosome-length haplotypes from 1000GP VCF files. Corresponds to `--buffer-size 200 --batch-size 100 --force-phasing --discard-overlaps`.

### Construction jobs

The construction will automatically create one or more jobs for each VCF file, each of them corresponding to one or more contigs (non-alt paths in the input graph). The size of a job is the total length of the paths in it (in nodes), and the maximum size of a job is the length of the longest path in the graph. By default, contigs are assigned to jobs using a first-fit heuristic in decreasing order by length. This can be overridden with option `--inputs-as-jobs`, which creates one job for each VCF file.

## Construction from GFA format

**Example:** GBWT of W-lines.
```
# Build a GBWT of the walks in the GFA file.
vg gbwt -o walks.gbwt -G walks.gfa
```

**Example:** GBWT of P-lines.
```
# Build a GBWT of the paths in the GFA file.
# Parse metadata from path names of the form <sample>_<contig>_<haplotype>.
vg gbwt -o paths.gbwt --path-regex "(.*)_(.*)_(.*)" --path-fields _SCH -G paths.gfa
```

A GFA file can be converted into a GBWT index (and optionally a GBWTGraph; see step 5) with option `-G` / `--gfa-input`. This construction option requires one input arg. The construction will convert W-lines or P-lines into GBWT threads. If both line types are present, the P-lines will be interpreted as reference paths. The reference paths will be stored with sample name `_gbwt_ref` and using the path name as contig name. When a GBWTGraph is converted into another graph type with `vg convert`, threads with sample name `_gbwt_ref` will be stored as paths.

GFA parsing will automatically select an appropriate buffer size for GBWT construction. This can be overridden by setting `--buffer-size` manually.

### GFA parsing options

* `--max-node N`: Chop segments longer than `N` bp (default 1024, use 0 to disable).
* `--path-regex X`: Parse metadata from path names using regex `X` (default `.*`).
* `--path-fields X`: Map regex submatches to these fields (default `S`).
* `--translation FILE`: Write the segment to node translation table to `FILE`. A translation table is generated if the segments have string names or if they have to be chopped into multiple nodes.

### Parsing metadata from path names

When indexing P-lines (with no W-lines in the file), GBWT metadata is parsed from path names using the regular expression specified by option `--path-regex`. The path name is recursively split into submatches corresponding to parts of the regex enclosed by parentheses. The tree of submatches, with the entire path name as the root, is traversed in preorder. The submatches are assigned to GBWT metadata fields according to the string specified by option `--path-fields`. 

Metadata fields (case insensitive):
* `S`: Sample name (string).
* `C`: Contig name (string).
* `H`: Haplotype / phase identifier (integer).
* `F`: Fragment identifier / running count (integer).
* Other: Ignore this field.

In the above example, path names of the form `<sample>_<contig>_<haplotype>` are parsed using options
`--path-regex "(.*)_(.*)_(.*)"` and `--path-fields _SCH`.

Each GBWT thread must have a unique name based on the metadata fields. If the fragment indentifier field `F` is left unspecified, threads with otherwise identical metadata will be disambiguated by automatically assigned fragment identifiers.

### Construcion jobs

Starting from **version 1.38.0**, GBWT construction from GFA starts by partitioning the graph into weakly connected components. The components are sorted by minimum node ids. Large components become separate jobs, while consecutive small components are combined into larger jobs. The jobs are started from the largest to the smallest (by the number of nodes), and the resulting GBWTs are automatically merged in the original order using the fast algorithm (see step 2).

## Extraction from a GBZ file

**Example:**
```
# Extract the GBWT index from a GBZ file.
vg gbwt -o graph.gbwt -Z graph.gbz
```

The GBWT index in a GBZ file can be extracted with option `-Z` / `--gbz-input`. This construction option requires one input arg. See step 5 for extracting the GBWTGraph as well.

Starting from **version 1.37.0**, the segment to node translation table can also be extracted with option `--translation FILE`.

## Construction from embedded paths

Construction from embedded paths requires an input graph (`-x` / `--xg-name`).

**Example:**
```
# Build a GBWT of the embedded (reference) paths with each path as a contig in
# the GBWT metadata.
vg gbwt -x graph.xg -o ref-paths.gbwt -E
```

The embedded paths in a graph can be indexed as GBWT threads with option `-E` / `--index-paths`. This construction option does not use input args. Path metadata is respected. 

## Construction from GAF/GAM files

Construction from GAF/GAM input requires an input graph (`-x` / `--xg-name`).

**Example:**

```
# Build a GBWT of the alignments in the GAF files.
vg gbwt -x graph.xg -o alignments.gbwt -A aln1.gaf aln2.gaf
```

The paths corresponding to alignments can be converted to threads and stored in a GBWT index with option `-A`/ `--alignment-input`. Multiple alignment files can be given as input args, but they will be processed sequentially. The default input format is GAF. Alignment files in the GAM format used by older versions of VG can be indexed with option `--gam-format`.

# Merging GBWT indexes (step 2)

There are multiple algorithms for merging GBWT indexes. The insertion and parallel algorithms work with any input GBWTs, while the fast algorithm requires that node identifiers do not overlap between the inputs.

Merging requires an output GBWT (`-o` / `--output`).

## Insertion algorithm

**Example:**
```
# Merge three batches of haplotypes into a single index.
vg gbwt -o merged.gbwt -m batch1.gbwt batch2.gbwt batch3.gbwt
```

Multiple input GBWTs can be merged into a single index with option `-m` / `--merge`. The default (insertion) algorithm uses the first input as the base index. It then loads the subsequent inputs one by one, extracts the threads from them, and inserts them into the base index using the construction algorithm. This is somewhat slower than building the indexes in the first place. Hence the largest index should be used as the base index.

## Fast merging of non-overlapping indexes

**Example:**
```
# Merge GBWTs of individual chromosomes into a single index.
vg gbwt -o all.gbwt -f $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.gbwt; done)
```

If each input GBWT uses separate node identifiers, they can be merged quickly with option `-f`/ `--fast`. This is usually the case when the input GBWTs are over separate chromosomes. The fast algorithm is automatically used when the construction from input VCFs creates multiple jobs.

## Parallel merging of overlapping indexes

**Example:**
```
# Merge three batches of haplotypes into a single index.
vg gbwt -o merged.gbwt -b batch1.gbwt batch2.gbwt batch3.gbwt
```

Parallel merging algorithm can be used with option `-b` / `--parallel`. It is primarily intended for merging large GBWTs that already contain thousands of human haplotypes. The algorithm uses the first input as the base index. Then, for each subsequent input, it first extracts all paths from the index, searches simultaneously for them in the base index, and writes the rank array into a number of temporary files. Then it launches a number of merge jobs that stream the rank array from the files and use the information for interleaving the two GBWTs.

### Parallel merging options

* `--chunk-size N`: Search in chunks of `N` sequences (default 1). Each thread is represented using two sequences. If the threads are short (e.g. alignments), larger chunk size may increase speed.
* `--pos-buffer N`: Use `N` MiB position buffers for each search thread (default 64).
* `--thread-buffer N`: Use `N` MiB thread buffers for each search thread (default 256). The compressed thread buffers should be several times larger than the uncompressed position buffers.
* `--merge-buffers N`: Merge 2^`N` thread buffers into one file per merge job (default 6). Larger numbers increase memory usage and make the temporary files larger. Smaller numbers create more temporary files and may make the merge jobs slower.
* `--merge-jobs N`: Run `N` parallel merge jobs. The default `M`/2 jobs on a system with `M` logical CPU cores, but no more than 4 jobs.

# Path cover GBWTs (step 4)

This step builds a GBWT based on artificially generated paths (threads). The path generation algorithm is similar to the one used in the Pan-genome Seed Index (PSI). We generate `N` threads for each component in the graph, starting each from an arbitrary node with the highest priority. We extend the thread greedily, always choosing the extension with the highest priority. See the comments in the [header](https://github.com/jltsiren/gbwtgraph/blob/master/include/gbwtgraph/path_cover.h) for further details.

Note that this is a greedy algorithm for a maximum coverage problem. We try to cover as many nodes as possible with the `N` threads while taking the priorities into account, but some nodes may not be covered.

## Path cover options

Path cover generation requires an input graph (`-x` / `--xg-name`), an output GBWT (`-o` / `--output`), and one algorithm.

* `-a` / `--augment-gbwt`: Augment the input GBWT with a greedy path cover of components with no threads.
* `-l`/ `--local-haplotypes`: Create a path cover based on sampling local haplotypes in the same proportions as in the input GBWT.
* `-P` / `--path-cover`: Create a greedy path cover of all components.
* `-n N` / `--num-paths N`: Generate `N` threads for each component (default 16 for most options).
* `-k K` / `--context-length K`: Use subpaths of length `K` for determining the priority of extensions.
* `--pass-paths`: Add named paths (e.g. reference paths) from the graph to the path cover. Requires **version 1.39.0** or later.

## Greedy path cover

**Example:**
```
# Generate a greedy cover with 16 threads per component.
vg gbwt -x graph.xg -o greedy-cover.gbwt -P
```

The basic greedy path cover algorithm (option `-P` / `--path-cover`) assumes that each path in the graph is equally likely. High-priority nodes and subpaths are those with the lowest coverage in the generated threads so far.

Greedy path cover does not use input GBWTs.

## Sampling local haplotypes

**Example:**
```
# Generate 64 threads per component by sampling the local haplotypes proportionally.
vg gbwt -x graph.xg -o sampled.gbwt -l haplotypes.gbwt
```

The sampling algorithm (option `-l` / `--local-haplotypes`) samples the local haplotypes proportionally. If the input GBWT contains threads for the current component, the highest priority is given to the nodes and subpaths with the highest ratio
```
  true_frequency / (sampled_frequency + 1)
```
so far. If there are no threads, the algorithm generates a greedy path cover for the component.

Sampling requires one input GBWT. The default number of generated threads is `N` = 64.

## Path cover of missing components

**Example:** Simple augmentation.
```
# Augment the GBWT with a greedy path cover of missing components.
vg gbwt -x graph.xg -o augmented.gbwt -a haplotypes.gbwt
```

**Example:** Build and augment.
```
# Combine multiple input graphs built with `vg construct -a` and a decoy graph.
vg index -x graph-with-alts.xg -L $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vg; done) decoy.vg

# Build the GBWT using 14 parallel jobs. Then augment it with a path cover of the decoy graph.
vg gbwt -x graph-with-alts.xg -o 1000gp.gbwt --preset 1000gp --num-jobs 14 -v -a \
    $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vcf.gz; done)
```

The augmentation algorithm (option `-a` / `--augment-gbwt`) augments the input GBWT with a greedy path cover of graph components without threads.

Augmentation requires one input GBWT.

# Other steps

## Removing samples (step 3)

**Example:**
```
# Remove sample NA12878 from the GBWT.
vg gbwt -o removed.gbwt -R NA12878 1000gp.gbwt
```

Option `-R` / `--remove-sample` removes all threads corresponding to the sample with name `X` from the input GBWT (may repeat).

Removing samples requires one input GBWT and an output GBWT (`-o` / `--output`).

## GBWTGraph construction (step 5)

**Example:** Simple GBWTGraph construction.
```
# Build a GBWTGraph based on the input GBWT.
vg gbwt -x graph.xg -g graph.gg haplotypes.gbwt
```

**Example:** Haplotype sampling and GBWTGraph construction.
```
# Sample the local haplotypes and build a GBWTGraph based on the sampled GBWT.
vg gbwt -x graph.xg -o sampled.gbwt -g sampled.gg -l haplotypes.gbwt
```

**Example:** GBWTGraph construction from GFA input.
```
# Build GBWT and GBWTGraph from GFA input.
vg gbwt -o graph.gbwt -g graph.gg -G graph.gfa
```

**Example:** GBZ construction from GFA input.
```
# Build GBZ (GBWT + GBWTGraph) from GFA input.
vg gbwt -g graph.gbz --gbz-format -G graph.gfa
```

**Example:** GBWT and GBWTGraph extraction from a GBZ file.
```
# Extract GBWT and GBWTGraph from GBZ input.
vg gbwt -o graph.gbwt -g graph.gg -Z graph.gbz
```

We can build a GBWTGraph corresponding to the final GBWT with option `-g X` / `--graph-name X` and write it to file `X`. The final GBWT is either the output GBWT if other options require it or the only input GBWT otherwise. The graph requires the final GBWT. It does not work without it or with other GBWT indexes.

The GBWTGraph can be serialized serialized in the [GBZ format](https://github.com/jltsiren/gbwtgraph/blob/master/SERIALIZATION.md) with option `--gbz-format`. As this format also includes the GBWT index, option `-o X` / `--output X` is ignored and unnecessary. GBZ format requires **version 1.34.0**. 

GBWTGraph construction requires one input GBWT and a graph, (`-x` / `--xg-name`), GFA input (`-G` / `--gfa-input`), or GBZ input (`-Z` / `--gbz-input`).

## R-index construction (step 6)

**Example:**
```
# Build an r-index for the input GBWT.
vg gbwt -r haplotypes.ri haplotypes.gbwt
```

We can build an r-index corresponding to the final GBWT with option `-r X` / `--r-index X` and write it to file `X`. The final GBWT is either the output GBWT if other options require it or the only input GBWT otherwise. The r-index requires the final GBWT. It does not work without it or with other GBWT indexes.

R-index construction requires one input GBWT.

## Metadata output (step 7)

There are several options for printing metadata from the input GBWT to stdout.

* `M` / `--metadata`: Print basic metadata information.
* `C` / `--contigs`: Print the total number of contigs.
* `H` / `--haplotypes`: Print the total number of haplotypes.
* `S` / `--samples`: Print the total number of samples.
* `L` / `--list-names`: When used with `-C` or `-S`, prints the names of the contigs or the samples instead of the total.
* `T` / `--thread-names`: Print the names of all threads.

Metadata output requires one input GBWT.

## Thread extraction (step 8)

These options extract threads from the input GBWT. They work even when the GBWT index has been built without metadata.

* `-c` / `--count-threads`: Print the total number of threads to stdout.
* `-e X` / `--extract X`: Extract the threads as `sdsl::int_vector_buffer<0>` to file `X`. This format can be used by the `build_gbwt` tool included in the GBWT repository.

Thread extraction requires one input GBWT.
