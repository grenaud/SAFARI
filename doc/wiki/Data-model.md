vg uses Protocol Buffers for internal data representation and serialization. In this page we describe the ProtoBuf schema we use and some conceptual ideas behind the API, without getting too deep into the specifics of the API.

### A quick ProtoBuf primer
ProtoBuf is a schema language, much like Apache Avro, FlatBuffers, Cap'n Proto, or many others. In essence, it's a human-readable language developed by Google that allows one to describe objects ('messages' in protobuf-speak). One writes their data model (or 'schema') in this language and then compiles it with a special compiler, `protoc`, which generates source code in a variety of common programming languages (C++, Python, Java, etc.). The generated source code contains getters and setters for fields in the schema. In essence, protobuf just makes it really easy to add/remove fields from a data model and port it into other languages.

### The vg protobuf schema
The vg protobuf schema can be found [here](https://github.com/vgteam/vg/blob/master/src/vg.proto). If you browse through it you'll see our messages and their nested fields. For example, there is a message called 'Alignment' that represents a read aligned to the graph - it's analogous to a SAM record. It has nested fields like 'sequence', 'quality', etc. We'll walk through these, as well as some additional C++ structures that support fast queries on them, in the following sections.

### Graphs, Nodes, and Edges

### Paths

### Alignments, Mappings, and Edits

### Pileups (NodePileup, EdgePileup, and plain old Pileup)

### Translations

### Genotype, Locus, and Support