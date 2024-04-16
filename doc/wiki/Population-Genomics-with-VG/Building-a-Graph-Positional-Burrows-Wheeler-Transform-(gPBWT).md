# Building a Graph Positional Burrows-Wheeler Transform (gPBWT)

VG includes functionality for embedding information about haplotypes from a VCF into the XG index. Currently, this only works for directed acyclic vg graphs (such as those generated directly from a VCF).

This page will walk you through creating a VG graph with embedded alt information from a single-chromosome VCF file, creating the gPBWT-enabled XG index for that chromosome, and querying the gPBWT for the number of haplotypes matching a path.

## Download Data

First, download the VCF, VCF index, and reference sequence for this example. Make sure to decompress the FASTA file.

```
wget ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ALL.chr22.phase3_shapeit2_mvncall_integrated_v5a.20130502.genotypes.vcf.gz
wget ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ALL.chr22.phase3_shapeit2_mvncall_integrated_v5a.20130502.genotypes.vcf.gz.tbi
wget ftp://ftp.1000genomes.ebi.ac.uk//vol1/ftp/technical/reference/phase2_reference_assembly_sequence/hs37d5.fa.gz
gunzip hs37d5.fa.gz
```

## Build VG Graph with Alt Information

To build a gPBWT, a VG graph with embedded alt information is required. This information takes the form of specially named paths that trace the alts of each VCF variant used to construct the graph, and can be embedded in a VG graph using the `-a` option to `vg construct`.

```
vg construct -r hs37d5.fa -v ALL.chr22.phase3_shapeit2_mvncall_integrated_v5a.20130502.genotypes.vcf.gz -p -R 22 -C -a -m 1000 > chr22.vg
```

## Build gPBWT

The gPBWT is built from the annotated VG graph created above and the original VCF file (from which haplotype information is extracted), which is specified with the `-v` option to `vg index`. The gPBWT is saved as a component of the XG index.

```
vg index -x chr22.vg.xg -v ALL.chr22.phase3_shapeit2_mvncall_integrated_v5a.20130502.genotypes.vcf.gz -p chr22.vg
```

## Obtain Alignments

The gPBWT is used to count the number of haplotypes in agreement with the paths taken by Alignment objects, as read from a GAM file. To exercise the gPBWT, we first need to generate some Alignments to the graph with `vg sim`.

Here, we make sure to filter the generated Alignments to remove those with runs of Ns, as Alignments generated in sequence gaps are not particularly informative.

```
vg sim -x chr22.vg.xg -n 1000000 -s 1337 -l 100 -aJ | grep -v NN | vg view -JGa - > test.gam
```

## Count Matching Haplotypes

For each Alignment, we can quickly obtain the number of haplotypes in the gPBWT that have the Alignment's Path as a subhaplotype, using the `-H` option of `vg find`. The resulting file will contain the number of haplotypes containing each Alignment's Path, one per line, in the order that the Alignments appeared in the input GAM.

```
vg find -x chr22.vg.xg -H test.gam > counts.txt
```