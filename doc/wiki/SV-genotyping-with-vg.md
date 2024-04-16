## Background
Structural variants are large-scale changes (commonly defined as >50bp) changes in the genomic sequence. Variation graphs are well-suited for encoding, visualizing and genotyping SVs.

## Construct an SV graph
SVs can be encoded in a variation graph in the same manner as any other type of variant.  Large insertions and deletions can likewise be encoded in VCF in the same way as smaller variants, by specifying the relevant sequences in the REF and ALT fields.  The VCF format also contains support for "symbolic alleles" for SVs.  Of these, `vg construct` supports `<INS>, <DEL>` and `<INV>`.

Graphs with symbolic alleles can be constructed with `vg construct -S`:  
```
vg construct -f -S -I insertions.fa -a -r ref.fasta -v vars.vcf.gz > graph.vg
```
This method requires a fasta and a bgziped + tabixed VCF file. `insertions.fa` is a FASTA file containing the sequences of insertions from the VCF (i.e. the allele fields in the VCF match the FASTA identifiers of the relevant sequence).

*Symbolic SV support is very sensitive to a variety of VCF INFO fields and, in part due to the looseness of the specification, often doesn't work on a given VCF. It is therefore recommended to just convert symbolic VCFs to explicit VCFs before construction, when possible, to avoid these pitfalls.*

*Note*: The `-f` and `-a` options are important whether or not `-S` is needed.

SVAHA2(https://github.com/edawson/svaha2) is another method for constructing SV graphs in the GFA format. It takes as its arguments a plain-text VCF and a fasta file:  
```          
svaha2 -r ref.fa -v vars.vcf > graph.gfa
```


**We recommend you use vg construct currently for structural variation calling.** 

## SV Genotyping with vg call

The first step is to compute read supports with `vg pack`, filtering by MAPQ>=5, ignoring first and last 5bp of each read:
```
vg pack -x graph.vg -g mapped.gam -o graph.pack -Q 5 -s 5
```
Then the snarls should be precomputed (if this step is skipped, vg call will compute them on the fly)
```
vg snarls graph.vg > graph.snarls
```
The VCF (that was used to construct the graph with `vg construct -a`) can then be genotyped from the GAM as follows
```
vg call graph.vg -k graph.pack -r graph.snarls -v vars.vcf.gz -s <SAMPLE> > genotypes.vcf
```
If an insertion fasta file was needed to `construct` with `-I`, it must be passed to `call` with `-i`.
