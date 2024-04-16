# General

The [[index construction guidelines|Index Construction]] work best with datasets that are no larger than the 1000 Genomes Project (2500 human samples). This page describes techniques that may be useful when trying to index larger datasets.

This page now assumes **VG version 1.29.0** or later and uses `vg gbwt` for GBWT construction. See [[VG GBWT Subcommand]] for further documentation.

# GBWT construction for many samples

## Haplotype identifiers

By default, GBWT stores haplotype identifiers at one out of 1024 positions on each path. The distance can be changed by using option `--id-interval N`. This is a time/space trade-off. Each stored identifier typically takes 3 to 4 bytes of memory, while the time required for determining the haplotypes containing a particular subpath is proportional to the distance between stored identifiers. In a 1000GP GBWT with the default distance, the stored identifiers take roughly as much space as the GBWT itself. If the number of samples is larger, the distance should also be proportionally larger.

## VCF parsing

Parsing a massive VCF file can take hours or even days. Because the parse does not depend on GBWT construction parameters other than `--force-phasing`, parsing the file only once and storing the intermediate data on disk may save time when trying different GBWT construction parameters. With option `--parse-only`, `vg gbwt` will output the intermediate data to files instead of building a GBWT index.

File `<prefix>_contig` will contain the reference path and the paths for each alternate allele for VCF contig `contig`, while files `<prefix>_contig_X_Y` will contain the phasing information for samples `X` to `X`+`Y`-1. These files can then be used as an input to the standalone GBWT construction tool `deps/gbwt/build_gbwt`.

In the following example, we index `graph-with-alts.xg`, which contains VCF contigs `chr1` and `chr2`. The output is written to `graph.gbwt`.

```
vg gbwt -x graph-with-alts.xg -o parse --parse-only -v variants.vcf.gz
build_gbwt -p -r -o graph parse_chr1 parse_chr2
```

## Merging GBWT indexes over the same contig

GBWT construction for tens of thousands of samples can be 2x to 3x faster, if we first build indexes for batches of e.g. 2500 samples in parallel, and then merge the partial indexes.

The default GBWT merging algorithm (`vg gbwt -m`) is mostly suited for merging small indexes, as it is slower than direct construction. The fast algorithm (`vg gbwt -f`) only works with GBWT indexes over different contigs. In order to merge GBWT indexes over the same contig quickly, we need the parallel algorithm (`vg gbwt -b`).

In the following, we first parse the VCF file and write the phasing information in batches to `parse_chr1_*`. Then we assume that the names of the phasing files have been split into 12 larger batches in files `chr1_1.txt` to `chr1_12.txt`. We build GBWT indexes for the large batches using 4 parallel jobs. Finally we merge the partial indexes using the parallel algorithm and write the result to `chr1.gbwt`.

```
vg gbwt -x chr1.vg -o parse --parse-only -v chr1.vcf.gz
seq 1 12 | parallel -j 4 "./build_gbwt.sh {}"
vg gbwt -b -o chr1.gbwt $(for i in $(seq 1 12); do echo chr1_${i}.gbwt; done)
```

Script `build_gbwt.sh` builds the GBWT index for the files in `chr1_$1.txt` in both orientations (`-r`) using 500-million node buffers (`-b 500`) and sample interval 16384 (`-s 16384`). By adding `-S`, we could instruct it to skip overlapping variants, effectively generating chromosome-length haplotypes.

```
#!/bin/bash
build_gbwt -r -b 500 -s 16384 -p -o chr1_$1 -L chr1_${1}.txt parse > /dev/null 2> /dev/null
```

The thread identifiers are determined by the order of the partial indexes in the arguments of `vg gbwt -b`.

Merging takes the first GBWT index as the base. Then, for each subsequent index in the argument list, it builds the rank array for that index relative to the base index, and interleaves the indexes according to the rank array. The merging is heavily parallelized and uses plenty of temporary disk space (2-3 bytes times the total length of the threads in the batch). By default, the temporary files are written to the current directory, but the directory can be changed with `-t DIR`.

# GCSA construction for a dense graph

In the 1000GP graph, the average distance between variants is around 35 bp. As the distance decreases, we will eventually cross a threshold, after which most of the graph consists of complex regions. If we prune the graph, the complex regions will be removed. If we have many samples, unfolding the haplotypes in pruned regions will result in too much node duplication, making GCSA constrution infeasible.

As long as the node id spaces of two graphs are compatible, VG can use the GCSA index of one graph with the other. We take advantage of this by indexing pruned graphs when the original graph is too complex to index. When we unfold the haplotypes after pruning, we ensure node id compatibility with the node mapping, while simultaneously guaranteeing that the index contains all kmers from the haplotypes. In a dense graph, we can achieve something almost as good: an index with all kmers from the haplotypes, except those containing an alternate allele of a rare variant.

## Simplifying the graph

We can remove the alternate alleles of rare variants from the graph without changing node ids by using `vg simplify`. In the following, we remove variants with alt frequency less than 0.001 from `graph.vg` and write the result to `graph.simplified.vg`.

```
vg simplify -v variants.vcf.gz -f 0.001 graph.vg > graph.simplified.vg
```

We can use occurrence counts instead of frequencies by replacing option `-f` with `-c`.

## Pruning the simplified graph

In order to unfold haplotypes in the simplified graph, we need a temporary GBWT index over the simplified graph. Because the simplified graph does not contain the paths corresponding to rare variants, we should add option `--ignore-missing` to suppress the warnings.

```
vg gbwt -x graph.simplified.vg -o simplified --parse-only --ignore-missing -v variants.vcf.gz

# Build graph.simplified.gbwt from the parse in simplified_contig*
```

Now we can unfold the haplotypes using the temporary GBWT index.

```
vg prune -u -g graph.simplified.gbwt -m node_mapping graph.simplified.vg > graph.simplified.pruned.vg
```

Now we have the files `graph.simplified.pruned.vg` and `node_mapping` we need for GCSA construction. The simplified GBWT index `graph.simplified.gbwt` is no longer useful.
