# IMPORTANT NOTE - DOCUMENT DEPRECATED
The concept in this document was an exploration during the [Japan NBDC/DBCLS BioHackathon 2019](http://2019.biohackathon.org/). Graph summarization should have helped to generate zoom levels for visualization. The idea was not further pursued but a concept to generate zooming levels of pangenome graphs is currently under implementation at https://github.com/graph-genome/component_segmentation. For an overview of the whole `Pantograph` project, please go to https://graph-genome.github.io/.

# Introduction
Graph summarization introduces a new type of nodes and relations to existing vg data.

A first proposal is to introduce three new types
1. vg:SummationNode
2. vg:SummationStep
3. vg:SummationPath

# vg:SummationNode

This is node with only topology i.e. how it connects to other `vg:SummationNode`s 
the second new piece is pointers to the nodes that it summarizes. A new predicate `vg:summaryOf` 
could be minted for this.

# vg:SummationStep

Like normal VG RDF steps are the key link between paths and nodes. Here they
are only different in the type otherwise the structure is the same.
To give a quick way to find the Steps by an offset from the start of the path we can reuse
`vg:position`

# vg:SummationPath 

These should have names and properties that link to existing paths. reusing  `vg:summaryOf` is an option.


```turtle
@prefix vg:<http://biohackathon.org/resource/vg#> . 
@prefix node: <http://example.org/vg/node/> . 
@prefix path: <http://example.org/vg/path/> . 
@prefix step: <http://example.org/vg/step/> . 
@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> . 
summationnodelevel1:1 a vg:SummationNode ;
  vg:length 64 ;
  vg:summaryOf node:1 , node:2 ;
  vg:linksForwardToForward summationnodelevel1:2 .

summationnodelevel1:2 a vg:SummationNode ;
  vg:length 96 ;
  vg:summaryOf node:3 , node:4 , node:5.

summationstep1:1 a vg:SummationStep ;
  vg:rank 1 ;
  vg:position 0 ;
  vg:path summationpath:x_at_zoom_2 ;
  vg:summaryOf path:x .

summationstep1:2 a vg:SummationStep ;
  vg:position 64 ;
  vg:rank 2 ;
  vg:path summationpath:x_at_zoom_2 ;  
  vg:summaryOf path:x .

node:1 rdf:value "CAAATAAGGCTTGGAAATTTTCTGGAGTTCTA" . 
node:2 rdf:value "TTATATTCCAACTCTCTGGTTCCTGGTGCTAT" . 
node:3 rdf:value "GTGTAACTAGTAATGGTAATGGATATGTTGGG" . 
node:4 rdf:value "CTTTTTTCTTTGATTTATTTGAAGTGACGTTT" . 
node:5 rdf:value "GACAATCTATCACTAGGGGTAATGTGGGGAAA" . 
step:x-1 vg:position 0 ; 
 a vg:Step ;
 vg:rank 1 ; 
 vg:node node:1 ; 
 vg:path path:x . 
step:x-2 vg:position 32 ; 
 a vg:Step ;
 vg:rank 2 ; 
 vg:node node:2 ; 
 vg:path path:x . 
step:x-3 vg:position 64 ; 
 a vg:Step ;
 vg:rank 3 ; 
 vg:node node:3 ; 
 vg:path path:x . 
step:x-4 vg:position 96 ; 
 a vg:Step ;
 vg:rank 4 ; 
 vg:node node:4 ; 
 vg:path path:x . 
step:x-5 vg:position 128 ; 
 a vg:Step ;
 vg:rank 5 ; 
 vg:node node:5 ; 
 vg:path path:x . 
node:1 vg:linksForwardToForward node:2 . 
node:2 vg:linksForwardToForward node:3 . 
node:3 vg:linksForwardToForward node:4 . 
node:4 vg:linksForwardToForward node:5 . 
```