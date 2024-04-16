# General

This page gives an overview of manual index construction for graphs based on VCF files. It assumes VG **version 1.31.0** or later, unless mentioned otherwise. 

Many users may want to consider [[automatic indexing|Automatic indexing for read mapping and downstream inference]] with `vg autoindex` as an alternative to `vg index`. `vg autoindex` provides a simplified interface based on the downstream mapping tool you want to run rather than the index you want to create. Internally, `vg autoindex` uses the best practices developed in the VG team's own work.

See also:

* [[Index types|Index Types]].
* [[Example scripts|Construction Examples]] for building the indexes for a 1000GP graph.
* [[vg gbwt|VG GBWT Subcommand]] pipeline.
* [[Indexing huge datasets|Indexing Huge Datasets]] (e.g. tens of thousands of human samples).

## Multithreading

* GBWT construction uses two parallel threads for each construction job. `vg gbwt` may create multiple parallel jobs, which can be limited with option `--num-jobs`.
* Parts of GCSA construction are multithreaded using OpenMP. The number of threads defaults to the number of cores, but it can be changed with command line option `-t` or with environment variable `OMP_NUM_THREADS`.
* Parts of `vg prune` are also multithreaded using OpenMP.
* Minimizer index construction is multithreaded using OpenMP.
* Snarl finding before distance index construction is multithreaded using OpenMP.

## Helper files

These files are used internally in various workflows. They can be safely deleted once the workflow has finished. Old files are probably no longer valid.

* Node mappings: Created by `vg ids`, updated by `vg prune -u`, used in GCSA construction. When `vg prune -u` simplifies the graph, it creates duplicates of original nodes. The node mapping is used to create a GCSA index that maps kmers to the original nodes.
* Kmer files: Created by `vg kmers` or as temporary files in GCSA construction (see below). In the former case, the typical extension is `.graph`.
* Snarls: Created by `vg snarls`, used for distance index construction with `vg index -j`. The typical extension is `.snarls`.

## Temporary files

GCSA construction creates large temporary files. These files typically take hundreds of megabytes for each 1 Mbp of sequence in the graph.

If the graph is more complex than expected, the construction may use orders of magnitude more space before ultimately failing. By default, `vg index -g` is allowed to use 2048 gigabytes of temporary disk space. The limit can be changed with option `-Z`.

GBWT construction uses temporary files to store run-length encoded phasing information. Most of the time, these files use similar space to the `.vcf.gz` file. Sometimes the files can be several times larger (e.g. human chromosome X with male and female samples in random order).

GBWT construction for a multi-chromosome graph using `vg gbwt` creates temporary GBWT indexes for individual chromosomes. These files require similar space to the final GBWT index.

XG construction also uses temporary files. These files typically take similar space to the final XG index. Note that the `vg prune` subcommand creates a temporary XG index and hence uses temporary files in the "with haplotypes" and "with many paths" approaches.

Temporary files use the following naming schemes:

* `vg-<random>/vg-kmers-tmp-<random>`: Kmer files used as input to GCSA construction.
* `gcsa_<hostname>_<pid>_<counter>`: Temporary graphs created in GCSA construction.
* `parse_<hostname>_<pid>_<counter>`: Reference and variant paths used in GBWT construction.
* `phasing_<hostname>_<pid>_<counter>`: Phasing information used in GBWT construction.
* `gbwt-<counter>-<random>`: Temporary GBWT indexes created by `vg gbwt`.
* `xg-<random>/xg-<random>*`: Temporary files used for XG construction.

### Temporary directory

The directory for the temporary files is chosen according to the following rules:

1. User-defined directory, e.g. from option `-b` in `vg index`.
2. Environment variable `TMPDIR`.
3. Environment variable `TMP`.
4. Environment variable `TEMP`.
5. Environment variable `TEMPDIR`.
6. Environment variable `USERPROFILE`.
7. Directory `/tmp`.

### Deleting temporary files

When VG exits or fails in a controlled way, the temporary files are deleted. If VG crashes or is interrupted, some of the temporary files may remain. It is usually a good idea to remove the temporary files after GCSA construction:

```
rm -rf $TMPDIR/vg-*
rm -f $TMPDIR/gcsa_*
```

The same is true for GBWT and XG construction:

```
rm -f $TMPDIR/parse_*
rm -f $TMPDIR/phasing_*
rm -f $TMPDIR/gbwt-*
rm -rf $TMPDIR/xg-*
```

The above assumes that the temporary directory has been set using environment variable `TMPDIR`.

## Haplotype paths

GBWT construction (`vg gbwt -v`) transforms a phased VCF file into a collection of haplotype paths in the graph. Every time there is a phase break, the construction starts a new path. There are three major causes of phase breaks:

* If a haplotype has alternate alleles at overlapping sites and the construction cannot resolve the overlap, there will be a phase break between the sites. **This is particularly common with the 1000GP VCF files.** With option `--discard-overlaps`, the construction replaces the alternate allele at the second site with a reference allele if the overlap cannot be resolved.
* Unphased genotypes cause two phase breaks: one before the site and another after the site. Unphased genotypes can be replaced with randomly phased ones by using option `--force-phasing`.
* Some VCF files list homozygous variants as unphased. By default, GBWT construction interprets them as phased. Option `--actual-phasing` can be used to consider such variants unphased.

If options `--discard-overlaps` and `--force-phasing` are both used, VG will typically generate chromosome-length haplotypes.

The size of the GBWT construction buffers can be changed to `N` million nodes with option `--buffer-size N` (default 100). The size should be increased for large chromosomes with a large number or variants and when generating chromosome-length haplotypes. The buffers typically use 6`N` to 8`N` megabytes. For best performance, buffer size should be tens of times larger than most paths (e.g. `--buffer-size 500` or `--buffer-size 1000` if typical path length is 20 million nodes).

# Indexes for `vg map` with a small graph

Small graphs (e.g. individual chromosomes or genomes smaller than 100 Mbp) are usually stored in a single `.vg` file. Indexing them is straightforward.

## Simple graph

If the graph does not contain complex regions (e.g. regions with many variants close to each other), we can usually index it directly.

### Without haplotypes

The following command builds both XG and GCSA indexes:

```
vg index -x graph.xg -g graph.gcsa graph.vg
```

### With haplotypes

At the moment, we can only build the GBWT index from a VCF file with phased genotypes. The construction requires that the VG graph contains paths corresponding to the variants. Such graphs can be built with the `-a` option:

```
vg construct -r reference.fa -v variants.vcf.gz -a > graph.vg
```

Once we have the graph, we can build all three indexes with the following commands:

```
vg index -x graph.xg -g graph.gcsa graph.vg
vg gbwt -x graph.vg -o graph.gbwt -v variants.vcf.gz
```

## Complex graph

If the graph is complex, we must prune (simplify) it with `vg prune` before building the GCSA index. Pruning uses heuristics to identify complex regions and removes then completely. If we have a reference, a set of paths, or a set of haplotypes, we can replace the pruned regions with simplified versions of them.

Pruning a graph requires enumerating all 24 bp paths in the graph. If the graph is too complex, this may take a very long time. In such cases, it is possible to speed up the pruning by first removing all high-degree nodes. This can be achieved with option `-M 32`, which removes nodes with degree over 32. 

### Without a reference or haplotypes

If there is no reference, we cannot avoid losing the complex regions completely from the index. We build the XG index for the original graph and the GCSA index for the pruned graph.

```
vg index -x graph.xg graph.vg
vg prune graph.vg > graph.pruned.vg
vg index -g graph.gcsa graph.pruned.vg
rm -f graph.pruned.vg
```

As the pruned graph is no longer needed, we can safely remove it.

### With a reference but without haplotypes

If we have a reference embedded as a path in the VG graph (e.g. from option `-r` in `vg construct`), we can restore it to the graph after pruning. The index will still lose all variation in the complex regions, but at least we have something in them.

```
vg index -x graph.xg graph.vg
vg prune -r graph.vg > graph.pruned.vg
vg index -g graph.gcsa graph.pruned.vg
rm -f graph.pruned.vg
```

Note that if the graph contains other paths in addition to the reference, they will be restored as well.

### With haplotypes

If we have a set of haplotypes, we can unfold each complex region into a set of paths corresponding to distinct haplotypes in that region. This way, we can include all observed variation in the GCSA index.

Again, we must have built the graph with option `-a`:

```
vg construct -r reference.fa -v variants.vcf.gz -a > graph.vg
```

We first build the XG index and the GBWT index. Then we prune and unfold the graph using the GBWT index. This produces a node mapping, which tells how to map the duplicated nodes on the unfolded paths to the corresponding original nodes. With the node mapping and the pruned graph, we can then build a GCSA index of the original graph.

```
vg index -x graph.xg graph.vg
vg gbwt -x graph.vg -o graph.gbwt -v variants.vcf.gz
vg prune -u -g graph.gbwt -m node_mapping graph.vg > graph.pruned.vg
vg index -g graph.gcsa -f node_mapping graph.pruned.vg
rm -f graph.pruned.vg node_mapping
```

### With many paths

We can unfold paths in a graph without a GBWT index This is primarily useful for graphs based on multiple alignment of sequences. Each of the sequences is embedded as a path in the graph, and pruning with `-r` would most likely just undo the pruning.

First we build an XG index for the original graph, storing the paths in the index. Then we prune the graph and unfold the paths, storing a node mapping as in the previous case. Finally we build a GCSA index using the node mapping.

```
vg index -x graph.xg graph.vg
vg prune -u graph.vg -m node_mapping > graph.pruned.vg
vg index -g graph.gcsa -f node_mapping graph.pruned.vg
rm -f graph.pruned.vg node_mapping
```

# Indexes for `vg map` with a large graph

Large graphs should be stored in multiple `.vg` files, e.g. in one file per chromosome. This saves memory, and many operations can also be parallelized to save time. On the other hand, working with the graph is often more complicated.

In the following examples, we assume a genome with chromosomes 1 to 22, X, and Y. There is a shared reference file `reference.fa` and a separate VCF file (e.g. `chr12.vcf.gz`) for each chromosome.

## 1. Graph construction

We build a separate `.vg` file for each chromosome. Option `-R` selects a region of the genome, while option `-C` confirms that the region is a chromosome, disabling heuristics that could make wrong guesses. Earlier instructions limited node size to 32 bases with `-m 32` to improve GCSA performance, but that is no longer necessary.

### Without haplotypes

```
for i in $(seq 1 22; echo X; echo Y); do
    vg construct -r reference.fa -v chr${i}.vcf.gz -R $i -C > chr${i}.vg
done
```

Because graph construction uses much less memory than index construction, we can run multiple jobs in parallel. With GNU Parallel, we run all 24 jobs at once with:

```
(seq 1 22; echo X; echo Y) | parallel -j 24 "vg construct -r reference.fa -v chr{}.vcf.gz -R $i -C > chr{}.vg"
```

### With haplotypes

Again, we need option `-a`:

```
(seq 1 22; echo X; echo Y) | parallel -j 24 \
    "vg construct -r reference.fa -v chr{}.vcf.gz -R {} -C -a > chr{}.vg"
```

Note that this is much slower than graph construction without `-a`: about 6 hours for the 1000GP human graph.

## 2. Node ids

After construction, each `.vg` file has its own node id space, and node ids overlap between the files. Before continuing, we first need to transform the graphs to a shared id space with `vg ids -j`.

### Without haplotypes

```
vg ids -j $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vg; done)
```

Running `vg ids -j` multiple times is not recommended, as subsequent runs create gaps in the node id space.

### With haplotypes or with many paths

We need to produce an empty node mapping to tell `vg prune` where the ids of duplicated nodes should start from:

```
vg ids -j -m mapping $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vg; done)
cp mapping mapping.backup
```

This takes around 3 hours for the 1000GP human graph.

`vg prune` modifies the node mapping, so it is better to have a backup if we have to prune the graphs again with different parameters. It is also possible to recreate the empty mapping with `vg ids -m` (without option `-j`), but it is much slower than using a backup.

## 3. GBWT construction

### Without haplotypes

We do not need a GBWT index if we do not have haplotypes.

### With haplotypes

First we need to collect all graphs into a single file. In this case, we use a temporary XG index that also stores the variant paths:

```
vg index -x graph-with-alts.xg -L $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vg; done)
```

This may take several hours. With the temporary XG, we can now build the final GBWT using a single command:

```
vg gbwt -x graph-with-alts.xg -o all.gbwt --num-jobs 14 -v \
    $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vcf.gz; done)
```

GBWT construction uses around 100 MB of memory for every 1 Mbp of sequence in the graph. Every indexing job uses two threads. If we have sufficient memory and CPU cores, we can run multiple jobs in parallel. For example, a system with 32 cores and 244 GB of memory can build the 1000GP GBWT using 14 jobs.

The total time is around 15 hours for the 1000GP graphs.

## 4. XG construction

We build a single XG index `all.xg` for the entire genome.

```
vg index -x all.xg $(for i in $(seq 1 22; echo X; echo Y); do echo chr${i}.vg; done)
```

## 5. Graph pruning

We must prune the complex regions of the graph before building a GCSA index.

Pruning a graph requires enumerating all 24 bp paths in the graph. If the graph is too complex, this may take a very long time. In such cases, it is possible to speed up the pruning by first removing all high-degree nodes. This can be achieved with option `-M 32`, which removes nodes with degree over 32.

### Simple graph

No pruning is necessary.

### Complex graph without a reference or haplotypes

```
for i in $(seq 1 22; echo X; echo Y); do
  vg prune chr${i}.vg > chr${i}.pruned.vg
done
```

Because pruning loads the VG graph in memory, memory usage is high: typically 100-200 MB for each 1 Mbp of sequence in the graph. Some parts are also multithreaded. Even though the pruning jobs are independent, parallelization options are limited on a single system. 

### Complex graph with a reference but without haplotypes

```
for i in $(seq 1 22; echo X; echo Y); do
  vg prune -r chr${i}.vg > chr${i}.pruned.vg
done
```

Note that if the graph contains other paths in addition to the reference, they will be restored as well.

### Complex graph with haplotypes

We start with an empty node mapping and update it with `-a -m`:

```
cp mapping.backup mapping
for i in $(seq 1 22; echo X; echo Y); do
  vg prune -u -g all.gbwt -a -m mapping chr${i}.vg > chr${i}.pruned.vg
done
```

Note that because all indexing jobs update the same node mapping, they must be run sequentially. Pruning the 1000GP graphs takes 3-4 hours.

### Complex graph with many paths

As with haplotypes, we start with an empty node mapping and update it with `-a -m`:

```
cp mapping.backup mapping
for i in $(seq 1 22; echo X; echo Y); do
  vg prune -u -a -m mapping chr${i}.vg > chr${i}.pruned.vg
done
```

Note that because all indexing jobs update the same node mapping, they must be run sequentially.

## 6. GCSA construction

We can finally build the GCSA index `all.gcsa` (with `all.gcsa.lcp`).

### Simple graph

```
vg index -g all.gcsa $(for i in $(seq 22; echo X; echo Y); do echo chr${i}.vg; done)
```

### Complex graph without haplotypes

```
vg index -g all.gcsa $(for i in $(seq 22; echo X; echo Y); do echo chr${i}.pruned.vg; done)
```

The pruned graphs (`chr*.pruned.vg`) are no longer necessary and can be safely deleted. On the other hand, we may want to keep them around if we need to rebuild the GCSA index.

### Complex graph with haplotypes or with many paths

We use the node mapping from `vg prune` to map the duplicate nodes to the corresponding originals:

```
vg index -g all.gcsa -f mapping $(for i in $(seq 22; echo X; echo Y); do echo chr${i}.pruned.vg; done)
```

GCSA construction takes around 30 hours for the 1000GP graphs.

The pruned graphs (`chr*.pruned.vg`) and the node mapping `mapping` are no longer necessary and can be safely deleted. On the other hand, we may want to keep them around if we need to rebuild the GCSA index.

# Other indexes

## GBWTGraph

We can quickly build a GBWTGraph for any GBWT index:

```
vg gbwt -x graph.xg -g graph.gg graph.gbwt
```

This makes most sense for a GBWT with haplotypes. If the GBWT is changed in any way, the GBWTGraph will most likely become invalid.

In **version 1.34.0**, we can also store the GBWT and the GBWTGraph in the GBZ format:

```
vg gbwt -x graph.xg -g graph.gbz graph.gbwt
```

## Distance index

Building a distance index requires finding all snarls, including trivial ones.

### Small graph

We first find all snarls with `vg snarls` and use option `-T` to include trivial ones:

```
vg snarls -T graph.xg > graph.snarls
```

We can also use `graph.vg` instead of `graph.xg`. With the snarls, we can now build the distance index:

```
vg index -s graph.snarls -j graph.dist graph.xg
```

Starting from **version 1.34.0**, we can also use GBZ graphs:
```
vg snarls -T graph.gbz > graph.snarls
vg index -s graph.snarls -j graph.dist graph.gbz
```

### Large graph

Snarl finding is quite memory-hungry, and it is often better to find them separately for each chromosome. We can simply concatenate the results into a single combined file:

```
rm -f all.snarls
for i in $(seq 1 22; echo X; echo Y); do
  vg snarls -T chr${i}.vg >> all.snarls
done
```

Afterwards, we build the distance index as before:

```
vg index -s all.snarls -j all.dist -x all.xg
```

## Minimizer index

Minimizer index construction requires a GBWT with haplotypes and a graph. If the graph is not a GBWTGraph, a temporary GBWTGraph will be constructed:

```
vg minimizer -g graph.gbwt -o graph.min graph.xg
```

If the graph is a GBWTGraph, this must be specified with option `-G`:

```
vg minimizer -g graph.gbwt -o graph.min -G graph.gg
```

In **version 1.34.0**, we can also use the GBZ format:

```
vg minimizer -o graph.min graph.gbz
```

If we are building the minimizer index for Giraffe, we should also annotate with distance information (option `-d`) for faster mapping:

```
vg minimizer -g graph.gbwt -d graph.dist -o graph.min -G graph.gg
```
