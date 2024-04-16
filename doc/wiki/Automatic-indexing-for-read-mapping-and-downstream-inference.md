## Preamble

The VG toolkit consists of many different tools, but one of the tasks it is most known for is read mapping. Indeed, VG contains three separate mapping algorithms.

1. [`vg map`](https://www.nature.com/articles/nbt.4227): the original, highly accurate mapping algorithm
2. [`vg giraffe`](https://www.biorxiv.org/content/10.1101/2020.12.04.412486v1): the much faster and still accurate haplotype-based mapping algorithm
3. [`vg mpmap`](https://www.biorxiv.org/content/10.1101/2021.03.26.437240v1): the splice-aware RNA-seq mapping algorithm that can produce [[multipath alignments|Multipath alignments and vg mpmap]]

Each of these mapping tools depends on a sizeable body of research in specialized data structures and algorithms. As a result, they each require a different set of indexes to be built before mapping. Historically, navigating the indexing process has been a pain point for our users. The indexing algorithms and their documentation are spread across several `vg` subcommands, and the steps need to be applied in a specific order to produce valid, usable results.

The `vg autoindex` subcommand is designed to alleviate the pain involved in this process. Rather than having an interface based on which index you want to produce, it has an interface based on which mapping tool you want to run. The inputs are all common interchange formats like FASTA, VCF, and GFA. Internally, `vg autoindex` has the logic of our best practice indexing pipelines built in. Power users might still find need for the individual indexing subcommands, but our goal is to produce indexes for any common use case in a single, easily-understood shell command.

## Examples

Most invocations for `vg autoindex` require very little configuration. The primary arguments are the `--workflow`, which indicates which mapper you want to run, and a set of interchange data formats.

#### Indexing for `vg map`

```
# starting from a FASTA and VCF
vg autoindex --workflow map --prefix /path/to/output --ref-fasta reference.fasta --vcf variants.vcf.gz

# starting from a GFA
vg autoindex --workflow map --prefix /path/to/output --gfa graph.gfa
```

#### Indexing for `vg mpmap` and downstream inference using `rpvg`

This invocation creates the indexes required for mapping with `vg mpmap` as well as indexes that are used for transcript inference using [`rpvg`](https://github.com/jonassibbesen/rpvg).

```
vg autoindex --workflow mpmap --prefix /path/to/output --ref-fasta reference.fasta --vcf variants.vcf.gz \
       --tx-gff transcripts.gff
```

#### Indexing for `vg giraffe`

`vg giraffe` is haplotype-based and performs best with phased variants, but indexes can still be constructed with an unphased VCF. 

```
vg autoindex --workflow giraffe --prefix /path/to/output --ref-fasta reference.fasta --vcf variants.vcf.gz
```

## Configuration for computational environment

Most computational parameters used in `vg autoindex` are determined automatically, but users may need to manually adjust a few of them. `vg autoindex` assumes that a fairly large amount of disk space will be available, so the `--tmp-dir` should be directed to a large volume. It is reasonable to expect the indexing for a eukaryotic pangenome to require 100s of GB of temporary storage and 10s-100s of GB of permanent storage (directed with `--prefix`). The `--threads` argument can limit the number of threads used for shared compute environments. Large jobs tend to be inconveniently slow with less than 8 threads. Finally, `vg autoindex` provides an interface to restrict memory usage by setting a target maximum memory occupancy (`--target-mem`). This target should be interpreted as an approximate limit (in general, it is hard to robustly predict future memory usage), but it can be decreased to encourage greater memory thrift at the expense of reduced ability to utilize available threads.