# Index types

This assumes vg **version 1.33.0** unless otherwise noted.

## Graphs

### XG

XG is a space-efficient immutable graph. It is typically used in mappers and other applications that do not modify the graph.

Multiple graphs can be combined into a single XG index with `vg index -x`. By default, the variants (alt paths) created with `vg construct -a` are not stored as paths in the XG index. They can be included with option `-L`.

A single graph can be converted into XG format with `vg convert -x`. 

The usual extension for XG files is `.xg`.

### GBWTGraph / GBZ

GBWTGraph is an immutable graph induced by the threads (lightweight paths) stored in an GBWT index. It uses the GBWT for graph topology and stores the sequences in the graph itself. Nodes and edges of the original graph not used on any GBWT thread are not included in the GBWTGraph. Any changes to the GBWT index make the GBWTGraph invalid.

GBZ is a wrapper that stores a GBWT index and the corresponding GBWTGraph. While the GBWTGraph file format corresponds closely to the in-memory representation of the data, GBZ compresses the sequences and is much more space-efficient. The GBZ format requires vg **version 1.34.0** or later.

GBWTGraphs can be built with `vg gbwt -g` (step 5 of the pipeline). Option `--gbz-format` stores the graph in GBZ format.

The usual extensions are `.gg` for GBWTGraph and `.gbz` for GBZ.

## Indexes

### GCSA

GCSA (sometimes called GCSA2) is an FM-index of a pruned de Bruijn graph that approximates the original graph. Queries up to a certain length (typically 256 bp) map perfectly to the original graph (or its subgraph if the graph was complex and had to be pruned). Longer queries may yield false positives.

In most applications, GCSA also needs the LCP array for additional functionality.

A GCSA index can be built with `vg index -g`. The inputs are typically pruned single-chromosome graphs.

The usual extensions are `.gcsa` for GCSA and `.gcsa.lcp` for the LCP array.

### GBWT

The GBWT is an FM-index that stores similar threads (lightweight paths) space-efficiently. In most applications, it is used for storing (real or artificial) haplotypes.

A GBWT index may also contain metadata with structured (sample, contig, haplotype, fragment) names for each thread. Threads with sample name `_gbwt_ref` are often interpreted as reference paths.

Steps 1-2 of the `vg gbwt` pipeline builds a GBWT index from several input types, including phased VCF (`-v`), GFA (`-G`), GBZ (`-Z`), embedded paths in a graph (`-E`), and GAM/GAF (`-A` / `-A --gam-format`).

The usual extension for a GBWT index is `.gbwt`. 

### Distance index

A distance index stores a hierarchical snarl decomposition of the graph and uses it for computing shortest distances between positions in the graph.

Building a distance index first requires finding the snarls with `vg snarls -T`. The index is then built with `vg index -j`.

The typical extension for a distance index is `.dist`.

### Minimizer index

A minimizer index stores a subset of kmers from the haplotypes and the corresponding graph positions in a hash table. The positions may also be annotated with distance information for faster distance index queries.

Minimizer indexes are built with `vg minimizer`. The construction requires a GBWT index with haplotypes and the original graph or the corresponding GBWTGraph.

The usual extension for a minimizer index is `.min`.

## Workflows

### `vg map`

The `vg map` workflow requires a graph (usually XG index) and a GCSA index with the LCP array. It may also uses a GBWT index with haplotypes for (potentially) more accurate alignment scores.

### `vg giraffe`

Giraffe requires the following indexes:

* GBWT index with haplotypes. The GBWT should be augmented with artificial haplotypes for decoys etc (`vg gbwt -a`). If the number of haplotypes is large (hundreds or more), it is usually better to downsample them (`vg gbwt -l`; also augments the result).
* The corresponding GBWTGraph.
* A distance index for the original graph or the GBWTGraph.
* A minimizer index for the GBWTGraph, annotated with information from the distance index.

The original graph (usually in XG format) must also be provided for output in SAM / BAM / CRAM format. If the original graph is provided, the GBWTGraph may be omitted, as it can be built quickly from the graph.

The minimizer index may also be omitted, as it can be built quickly.

