## Whole genome genotyping

A graph created from `whole-genome-vars.vcf.gz` with `vg construct -a` (the `-a` is crucial) can be used to genotype variants of all sizes from a GAM file.  Chunking, as described below, can be used but is typically not necessary for this case. The procedure is as described in the [README](https://github.com/vgteam/vg/blob/master/README.md)

Note: we assume an XG index was created with `vg index -L -x` (the `-L` is crucial).

```
# Compute the read support
vg pack -x whole-genome.xg -a whole-genome.gam -o whole-genome.pack -Q 5

# Compute the snarls (using fewer threads with `-t` can reduce memory at the cost of increased runtime)
vg snarls whole-genome.xg > whole-genome.snarls

# Genotype the VCF
vg call whole-genome.xg -r whole-genome.snarls -k whole-genome.pack -s <sample> -v whole-genome-vars.vcf.gz > genotypes.vcf
```
## Whole genome calling

### Calling the entire graph at once

This is a simpler but slower method than that outlined in the next section.  The results should be identical.

Calling variants from the graph requires augmenting the graph from the reads.  This step cannot be performed on an XG index, as its read-only.  In addition, VG-format graphs are too inefficient.  Until the entire toolset is updated to use a more efficient format, the first step is to convert the graph into packed-graph format.

```
# convert the graph (can be vg or xg)
vg convert whole-genome.xg -p > whole-genome.pg
```

Now the graph can be called by augmenting it before running `vg call`.  It is important to use `vg augment -m` to set a minimum coverage.  The value should be chosen based on the average coverage of the GAM (obtainable by `vg depth`).  Typically, a value of about 10% of the depth is reasonable.  An adaptive version that chooses it automatically is in the works.  We will assume 30X coverage and use `-m 4` below. 

```
# augment the graph, filtering out mappings and bases with quality < 5, and breakpoints with coverage < 4
vg augment whole-genome.pg whole-genome.gam -m 4 -q 5 -Q 5 -A whole-genome-aug.gam > whole-genome-aug.pg

# compute the snarls
vg snarls whole-genome-aug.pg > whole-genome-aug.snarls

# compute the support
vg pack -x whole-genome-aug.pg -g whole-genome-aug.gam -o whole-genome-aug.pack

# call variants on every path in the graph
vg call whole-genome-aug.pg -r whole-genome-aug.snarls -k whole-genome-aug.pack -s <sample> > whole-genome-calls.vcf 
```

### Calling the graph by first splitting into components

`vg augment` can take upwards of 10 hours on human-genome sized graphs.  When resources are available it can be faster to parallelize this step by splitting the graph into components.  Below, we will assume a graph created using `vg construct` in which case each reference path will correspond to a connected component.  This is not a requirement, however.  Connected components can be computed by `vg chunk` without using paths by using `-C` instead of `-M`.

Splitting the graph into components (chromosomes in this case) can be done as follows. `-M` makes a component for each path in the graph.  A subset can be selected by, ex, replacing `-M` with `-C -p chr1 -p chr2`
```
vg chunk -x whole-genome.xg -M -O pg
```
If the paths in the graph are of the form `1 2 3 4 ... X Y`, this command will result in files `chunk_1.pg, chunk_2.pg ... chunk_Y.pg` being created in the current directory. And each one can be called (using the whole-genome GAM) as described in the previous section, in parallel if desired.

```
# augment the graph, filtering out mappings and bases with quality < 5, and breakpoints with coverage < 4
# the -s option is crucial
vg augment chunk_1.pg whole-genome.gam -s -m 4 -q 5 -Q 5 -A chunk_1_aug.gam > chunk_1_aug.pg

# compute the snarls
vg snarls chunk_1_aug.pg > chunk_1_aug.snarls

# compute the support
vg pack -x chunk_1_aug.pg -g ahunk_1_aug.gam -o chunk_1_aug.pack

# call variants on the component
vg call chunk_1_aug.pg -r chunk_1_aug.snarls -k chunk_1_aug.pack -s <sample> > 1.vcf 
```

On a distributed computing system, it may be inefficient to copy the whole-genome GAM to each augment job. If desired, the GAM can be split into components as well by passing it to `vg chunk` by adding `-g -a whole-genome.gam` to its invocation.  This will create `chr_1.gam, chr_2.gam etc.` that can be used in place of whole-genome.gam in the above steps.  







