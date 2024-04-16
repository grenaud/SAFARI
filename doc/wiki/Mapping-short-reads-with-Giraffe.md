<!-- !test program bash -eo pipefail -->
This tutorial will explain how to use `vg giraffe` to map short reads to a pangenome graph.

The easiest way to start mapping to your own data is to have a single-copy reference FASTA, and a phased VCF(s) on several samples describing the variation you want to include. Your FASTA should not include alternative loci, because to use them properly you would also need to provide an alignment of the alternative loci to the main chromosome contigs, and vg cannot make sue of such an alignment alongside a VCF. Your VCF file(s) should be self-consistent, and not include contradictory or overlapping variants. They should also be restricted to VCF 4.2 features; the `*` allele of VCF 4.3 is not yet supported.

You can also use unphased VCFs, or a GFA file, as input, but in those cases Giraffe will not have real haplotype information and will instead map to greedily-selected covering paths of variable regions. This can produce bad mapping results when reads span multiple variants.

## Indexing with `vg autoindex`

To turn your FASTA and VCFs into a graph, you can use `vg autoindex`, passing the base FASTA with `-r` and the VCFs each with `-v`. You can set the base name of the output files with `-p`; it will default to `index`.

<!-- !test check Build and index graph with autoindex -->
```sh
cp ./wiki-data/mock-hs37d5/data/hs37d5.fa ./wiki-data/mock-hs37d5/data/hs37d5.fa.fai .
cp ./wiki-data/mock-hs37d5/data/*.vcf.gz ./wiki-data/mock-hs37d5/data/*.vcf.gz.tbi .
VCF_ARGS=()
for CHROM in {1..22} X Y ; do
    VCF_ARGS+=(-v chr${CHROM}.vcf.gz)
done
vg autoindex --workflow giraffe -r hs37d5.fa "${VCF_ARGS[@]}" -p hs37d5-pangenome
rm *.fa *.fa.fai *.vcf.gz *.vcf.gz.tbi
```

This will build all the files needed for Giraffe to run:

* GBWT haplotype index, augmented with a path cover of graph components without haplotypes and/or subsampled to a reasonable number of local haplotypes, in the `.giraffe.gbwt` file.
* GBWTGraph that provides node sequences in the `.gg` file.
* Minimizer index for seed finding in the `.min` file.
* Minimum distance index in the `.dist` file.

Starting from **version 1.34.0**, the first two are combined in a single `.giraffe.gbz` file.

### Alternative inputs

If you are working with a graph input in GFA format, instead of a FASTA and VCFs, you can use the `-g` option to provide the GFA as input to `vg autoindex`, instead of the `-r` and `-v` options. However, if you have phased VCFs, it is better to use them instead, because then Giraffe will have access to haplotype information when mapping.

### Troubleshooting

If you have trouble building indexes, you may need more memory. You can control the amount of memory that the autoindexing process seeks to use at any given time with the `--target-mem` option, but be aware that this is a target, and the heuristics used to estimate the memory requirements for building various partial indexes may not work well on your data.

## Mapping with `vg giraffe`

Once your indexes are built, you can then map reads using `vg giraffe`. For example, to map single-end reads into vg's GAM alignment format, you can do:

**Version 1.33.0**
```sh
cp ./wiki-data/mock-hs37d5/data/sim.fq .
vg giraffe -H hs37d5-pangenome.giraffe.gbwt -g hs37d5-pangenome.gg -m hs37d5-pangenome.min -d hs37d5-pangenome.dist -f sim.fq >mapped.gam
rm sim.fq
```

**Version 1.34.0 and later**
<!-- !test check Map to autoindexed graph with Giraffe -->
```sh
cp ./wiki-data/mock-hs37d5/data/sim.fq .
vg giraffe -Z hs37d5-pangenome.giraffe.gbz -m hs37d5-pangenome.min -d hs37d5-pangenome.dist -f sim.fq >mapped.gam
rm sim.fq
```

If you would prefer GAF-format output, add `-o gaf` to the command line.

If you have paired-end reads, add `-i` if they are interleaved, or add a second FASTQ file with another `-f` option.

## Evaluating with `vg stats`

To inspect the alignments for quality control, you can use:

<!-- !test in Analyze alignments -->
```sh
vg stats -a mapped.gam
```

For this test data (simulated directly from the base FASTA with no variants), this will produce:

<!-- !test out Analyze alignments -->
```
Total alignments: 20000
Total primary: 20000
Total secondary: 0
Total aligned: 20000
Total perfect: 20000
Total gapless (softclips allowed): 20000
Total paired: 0
Total properly paired: 0
Insertions: 0 bp in 0 read events
Deletions: 0 bp in 0 read events
Substitutions: 0 bp in 0 read events
Softclips: 0 bp in 0 read events
```

In real data, you should expect `Insertions`, `Deletions`, `Substitutions`, and `Softclips` all to be nonzero, and if your reads are paired you would want to see almost all your reads under `Total properly paired`

When you are done following along, you can clean up after yourself:

<!-- !test check Clean up -->
```sh
rm mapped.gam hs37d5-pangenome.*
```
