These are the steps you'd require to generate and map to a [[variation graph]] based on GRCh37 and the 1000G phase 3 release.

Two files are required, the reference [`hs37d.fa`](ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/technical/reference/phase2_reference_assembly_sequence/hs37d5.fa.gz) (700M) and the [p3v5b sites list from the 1000G](ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ALL.wgs.phase3_shapeit2_mvncall_integrated_v5b.20130502.sites.vcf.gz) (several G). I used a slightly older version that what is now available, `ALL.wgs.phase3_shapeit2_mvncall_integrated_v5.20130502.sites.vcf.gz`, but it does not matter for the purposes of this tutorial. You'll need a system with at least 64G of RAM (more won't hurt) to build the disk-backed rocksdb-based index, and 256G of RAM for the xg and GCSA2 based index.

To prepare, let's download the source files and unzip the reference:

```shell
# get the reference
wget ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/technical/reference/phase2_reference_assembly_sequence/hs37d5.fa.gz
# and the 1000G VCF
wget ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ALL.wgs.phase3_shapeit2_mvncall_integrated_v5b.20130502.sites.vcf.gz
# and its index
wget ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/ALL.wgs.phase3_shapeit2_mvncall_integrated_v5b.20130502.sites.vcf.gz.tbi
# unpack the reference
gunzip hs37d5.fa.gz
```

## Construction

In [[vg construct]], we take a VCF file and the reference sequence it was based on and generate a variation graph suitable for use by other tools in vg. In the construction process we interpret the VCF+ref combination as a [directed acyclic sequence graph](https://en.wikipedia.org/wiki/Directed_acyclic_graph). Variants in the input are decomposed using pairwise alignment between the reference and alternate, so that the input is interpreted as if it had been passed through [vcfallelicprimitives](https://github.com/ekg/vcflib#vcfallelicprimitives).

The construction process can be distributed, as we'll construct the whole genome graph one chromosome at a time. We specify which chromosome we want to work on by giving `vg construct` the `-R`/`--region` parameter, which limits the constructed graph to a particular reference range. Any samtools-format region specifier will work, but here we'll use whole chromosomes. Presently, it's advisable to break the construction apart due to memory requirements of vg--- it is optimized for high performance mutable graphs, and stores them in memory in a very non-succinct way.

Now we make a file `$i.vg` for each chromosome `$i`:

```bash
ref=hs37d5.fa
vars=ALL.wgs.phase3_shapeit2_mvncall_integrated_v5b.20130502.sites.vcf.gz

echo constructing graph
(seq 1 22; echo X; echo Y) | parallel -j 24 "time vg construct -C -R {} -r $ref -v $vars -t 1 -m 32 >{}.vg"
```

This step should take about half an hour when using 24 cores. The result will be around 4.5G of `.vg` files.

## Node ID coordination

Next, we'll need to generate a joint id space across each graph, since they were generated without coordination. The node id space that results from `vg construct` will be partially ordered based on the fact that a reference + VCF will generate a DAG. There are other ways to work around this (such as giving each graph a big node id space up front) but this seems to be the simplest solution. It will take another hour or so:

```bash
vg ids -j $(for i in $(seq 1 22; echo X; echo Y); do echo $i.vg; done)
```

Note that I'm passing multiple `.vg` files to `vg ids`. These are treated as if they define one big graph. You can pass multiple sub-graphs to `vg kmers`, `vg ids`, and `vg index`. Each graph will be loaded and processed serially, limiting maximum memory to the size of the largest graph in memory.

## Indexing with xg and GCSA2

Now that the `.vg` files contain a single graph with a coherent node id system, we can convert them into a single database that contains both the graph itself and kmers of the graph.

You will need 60G of disk to store this index pair, but several terabytes to build it (1.5T) plus a lot of I/O bandwidth. Both indexing processes will require a large amount of memory (up to ~200G for xg), and you may wish to distribute parts of the GCSA indexing over a cluster to complete in a reasonable amount of wallclock time.

We build the xg index in one step:

```bash
vg index -x wg.xg $(for i in $(seq 22; echo X; echo Y); do echo $i.vg; done)
```

It should take 2-3 hours. The resulting index is 15G (30G in older versions), and takes up about the same amount of space on disk and in memory. It also provides near constant-time queries for entities of interest in the graph.

The GCSA index requires graph simplification. We mask out high-complexity regions from the graph and replace them with paths corresponding to the reference sequence.

```bash
for chr in $(seq 1 22; echo X; echo Y);
do
    vg prune -r $chr.vg > $chr.pruned.vg
done
```

Although it masks regions of the graph, the complexity reduction does not destroy the id space. As such we can use the new graph to build an index for the whole genome graph.

```bash
vg index -g wg.gcsa $(for i in $(seq 22; echo X; echo Y); do echo $i.pruned.vg; done)
```

This takes about 32 hours on a 32 core system. The resulting GCSA2 index will be named `wg.gcsa` and the LCP array stored as `wg.gcsa.lcp`. In this case we've done 4 rounds of prefix doubling (16 to 32, 32 to 64, 64 to 128, and 128 to 256), so the index will allow queries of up to length 256. We need many terabytes of I/O to build the index, but only around 110G of RAM at peak.  **Important**: The location of the temporary files created for this process is specified using the TMPDIR environment variable. Make sure it is set to a volume a couple of terabytes of free space. 

## Alignment

With the index created, we can map reads against it index. In this case we'll use paired-end mapping based off of an interleaved fastq file. An easy way to generate one is to use `samtools sort -n` on an existing SAM/BAM/CRAM file, and then stream it into an interleaved fastq format with `samtools bam2fq`.

Assuming we use the xg/GCSA index on a 32-core machine, we can use this command line to run mapping:

```bash
# sort the CRAM file by read name so we can use the pairs in mapping
samtools sort -@ 16 -n aln.cram -o x \
    | samtools view -CT ~/graphs/hs37d5.fa - >aln.rnsrt.cram
# convert the alignments to interleaved fastq and map
samtools bam2fq aln.rnsrt.cram \
    | vg map -if - -x wg.xg -g wg.gcsa -t 32 -S 0 -u 1 -m 1 >aln.gam
```

## Errata

If you're interested in inspecting any of the results (graphs and alignments) check out `vg view`.

```bash
vg view -a aln.gam >aln.json
vg view 1.vg >1.gfa
```
You can chain commands together, for instance grabbing only a little bit of the reference using `vg find` and then converting it into something that can be visualized with graphviz.

```bash
vg find -p 7:10000000-10001000 -x wg.xg | vg view -d - >viz.dot
```