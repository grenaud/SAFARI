Formats
--------

vg has a variety of formats, some unique to the world of graphs and others directly analogous to familiar legacy formats. We present them here in a pseudo-glossary to make working with them understandable.

## A note on Protobuf quirks
Protocol Buffers are used by many of the vg file formats. There are a few things that frustrate first-time viewers that we'll address here. First, protobuf is binary, so you can't just use `less` to view it; use `vg view` and the relevant flags.

**In addition, when a protobuf field has a value of 0 or is unset, it won't show up in the output. This can make parsing a bit of a pain if you aren't aware.**

## .vg (ProtoBuf graph format)
The backing structures of vg are [Protobuf](https://developers.google.com/protocol-buffers/) messages. Protobuf is a schema language that allows us to easily develop our data model, create C++ source code for it, and serialize/deserialize it to and from disk. It is binary data (and often compressed), so don't bother trying to look at it with `less` - we suggest using the `vg view` command to check out a graph in GFA or even as a pdf using `graphviz`.

The `.vg` format is the actual graph object, just stored in a file. We use the graph for things like `vg view`, and we can modify it with `vg mod`. We often operate on graph indices though for the sake of efficiency.

## Special graphs terminology
1. The 'reference graph' : We use the phrase reference graph to describe a graph containing a reference genome and variation to that reference (i.e. a FASTA and a VCF).

2. The 'flat graph': A flat graph contains only a reference genome. It's 'flat' in the sense that because there are no bubbles; the graph itself resembles pearls on a string.

3. The 'augmented graph': We use this to describe a graph which has had reads mapped to it, then had all paths implied by those reads inserted into the graph. This means if you mapped the same reads to an augmented graph every read would (in theory) map perfectly - all variation and error included in the reads is now included in the graph. We use the augmented graph as the basis for `vg genotype`.

4. The 'sample graph': A variation graph containing variation from one or several samples. These may be from an assembly of several genomes, for example, or a single flat graph augmented with the reads from a single sample.

## .xg (XG lightweight graph / path index)
[XG](https://github.com/vgteam/xg.git) can be thought of as a succinct (i.e. lightweight or low-memory) representation of the graph it indexes. The xg index contains the paths of the graph and the nodes and edges, though not their sequences; the structure of the graph is conserved but the actual sequence is removed.

XG uses succinct data structures to accomplish this, and the translation between the graph and these structures is a bit complicated so we'll leave its description to elsewhere in the wiki. However, the basic idea is this - you can perform queries like getting the distance between two nodes, or get adjacent nodes, or find the in degree of a node, all using just the xg index of a graph. You'll use the xg index for mapping reads but also for a variety of functions that leverage its speed compared to operating on the raw graph.

## .gcsa (GCSA generalized compressed suffix array index)
The GCSA/GCSA2 index is equivalent to the `.sa` file produced by `bwa index`. This contains a suffix array which allows quick lookup of where specific sequences are in the graph. This allows us to map reads against the graph, and this is where you'll see the GCSA index come up.



## .gam (Graph Alignment / Map, `vg`'s BAM)
GAM is `vg`'s BAM equivalent, providing the relevant information to determine where a sequence (e.g. a read) mapped in the graph. It supports paired end reads by interleaving them in the GAM. It is again usually in binary (and compressed) form, but a quick `vg view` will expose what it looks like.

GAM can also be transformed into JSON records, with the following characteristics:
- One record (i.e. one read) per line, which is one JSON object.
- All fields set to false / 0 / NULL do not appear in JSON output.


## .gam.index (An on-disk index for GAM)
GAM is great, but there is not really an implicit order of sequences in the format. We use the GAM index to query reads without having to loop over the entire GAM for each query. For example, say we'd like a way to get all the reads mapping to a specific position or sort reads by position like BAM does. We can accomplish these with the GAM index.

Under the hood, the GAM index is a RocksDB database. RocksDB is a disk-based key-value store (essentially a map). It's nice because it keeps us from having to put a GAM in memory, which would cause serious problems for most machines, but unfortunately it can be a little slow since we have to go to and from disk.

## .dot (DOT format for graphtools viz)
DOT is a standard format used by graphtools, a package for graph visualization designed by AT&T labs a long, long time ago. It's kind of human readable, but it's best used as an intermediate between vg and a PDF or SVG file.


## GFA (Graph Fragment / Assembly format)
[GFA](https://github.com/GFA-spec/GFA-spec) is a format that's designed to allow data interchange between programs which operate on sequence graphs. These might include assemblers; pangenomics programs like vg; toolkits for manipulating the format like RGFA, gfatools and gfakluge; or visualization programs like Bandage. By allowing interoperability between these different programs GFA enables some really cool workflows - imagine performing a *de novo* assembly, outputting it as a graph in GFA, then using `vg` to map reads against it before visualizing with Bandage. GFA is a big step toward building an entire community and ecosystem of graph-based tools.

GFA is sort of human readable; check out the docs for a proper schema description.



## `vectorize` output (vectors and vowpal_wabbit formats for input to machine learning algorithms)
There is a lot of information contained in reads mapped to a graph. We can use mappings to a graph as the basis
for machine learning algorithms using `vg vectorize`. In essence, a mapping to the graph describes some information about the genotype of a sample. We can associate specific mappings (e.g. a causal SNP) with class labels (e.g. phenotypes) and train learning algorithms to recognize reads supporting specific labels based on their mappings.