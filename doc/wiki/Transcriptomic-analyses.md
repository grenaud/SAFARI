This wiki describes how to use `vg rna` and related tools for transcriptomic analyses. 

## Spliced pangenome graphs

The splicing structure of a gene can be represented as a graph, similar to how genomic variant information can be represented using a pangenome graph. 

<p align="center">
<img src="https://github.com/jonassibbesen/bioinformatics-diagrams/blob/master/spliced_reference_graph.png" width="600">
</p>

Here nodes and edges correspond to exons and splice-junctions, respectively. With transcripts represented as paths through the graph. Without the introns and intergenic regions these are also known as splicing graphs.

The spliced reference graph can be combined with a pangenome graph to produce a spliced pangenome graph containing both transcriptomic splicing information and genomic variant information. 

<p align="center">
<img src="https://github.com/jonassibbesen/bioinformatics-diagrams/blob/master/spliced_pangenome_graph.png" width="700">
</p>

Paths through this graph can still represent haplotypes and transcripts, but also now haplotype-specific (HS) transcripts (see *Pantranscriptomes* section).  

### Construction

We can use `vg rna` to construct spliced pangenome graphs by adding splice-junctions and optionally transcripts to an existing pangenome graph. This can be done using the following command:

```
vg rna -p -t <threads> -n annotation.[gtf|gff3] graph.pg > spliced_graph.pg
```

with the hereunder additional inputs and options available. For easy construction of spliced pangenome graphs and indexes using default parameters see [Automatic indexing for read mapping and downstream inference](https://github.com/vgteam/vg/wiki/Automatic-indexing-for-read-mapping-and-downstream-inference) wiki describing `vg autoindex`.

**Graph format:**  `vg rna` supports any of the handle graph implementations and will use the same format for the graph output as the input. It is, however, recommended that the *PackedGraph* format is used as it strikes a good balance between memory usage and graph edit speed. A graph can be converted to the *PackedGraph* format using `vg convert -p`.

**Transcript annotation:** `vg rna` supports both the GTF and GFF3 transcript annotation format. Note that all references (column 1) in the annotation must be part of the graph as embedded paths. By default only lines with the *exon* feature (column 4) will be parsed. This can be changed using `--feature-type`. In addition, the attribute tag (column 9) that will be used as the transcript id/name can be changed using `--transcript-tag` (default: *transcript_id*).

**Intron database:** Besides transcripts, a database of introns can also be added as splice-junctions to a graph. This can be done using `--introns`. The input format is BED with the start and end being the intron boundaries. Note that the strand (column 6) is also needed.

**Transcript paths:** Reference transcript paths can be added as embedded paths to the graph using `--add-ref-paths`. Reference transcript paths are transcripts that follow the reference paths defined in the annotation (column 1). See *Pantranscriptomes* section for more information on non-reference haplotype-specific (HS) transcript paths.

**Splicing graph:** By default `vg rna` will construct a spliced pangenome graph that includes the intergenic and intronic regions. If only the exonic regions (splicing graph) are of interest use `--remove-non-gene`. Note that all existing embedded paths will be deleted (including the reference). It is therefore recommended that reference transcript paths are added to the graph using `--add-ref-paths` (see above). 

## Pantranscriptomes

In a similar manner to the spliced pangenome graph we can also combine transcripts and haplotypes to construct a pantranscriptome. A pantranscriptome is a population-level transcriptomic reference consisting of a collection of haplotype-specific (HS) transcripts, which can be represented as paths in a spliced pangenome graph.

<p align="center">
<img src="https://github.com/jonassibbesen/bioinformatics-diagrams/blob/master/pantranscriptome.png" width="600">
</p>

### Construction

In addition to constructing spliced pangenome graphs we can also use `vg rna` to create pantranscriptomes by projecting reference transcript paths onto haplotypes. This can be done using the following command:

```
vg rna -p -t <threads> -n annotation.[gtf|gff3] -l haplotypes.gbwt graph.pg > spliced_graph.pg
```

with the hereunder additional inputs and options available. For easy construction of pantranscriptomes using default parameters see [Automatic indexing for read mapping and downstream inference](https://github.com/vgteam/vg/wiki/Automatic-indexing-for-read-mapping-and-downstream-inference) wiki describing `vg autoindex`.

**Haplotypes:** `vg rna` supports both haplotypes that are stored in a [Graph Burrows-Wheeler Transform (GBWT)](https://github.com/jltsiren/gbwt) index and haplotypes that are embedded as paths in the graph. To project transcripts onto all other (non-reference) paths in the graph use `--proj-embed-paths`. For more information on how to construct a GBWT of haplotypes see [VG GBWT Subcommand](https://github.com/vgteam/vg/wiki/VG-GBWT-Subcommand) wiki describing `vg gbwt`. 

**Graph type:** It is possible to create a pantranscriptome using a pangenome graph as input, thus constructing the spliced pangenome graph simultaneously. However, if a GBWT index with many haplotypes is used as input it is often more efficient to run `vg rna` twice by first constructing the spliced pangenome graph, then the haplotype GBWT against this spliced pangenome graph, and finally the pantranscriptome. The reason for this is that the GBWT needs to be updated when splice-junctions are added to the graph, which can be really slow. This three step process is automated by the `vg autoindex` command.   

**HS transcript paths:** By default `vg rna` will collapse identical HS transcript paths. This can be changed using `--do-not-collapse`. HS transcript paths can also be added as embedded paths to the graph using `--add-hap-paths`; however, this is generally not recommended for really large path sets.

**Pantranscriptome:** The resulting pantranscriptome consisting of HS transcript paths can be written to a GBWT using `--write-gbwt`. A bidirectional GBWT, storing each path in both directions, can be created using `--gbwt-bidirectional`. This can make downstream analyses using rpvg (see *Downstream analyses* section) more efficient if the reads are not strand-specific. The `--write-info` will create a TSV file with information on the transcript and haplotype origins of each HS transcript path in the pantranscriptome. This file is needed for downstream analyses using rpvg. By default `vg rna` will not output reference transcript paths as part of the pantranscriptome; however, this can be changed using `--out-ref-paths`.   

## Downstream analyses

All of the standard tools in the vg toolkit also works on spliced pangenome graphs. However, some tools have been optimized or designed specifically for transcriptomic analyses.

### RNA-seq mapping

To map RNA-seq reads to a spliced pangenome graph we recommend using `vg mpmap` as it has been specifically optimized for RNA-seq data. More information on how to run `vg mpmap` with RNA-seq data can be found at the [Multipath alignments and vg mpmap](https://github.com/vgteam/vg/wiki/Multipath-alignments-and-vg-mpmap) wiki.

### Transcript quantification 

[rpvg](https://github.com/jonassibbesen/rpvg) can be used to infer the expression of the haplotype-specific (HS) transcripts in a pantranscriptome. While not specifically part of the vg toolkit it is made by the same developers and uses the output from `vg rna` and `vg mpmap`. rpvg takes as input a spliced pangenome graph, read alignments in either *gam* or *gamp* format and a GBWT index pantranscriptome with HS transcript paths. 

**Read alignments:** It is our experience that the best quantification results are achieved using the default multipath alignment output (*gamp*) from `vg mpmap` as input to rpvg. 
