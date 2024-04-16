One approach to annotation genomic variant graphs, is to annotate nodes in the graph using the RDF serialization.
If you look at the RDF approach to serializing the VG one could even say that each path is an annotation. 
A thought that can be extended in practice.


assume a graph where there are three nodes on the reference genome.

```
(aac)->(a)->(ta)
```

```turtle
path:reference a gcr:chromsome . #path could be for example a gcr38 chromosome.

step:1 vg:path path:reference ;
              vg:node node:1-3 ;
              vg:rank 1 .
step:2 vg:path path:reference ;
              vg:node node:4-1 ;
              vg:rank 2 .
step:3 vg:path path:reference ;
              vg:node node:5-2 .
              vg:rank 3 .
node:1-3 rdf:value "aac" .
node:4-1 rdf:value "a" .
node:5-2 rdf:value "ta" .
```

So we kind of annotated node 1-3,4-1,and 5-2 to be part of the reference genome.

We then have 2 variants making 3 alleles.

```
      >(t)\
     /      \
(aac)->(a)->(tag)
      \     / 
       >(c)/
```
this adds the following two nodes.

```
sample_Y_node:4-1 rdf:value "t" .
sample_Z_node:4-1 rdf:value "c" .
```

and we have two more paths
```turtle
path:sample_Y a my_lab:sample_Y . #path could be a genome of an custom etc ...
path:sample_Z a my_lab:sample_Z .

step_Y:1 vg:path path:sample_Y ;
              vg:node node:1-3 ;
              vg:rank 1 .
step_Y:2 vg:path path:sample_Y ;
              vg:node sample_Y_node:4-1 ;
              vg:rank 2 .
step_Y:3 vg:path path:sample_Y ;
              vg:node node:5-2 .
              vg:rank 3 .


step_Z:1 vg:path path:sample_Z ;
              vg:node node:1-3 ;
              vg:rank 1 .
step_Z:2 vg:path path:sample_Z ;
              vg:node sample_Z_node:4-1 ;
              vg:rank 2 .
step_Z:3 vg:path path:sample_Z ;
              vg:node node:5-2 .
              vg:rank 3 .
```

We can then say something using a second schema/ontology to talk about the SNP concept.

```
SNP:XYZ a example:SNP_SITE .
        example:ancestral_allele node:4-1 ;
        example:allele sample_Y_node:4-1 , sample_Z_node:4-1 .
```

