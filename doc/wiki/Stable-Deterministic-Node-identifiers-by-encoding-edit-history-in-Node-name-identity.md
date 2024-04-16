The nodes are named by the edits required to the original paths. We realise as the biology is unstable, stable identifiers are not truly possible but we should have as stable as possible identifiers as possible and each identifier should have its provenance associated with it. The following proposal is a solution to this problem:

We are have a reference assembly ACTGA named r1. As a variant graph we end up with one node.
We can name this node after the reference, where it starts and how long it is.
Giving an implicit name of r1:0:5 (original sequence:starting:ending)

![Start with a sequence ACTGA](https://chart.googleapis.com/chart?chl=+graph+r1+%7B%0D%0A+++++chr38%3A0+%5Btype%3Ds%2C+label%3DACTGA%5D%3B%0D%0A+%7D%0D%0A++++++++&cht=gv)

Then add a second sequence AATGA, lets call that patient1 or p1.

![Then add a second sequence AATGA](https://chart.googleapis.com/chart?chl=+graph+r1+%7B%0D%0A+++++r1%3A0+%5Btype%3Ds%2C+label%3D%22r1%3A0-5%3DACTGA%22%5D%3B%0D%0A+++++p1%3A0%5Btype%3Ds%2Clabel%3D%22p1%3A0-5%3AAATGA%22%5D%3B%0D%0A+%7D%0D%0A++++++++&cht=gv)

So we have two nodes, r1:0:5 and p1:0:5, this would be turned into four nodes in a VG graph.
However we name the nodes as much as possible by where they originated in the graph, giving preference to modifying nodes that where already in the graph.
![This would generate the following variant graph](https://chart.googleapis.com/chart?chl=+graph+r1+%7B%0D%0A+++++r1_0_1+%5Btype%3Ds%2C+label%3D%22r1%3A0-1%3DA%22%5D%3B%0D%0A+++++r1_1_1+%5Btype%3Ds%2C+label%3D%22r1%3A1-1%3DC%22%5D%3B%0D%0A+++++r1_2_3+%5Btype%3Ds%2C+label%3D%22r1%3A2-3%3DTGA%22%5D%3B%0D%0A+++++p1_1%5Btype%3Ds%2Clabel%3D%22p1%3A1-1%3AA%22%5D%3B%0D%0A+++++r1_0_1+--+r1_1_1+.%0D%0A+++++r1_1_1+--+r1_2_3+.%0D%0A+++++r1_0_1+--+p1_1+.%0D%0A+++++p1_1+--++r1_2_3+.%0D%0A+%7D%0D%0A++++++++&cht=gv)

### Computational considerations

Inside VG we keep using integer identifiers, only when serialising do we write out the long names.
The edit history must be saved, but vg can write these to disk e.g. like a write ahead log as used in databases.
Reading the log from future to past allows vg to build these identifiers on demand.
The integers to long names can be a simple int to string map that is written to disk.

### Source for URIs in the RDF representation

In the RDF world we would have names like

```
http://www.ncbi.nlm.nih.gov/projects/genome/assembly/grc/38/chromosome/11/0:6000
```
and
```
http://www.ncbi.nlm.nih.gov/projects/genome/assembly/grc/38/chromosome/11/6000:248950422
```
as well as 
```
http://www.example_hospital/sample/XXXXXX/11/6002:248956422
```
The first would be for a node that goes from the zeroth base to base 6000, on the grc38 chromosome 11 sequence. 
The second would follow the first and go from the 6000th base to base 248950422 again on chromosome 11 of cr38.
The third would represent a node that is unique on a patient. 


So we would prefix the authority of the sequencer in the node names.
We can then say

```
http://www.example_hospital/sample/XXXXXX/11/6002:248956422
owl:sameAs 
http://www.ncbi.nlm.nih.gov/projects/genome/assembly/grc/FUTURE/chromosome/11/6007:248956422
```
Allowing us to link nodes in vg versions.
