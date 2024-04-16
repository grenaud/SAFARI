### A unified representation of variants
In vg, there is almost no difference in the way variants of any size are represented. It is best to first think about our reference, which is Path in the graph, or a string of nodes connected by edges in a linear fashion.


Any variants relative to the reference path create bubbles within the graph. A bubble represents alternative paths between two nodes in the graph. Bubbles may contain two or many paths between their entrance/exit nodes. A variant's size does not affect the way it appears in the graph - if we did not impose a limit on the number of basepairs in a node, the graph structure representing a very large SV and a single SNP would be indistinguishable.

### Paths and Snarls and SnarlTraversals
Paths in vg are annotations on Nodes of the graph. This may include mismatches, insertions, and deletions relative to the basepairs of the node. Paths are permitted to be empty (i.e. in the case of a flat genomic deletion), however these empty paths are not useful in practice as they do not provide any coordinate reference to the graph. Paths are composed of Mappings, which are a position (A node ID and a basepair where the mapping begins within that node) and a list of Edits (the operations performed to transform the reference sequence to that described by the path). Tracing along the path's Mappings and replacing the "from" field of each Edit with its "to" field would yield the sequence the path intends to spell out.

Paths in vg only represent alleles, excluding the start and end nodes of a bubble.

Snarls are equivalent to bubbles in the graph - they have a pair of nodes that define an entrance and exit, as well as a set of child nodes / snarls that describe their inner structure. SnarlTraversals are exactly equivalent to Paths, except they are restricted to only have perfect matches to the underlying graph (i.e. no Edit fields exist nor would they be permissible except as exact matches). SnarlTraversals must be associated with a Snarl.

Snarls and SnarlTraversals solve the problem of empty paths. By requiring an entrance/exit for each Snarl, we can represent a deleted genomic region as a SnarlTraversal containing no nodes but associated with a Snarl describing the flanking entrance/exit nodes.

### Flat vs. Regular ("parsed" or "aligned") alleles
Flat alleles require that every allele in the graph must be represented by at least one base. This allows as to label deletions, rather than using empty paths.
