Basic operations
---------
It is useful to think of `vg` as a toolbox for manipulating the internal variation graph representation. Almost any goal can be expressed in terms of operations on the graph; while developing this way of thinking initially requires some mental gymnastics, in the end it makes the user's life simpler by abstracting their problem out of the biological context and into a more general one. For example, variant calling can be expressed as the series of `construct`, `index`, `map`, and `call` operations, with inputs and outputs along the way.

```
# Construct
vg construct -r reference.fa -v variants.vcf.gz > graph.vg 

# Index
vg index -x graph.xg -g graph.gcsa graph.vg 

# Map
vg map -f reads.fq -g graph.gcsa -x reads.xg > mapped.gam

# Pileup/Augment
vg augment -a pileup -Z samp.trans -S samp.support graph.vg mapped.gam > samp.aug.vg 

# Call
vg call -z samp.trans -s samp.support samp.aug.vg > calls.vcf 
```
            
Let's walk through a basic `vg` pipeline like the one above step-by-step to help illustrate what all it can do.

![The vg pipeline, as a flowchart diagram](https://github.com/vgteam/vg/raw/master/doc/figures/pipeline_flowchart.svg)

## First things first: build your graph
There are two main ways to build a graph in vg. If you have a reference genome, you should generally start with `vg construct`. 

At its most basic, `construct` takes only a reference genome with the `-r` flag. This generates a linear graph, completely analogous to the linear reference used for input.

```
vg construct -r reference.fa > ref.vg # a 'flat' graph, in vg parlance. One that contains no bubbles.
```

While all of the operations `vg` will operate on this graph, it wouldn't be particularly interesting (`bwa` could do the same things). What we would like to do is use the prior information from a population (e.g. as variant calls in a VCF) as a prior for mapping our reads. We hope that by doing so we'll get better mappings and better rediscovery of known variants. We can create such a graph like so if we have a VCF containing calls to the same reference we use as input.

We need to gzip compress and tabix-index our VCF, if not done already, like so:

```
bgzip variants.vcf && tabix variants.vcf.gz
```

Than we can run `vg construct` on the reference and the variants:

```
vg construct -r reference.fa -v variants.vcf.gz > graph.vg
```

The `reference.fa` file also needs an index, `reference.fa.fai`, but that file will be created by `vg construct` if it is not already present.

### Detour: Using `vg msga` instead of `vg construct`

If you don't have a reference genome but do have some polished long reads, you could try a progressive assembly using `vg msga`.

```
vg msga -f polished_contigs.fa > graph.vg
```

We'll save the [details of `msga` for another wiki page](https://github.com/vgteam/vg/wiki/Long-read-assemblies-using-vg-msga), but in short we can imagine it doing the following steps:

1. Make a 'flat' graph of the first read.
2. Align the second read to this graph.
3. Incorporate new paths introduced by the second read into the graph.
4. Reindex the graph and continue the process with reads 3, 4, 5...N.

We've now seen how to create graphs, both flat ones and those containing reference variation. Now we'll talk about indexing, an essential step if we want to map reads.

## Index your graph
Indexing a graph is directly analogous to indexing a linear reference with `bwa index`; we need to create ways to efficiently look up entities in the graph if we want to map reads.

### The xg index
`xg` format that can store a variation graph - nodes, edges, and paths - in a compressed self-index that takes up much less memory. This allows us to fit the graph structure in memory, speeding up the process of mapping reads. It also allows us to efficiently query paths in the graph. We need the xg index for mapping, and we construct it with the following command: 

```
vg index -x graph.xg graph.vg
```

### The GCSA2 index
GCSA2 stands for "Generalized Compressed Suffix Array", but you can think of it as a graph BWT index like that used by `bwa`. It allows fast lookup of the locations in the graph where specific subsequences are present - for example, if I wanted to know where the sequence "GATTACA" was in my graph, I could query the GCSA2 index, and it would provide me the location of the "GATTACA" sequence. The GCSA2 index requires a kmer size parameter as it's based on a pruned de Bruijn graph. We can construct a GCSA2 index like so:

```
vg index -g graph.gcsa graph.vg
```

Note that this will create a `graph.gcsa` **and** a `graph.gcsa.lcp` file; Although you only have to pass vg the name of the `.gcsa` file, both files must exist next to each other with the correct extensions when you use the index.

#### GCSA-indexing large and complex graphs

For graphs with dense variation, such as whole-genome graphs derived from the 1000 Genomes data, you may not be able to build the GCSA for all kmers in the graph, because of combinatorial interactions between adjacent variants. In this case, you will need to prune the graph before indexing it, so that only a non-combinatorially-exploding subgraph of each complex region is indexed.

```
vg prune -r graph.vg > graph.pruned.vg
vg index -g graph.gcsa graph.pruned.vg
```

For more hints on working with large graphs, including examples of how to parallelize over chromosomes, see [[Working with a whole genome variation graph]], or use [toil-vg](https://github.com/vgteam/toil-vg) to orchestrate your workflow.

### Detour: The GBWT Index

If your input VCF file contains phased haplotype information, you can store that information in a GBWT index and use it to inform read mapping. The [GBWT, or Graph Burrows-Wheeler Transform](https://github.com/jltsiren/gbwt#gbwt), is a compact format for storing large numbers of haplotypes, expressed as paths through a graph, in space that grows sublinearly in the number of stored haplotypes.

You can build a GBWT index by using this instead of your XG indexing command:

```
vg index -x graph.xg -G graph.gbwt -v variants.vcf.gz graph.vg
```

Because the GBWT indexing process relies on information extracted from the graph during the XG indexing process, currently the two indexes must be generated together.

#### Sub-detour: Pruning with Haplotypes

Once you have a GBWT, you can use it to inform graph pruning when building the GCSA index. Haplotypes from the GBWT are used to contain combinatorial explosions of kmers caused by adjacent variants.

```
vg prune -u -g graph.gbwt graph.vg > graph.pruned.vg
vg index -g graph.gcsa graph.pruned.vg
```

## Mapping reads
Okay, we've now created a graph and generated our indices for mapping reads, so let's map some.

To map reads using a maximal exact match (MEM) algorithm, we call `vg map`. The exact syntax to use depends on whether your input reads are paired, and, if they are paired, how they are stored.

```
## Single end reads in a single FASTQ
vg map -x index.xg -g index.gcsa -f reads.fq > mapped.gam
## Paired end reads interleaved in a single FASTQ
vg map -x index.xg -g index.gcsa -f reads.fq -i > mapped.gam
## Paired end reads in paired FASTQs
vg map -x index.xg -g index.gcsa -f reads1.fq -f reads2.fq > mapped.gam
```

The mappings are output in GAM format, an analogue of BAM for graphs.

If you generated a GBWT index of haplotypes, you can use it to inform your read mapping by adding `--gbwt-name graph.gbwt` to the `vg map` command line, giving the filename of the GBWT haplotype index.

## Calling variants with `vg call`
There are two variant callers in `vg`. The `vg call` variant caller relies on a `samtools pileup` style structure, but is more well-tuned and highly tested, and is currently the recommended variant caller. There is also `vg genotype`, which uses a more FreeBayes-like approach to variant calling, but it is harder to get good results with.

`vg call` deserves its own wiki page, so we'll just show some quick commands here. Briefly the graph is first augmented with variants suggested by the read pileup using `vg augment`. `vg call` then uses this new graph and the statistics about the read pileup to generate variant calls.

```
vg augment -a pileup -Z samp.trans -S samp.support graph.vg mapped.gam > samp.aug.vg
vg call -z samp.trans -s samp.support samp.aug.vg > calls.vcf
```

You can also add the `-b graph.vg` option, specifying the original graph. If you do, it will generate a (non-standard) `XREF` tag in the output VCF, to distinguish variants that it calls that were substantially present in the graph reference, and are not newly discovered in the sample. **This option is required in vg 1.11 and below.**

## Detour: Calling variants with `vg genotype` instead of `vg call`

`vg genotype` relies on the graph itself and an index of the GAM that allows for efficient access to reads by graph position. It works by inserting all paths implied by reads into the graph and then looking for bubbles, then doing some logic to determine which side(s) of the bubble the reads support.

### The GAM index
The GAM index allows us to find which alignments in our GAM map to specific nodes/edges and vice versa. We need this index for genotyping, as we perform this query in that command's source code. Here's how we make one:

```
vg index -d mapped.gam.index -N mapped.gam
```

This will produce an index call `mapped.gam.index`, which shows up on your file system as a directory.

### Actually genotyping

You can run `vg genotype` like this:

```
vg genotype -v graph.vg mapped.gam.index/ > calls.vcf
```

This will barf a bunch of info to the terminal that relates to support for individual calls. It will also produce a VCF, but the coordinates won't be sorted, so you might want to sort them with `vcflib`.

At this point, you've gone from a reference genome, some population variation and a set of reads to final variant calls. Next, you can read [[Index Construction]] for more detailed information on how indexes are created, or [[Working with a whole genome variation graph]] for a walkthrough of how to construct, index, and align to a genome-scale graph.

