This page gives an overview of `vg sim` and its usage. Note that this process is automated in [toil-vg](https://github.com/vgteam/toil-vg) and we especially recommend using toil-vg if working with large graphs. 

Most of the time we might want to simulate reads from one of the sample that was used to create the graph. To do this we need to index each haplotype using the GBWT. This index will be used to identify the paths in the graph corresponding to the haplotype or sample we want to simulate the reads from.

## Constructing a graph and indexing haplotypes

Assuming that *x.fa* contains reference sequence for a chromosome *x* and *variants.vcf.gz* contains variants for multiple samples including the sample for which we want to simulate reads (later called *SAMP*).

```sh
vg construct -r x.fa -v variants.vcf.gz -a -f -m 32 > graph.vg
vg index -x graph.xg -G graph.gbwt -v variants.vcf.gz graph.vg
```

## Simulating reads from a specific sample

We can simulate reads from a specific sample with options `-g`/`--gbwt-name` and `-m`/`--sample-name`:

```sh
vg sim -x graph.xg -g graph.gbwt -m SAMPLE -n 1000 -l 150 -a > SAMPLE.gam
```

By default `vg sim` outputs reads in a text format. Using `-a` it outputs reads in the GAM format that we can then convert into FASTQ using `vg view -X`.

## Paired-end reads

We can specify the fragment size and its standard deviation using the `-p` and `-v` arguments. For example:

```sh
vg sim -x graph.xg -g graph.gbwt -m SAMPLE -n 1000 -l 150 -p 500 -v 50 -a > SAMPLE.gam
```

## Providing a real FASTQ file for error modeling

A FASTQ file can be used to model the error profile using the `-F` argument. For example:

```sh
FASTQ=ftp://ftp-trace.ncbi.nlm.nih.gov/ReferenceSamples/giab/data/NA12878/NIST_NA12878_HG001_HiSeq_300x/131219_D00360_005_BH814YADXX/Project_RM8398/Sample_U5a/U5a_AGTCAA_L002_R1_007.fastq.gz

vg sim -x graph.xg -g graph.gbwt -m SAMPLE -n 1000 -p 500 -v 50 -F $FASTQ -a > SAMPLE.gam
```

Note that in that case the read length argument (`-l`) is not necessary.
