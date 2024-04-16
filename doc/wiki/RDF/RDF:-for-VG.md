# Mini-introduction to RDF

[RDF](http://www.w3.org/RDF/) is a collection of standards that are used to represent information in graph forms. There are a multitude of byte wise serializations e.g. [RDF/XML](http://www.w3.org/TR/2014/REC-rdf-syntax-grammar-20140225/), [JSON-LD](http://json-ld.org/), [Turtle](http://www.w3.org/TR/turtle/), [HDT](http://www.rdfhdt.org/), as well as a standardized graph query language called [SPARQL](http://www.w3.org/TR/sparql11-query/).

RDF talks about resources (assume a webpage) and literals (numbers, strings, booleans, datetimes etc..).
It links resources together in 3's using a subject, predicate, object syntax called a triple.
In a sound datamodel triples read like simple sentences. These sentences/triples in aggregate are used to describe your information in an explicit (if tedious) manner.

For example take the following sentence.
```
Jerven loves his wife.
```
This can be translated into RDF in the turtle syntax.

```turtle
<https://www.linkedin.com/in/jervenbolleman> <http://dictionary.reference.com/browse/love> _:1 .
```
Like good English one finishes a triple with a dot.
Resources can be referenced using their full paths or using an abbreviation called a prefix. Which like in a good paper you define before first use.
```turtle
prefix linkedin:<https://www.linkedin.com/in/>
prefix dictionary:<http://dictionary.reference.com/browse/love> 

linkedin:jervenbolleman dictionary:love _:1 .
```

What is that ```_:1``` thingy:
```_:1``` is what we in the semanticweb world call a blank node. Think of them as temporary identifiers, that can be resolved into permanent ones the moment that you have more information (my wife is internet shy ;) )
They are horribly annoying, but sometimes super useful. They show up a lot in examples and complaints about semtech being difficult, but are relatively rare in large databases (but it keeps the philosophical computer science academics busy enough...)

The triples end up describing a DAG ([Directed Acyclic Graph](https://en.wikipedia.org/wiki/Directed_acyclic_graph)).
```turtle
linkedin:jervenbolleman dictionary:love _:1 .
_:1 dictionary:love <http://en.christinesrecipes.com/2011/06/pandan-chiffon-cake.html> .
```
This is a graph that has 3 nodes, my wife, me and a type of cake. And one predicate/relation linking those nodes. In RDF predicate/relations are nodes just like a start (subject) or end (object) of a vertex, that can have their own further information added.

So in a classical graph drawing it looks like this

![Linear digraph showing love relationships ;)](https://chart.googleapis.com/chart?chl=digraph+example+%7B%0D%0A+++++jerven%3A%5Btype%3Dp%5D%0D%0A+++++wife%3A%5Btype%3Dp%5D%0D%0A+++++jerven-%3Ewife%5Blabel%3Dlove%5D%0D%0A++++wife-%3Epandan%5Blabel%3Dlove%5D%0D%0A++++pandan%3A%5Btype%3Dc%5D%0D%0A+%7D%0D%0A++++++++&cht=gv)

# VG as RDF

VG seems to have two major parts, nodes and paths. Nodes make the total Variant Graph, while paths connecting Nodes are required to regenerate linear genomes that we know and love from the 90's ;)

The examples below depend on http://github.com/ekg/vg/issues/109 for resolution.

## Nodes

The same concepts can also be used for VG nodes.
```
node:1 rdf:type vg:Node . 
node:1 rdf:value "g" .

node:2 rdf:type vg:Node ; # a semi-colon means repeat a subject on the next line.
        rdf:value "a" .
```
here all we are saying is that we have a node number 1, that has a nucleotide "g" associated with it and a second node number 2 with a nucleotide "a".

## Paths

a path consists out of number of steps linking nodes together in a fixed observed order (where each node has a rank).

```turtle
path:GHCR38.p2/Chr11 rdf:type vg:Path ;
      rdfs:comment "This path represents the linear reference sequence GRCh38.p2 of Human chromosome 11" .    

step:44067802 rdf:type vg:Step ;
       vg:node node:1 ;
       vg:path path:GHCR38.p2/Chr11 ;
       vg:rank 1 .

step:44067803 rdf:type vg:Step ;
       vg:node node:2 ;
       vg:path path:GHCR38.p2/Chr11 ;
       vg:rank 2 .
```

Each step has a direction e.g. forward to reverse, forward to forward, reverse to forward, reverse to reverse.

etc...

![As an image this looks like ](https://chart.googleapis.com/chart?chl=digraph+example%7B%0D%0A%0D%0A++++path%5Btype%3Dpath%2Cshape%3Dbox%2Clabel%3D%22path%3AGHCR38.p2%2FChr11%22%5D%0D%0A++++node1%5Btype%3D%22node%22%5D%0D%0A++++node2%5Btype%3D%22node%22%5D%0D%0A++++rank1%5Bshape%3Doctagon%2Clabel%3D%22step%3A44067802%22%5D%0D%0A++++rank2%5Bshape%3Doctagon%2Clabel%3D%22step%3A44067803%22%5D%0D%0A++++seq1%5Bshape%3Dplaintext%2Clabel%3D%22%27g%27%22%5D%0D%0A++++seq2%5Bshape%3Dplaintext%2Clabel%3D%22%27a%27%22%5D%0D%0A++++order1%5Bshape%3Dplaintext%2Clabel%3D%221%22%2Cconstraint%3Dfalse%5D%0D%0A++++order2%5Bshape%3Dplaintext%2Clabel%3D%222%22%2Cconstraint%3Dfalse%5D%0D%0A++++path-%3Erank1%5Blabel%3D%22vg%3Apath%22%2Cdir%3Dback%5D%0D%0A++++path-%3Erank2%5Blabel%3D%22vg%3Apath%22%2Cdir%3Dback%5D%0D%0A++++rank1-%3Enode1%5Blabel%3D%22vg%3Anode%22%5D%0D%0A++++rank2-%3Enode2%5Blabel%3D%22vg%3Anode%22%5D%0D%0A++++rank1-%3Eorder1%5Blabel%3D%22vg%3Arank%22%5D%0D%0A++++rank2-%3Eorder2%5Blabel%3D%22vg%3Arank%22%5D%0D%0A++++node1-%3Eseq1%5Blabel%3D%22rdf%3Avalue%22%5D%0D%0A++++node2-%3Eseq2%5Blabel%3D%22rdf%3Avalue%22%5D%0D%0A++++node1-%3Enode2%5Blabel%3D%22vg%3AlinksForwardToForward%22%5D%0D%0A%7B+rank%3Dsame%3B++rank1+rank2+order1+order2+%7D%0D%0A%7B+rank%3Dsame%3B++node1+node2+%7D%0D%0A+%7D&cht=gv)

## Querying variation graphs

One of the many questions that will be asked is reconstructing a linear sequence.
The following sparql example query will rebuild a "sequence string" for each path by grouping the 
steps(rank) with a sequence fragment into an ordered concatenation giving one long "sequence string" as output.

```sparql
PREFIX vg:<http://example.org/o/> 
PREFIX rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#> 
SELECT 
  ?path (group_concat(?sequence; separator='') as ?pathSeq)
WHERE {?step vg:path ?path; 
             vg:node ?node ;
             vg:rank ?rank.
       ?node rdf:value ?sequence} 
GROUP BY ?path 
ORDER BY ?rank
```

Using the example data from above this will generate the following output (as it is a select query you get back tab delimited data or in XML or JSON format as you want)

```
path                   pathSeq
path:GHCR38.p2/Chr11   "ga"
```

Finding a nodes near a specific position in a path is also possible.
For ease of reading the query (and probably performance) we add an integer denoting the start of the step along the linear sequence.

The following example asks for any node that is near the positions 44067802 to 44067820 of the path:GHCR38.p2/Chr11. This uses only standard SPARQL functionality.
```sparql
PREFIX vg:<http://ekg.github.io/whatever/TO BE DETERMINED> 
PREFIX rdf:<ttp://www.w3.org/1999/02/22-rdf-syntax-ns#> 
SELECT 
  ?nearNode ?sequence
WHERE {
      ?step vg:path path:GHCR38.p2/Chr11; 
             vg:node ?node ;
             vg:position ?position ; #The offset from the start of the path. Not serialized yet in vg view 
             vg:rank ?rank.
       ?node rdf:value ?sequence .
       FILTER(?position > 44067802 && ?position < 44067820)
       ?node vg:linksForwardToForward|^vg:linksForwardToForward ?nearNode . #ask for before or the inverse of before aka after
 }
```
There are different ways of asking this query and some of the sparql extensions for graph algorithms can really help here. 

## Dealing with steps on reverse complement of node sequence

Steps have a rank and a node associated with them.

```
step:1 a vg:Step ;
       vg:rank 1 ;
       vg:node node:1 .
```
However, the node only has a forward sequence, if we want to use a node as a step but using the reverse strand then we need to change the predicate we use. e.g. vg:reverseOfNode

```
step:2 a vg:Step ;
       vg:rank 2 ;
       vg:reverseOfNode node:2 .
```
Which makes the rebuild fasta query into this
```
PREFIX vg:<http://example.org/o/> 
PREFIX rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#> 
SELECT 
  ?path (group_concat(?sequence; separator='') as ?pathSeq)
WHERE 
{
   ?step vg:path ?path . 
      {
          ?step vg:node ?node ;
          ?node rdf:value ?sequence
      } UNION {
          ?step vg:reverseOfNode ?node ;
          ?node rdf:value ?forwardSequence .
          #We need to reverse complement, we can do this inline in standard sparql 
          # or with an extension function
          #BIND(replace(
          #       replace(
          #         replace(?forwardSequence, "A", Y"),"T","A"), "Y", "T") as ?atcomplemented)
          #BIND(replace(
          #       replace(
          #          replace(?atcomplemented, "G", Y"),"C","G"), "Y", "C") as ?sequence)
          BIND(bio:reverseComplement(?forwardSequence) as ?sequence)
       }
    vg:rank ?rank.
} 
GROUP BY ?path 
ORDER BY ?rank
```
# Prefixes and identifiers of nodes

The examples above use ```rank:1``` and ```vg:rank``` etc... like in the earlier turtle examples these are prefixes. However, they are undefined, which is not allowed (per standard, for convenience many sparql databases allow you to set default well known prefixes). This is because the real resolution of these depends on https://github.com/ekg/vg/issues/112.

As you might have noticed all node identifiers consist out of well formed URLs. In the pure sense that means for each node in a VG there should be a web page. This is not a hard requirement, but is very useful.

These URLs ensure that one can get more context about nodes and also avoids most common identifier clashes. Enabling people to merge and link graphs on demand into one data store or file.

# Integrating other datasources/annotating nodes

This shows a small variant graph showing an extract from http://www.ncbi.nlm.nih.gov/SNP/snp_ref.cgi?type=rs&rs=33952257 and http://www.uniprot.org/uniprot/Q96QU6#VAR_048227.
 ![VG showing a variant from dbsnp which affects a gene product as annotated in UniProt](https://chart.googleapis.com/chart?chl=digraph+example%7B%0D%0Asubgraph+vg+%7B%0D%0Apath%5Btype%3Dpath%2Cshape%3Dbox%2Clabel%3D%22path%3AGHCR38.p2%2FChr11%22%5D%0D%0Anode1%5Btype%3D%22node%22%2Clabel%3D%22GHCR38.p2%2FChr11%3A44067802%22%5D%0D%0Anode2%5Btype%3D%22node%22%2Clabel%3D%22GHCR38.p2%2FChr11%3A44067803%22%5D+%0D%0Anode3%5Btype%3D%22node%22%2Clabel%3D%22dbsnp%3Ars33952257%22%5D%0D%0Arank1%5Bshape%3Doctagon%2Clabel%3D%22step%3A44067802%22%5D%0D%0Arank2%5Bshape%3Doctagon%2Clabel%3D%22step%3A44067803%22%5D%0D%0Aseq1%5Bshape%3Dplaintext%2Clabel%3D%22%27g%27%22%5D%0D%0Aseq2%5Bshape%3Dplaintext%2Clabel%3D%22%27a%27%22%5D%0D%0Aseq3%5Bshape%3Dplaintext%2Clabel%3D%22%27g%27%22%5D%0D%0Aorder1%5Bshape%3Dplaintext%2Clabel%3D%221%22%2Cconstraint%3Dfalse%5D%0D%0Aorder2%5Bshape%3Dplaintext%2Clabel%3D%222%22%2Cconstraint%3Dfalse%5D%0D%0Apath-%3Erank1%5Blabel%3D%22vg%3Apath%22%2Cdir%3Dback%5D%0D%0Apath-%3Erank2%5Blabel%3D%22vg%3Apath%22%2Cdir%3Dback%5D%0D%0Arank1-%3Enode1%5Blabel%3D%22vg%3Anode%22%5D%0D%0Arank2-%3Enode2%5Blabel%3D%22vg%3Anode%22%5D%0D%0Arank1-%3Eorder1%5Blabel%3D%22vg%3Arank%22%5D%0D%0Arank2-%3Eorder2%5Blabel%3D%22vg%3Arank%22%5D%0D%0Anode1-%3Eseq1%5Blabel%3D%22rdf%3Avalue%22%5D%0D%0Anode2-%3Eseq2%5Blabel%3D%22rdf%3Avalue%22%5D%0D%0Anode3-%3Eseq3%5Blabel%3D%22rdf%3Avalue%22%5D%0D%0Anode1-%3Enode2%5Blabel%3D%22vg%3AlinksForwardToForward%22%5D%0D%0Anode3-%3Enode2%5Blabel%3D%22vg%3AlinksForwardToForward%22%5D%0D%0Anode3-%3Enode1%5Blabel%3D%22variant+of%22%2Cstyle%3D%22dotted%22%2Ccolor%3D%22blue%22%5D%0D%0A%7D%0D%0Asubgraph+uniprot+%7B%0D%0Aannotation%5Blabel%3D%22annotation%3AVAR_048227%22%5D%0D%0Aannotation-%3ENatural_Variant_Annotation%5Blabel%3D%22rdf%3Atype%22%5D%0D%0ANatural_Variant_Annotation%5Blabel%3D%22Natural_Variant_Annotation%22%5D%0D%0Aannotation-%3Enode3%5Blabel%3D%22rdfs%3AseeAlso%22%5D%0D%0Aannotation-%3EN%5Blabel%3D%22up%3Asubstitution%22%5D%0D%0AN%5Bshape%3D%22plaintext%22%5D%0D%0A%7D%0D%0A%7D&cht=gv)


Showing that in an RDF representation one can combine data from very disparate sources, in this case genomic and proteomic information that has been mapped via dbSNP identifiers.

# Storage in existing RDF/SPARQL/GRAPH databases

An advantage of RDF is that it has a number of interesting serialisations but one information domain.
Databases that might be very interesting for VG graphs are for example:
* [Virtuoso](github.com/openlink/virtuoso-opensource) a columnar relational+rdf database (GPL+Commercial)
* [Oracle 12C Semnet](https://docs.oracle.com/database/121/RDFRM/toc.htm) all of Oracle including data guard, inmemory and labeled security, suitable for clinical settings.
* [BlazeGraph](https://www.blazegraph.com/product/gpu-accelerated/) GPU accelerated, clustered graph database (GPL+Commercial)
* [urika](http://www.yarcdata.com/) super computer based tech for extreme in memory graph sizes.

There are also concepts like [linked data fragments](http://linkeddatafragments.org/) a pure REST solution. Allowing client side access to complicated data (especially interesting to merge on demand some of the small data).

There is also the [R2RML standard](http://www.w3.org/TR/r2rml/) that has multiple implementations and allows one pretend a relational database is a SPARQL database.

# Named graphs

While we talk a lot about triples, the RDF world also has quads. These are triples with a fourth URL/Resource associated, which group triples into sets. This is useful for provenance, data management, security, query selectively etc...

For example one can put all nodes in the reference assembly in a named graph that reflects that fact, then put the nodes introduced by 1000Genomes UK into another named graph etc... This makes it easy to query your combined VG in different subsets from source.


# Custom DNA/Bioinformatics functions
SPARQL comes with a small set of functions, mostly from XPATH and XML world. But all production stores allow one to add custom domain specific functions for agregates and in filters.

e.g. in this example we query for the [reverse complement](http://www.genscript.com/sms2/rev_comp.html) of the DNA string that is actually stored in the database
```sparql
SELECT ?reversecomplement
WHERE
   ?node rdf:value ?dnasequence .
   BIND(bio:reverseComplement(?dnasequence) AS ?reversecomplement)
```
This means that all these existing databases can be extended for most common bioinformatics query needs.


# What about file size??

There are extremely verbose RDF formats. For example the n-quads format is just fat. With genomics we are already having issues with file storage so n-quads is not a good format there.
However there are serialisations such as [RDF/Thrift](http://afs.github.io/rdf-thrift/) and HDT that are more condensed. 
There is also the possibility of running SPARQL queries against existing datasources. For example the proof of concept code at [https://github.com/JervenBolleman/sparql-vcf] shows that one can query VCF (Variant Call File) with SPARQL using translation on demand.
Such approaches could also be build into VG view.
Which means that the RDF solution does not need to take more space than a custom solution. While I admit an off the shelf RDF solution probably will be more verbose than a tightly packed binary data-structure.

In the first experiments the gzipped RDF/Turtle files are 20% larger than the VG files. This can be improved upon by reducing unneeded white-space as well as grouping related concepts closer to each other.

# Experiment loading VG into Virtuoso 7.2.1

Virtuoso is a columnar relational database that also supports RDF and SPARQL. It happens to be the one used for sparql.uniprot.org. 

We use the data as described in [the wiki page working-with-a-whole-genome-variation-graph](/ekg/vg/wiki/working-with-a-whole-genome-variation-graph)

The first challenge is to generate a turtle file per Chromosome. For the VG parts you need a 200GB+ ram machine if you run it all in memory but this should be less with the -C (Chunked option). 

```bash
for i in $(seq 22;echo X;echo Y)
do
  vg view -r "http://example.org/vg/chr/$i" -VCt $i.vg | pigz > $i.ttl.gz
done
```
The -r sets the rdf base uri. This namespaces all identifiers so that they do not clash once loaded into a triplestore. This is set to example.org in the examples but should be set to something like http://purl.sanger.ac.uk/durbin/variant_graphs/2015/12/experiment1 or equivalent in practice.

Then you take a [virtuoso build](https://github.com/openlink/virtuoso-opensource) and configure it for loading the data. The config file below is adapted from the uniprot sparql endpoint one, change paths as needed.

```bash
nohup ./virtuoso-t -f +wait +configfile ./virtuoso-config-for-test.ini >> vg_rdf_exp_load.log
```

```bash
for f in $(ls -1 *.ttl.gz)
do
    echo $f
    #tell virtuoso we want to load a turtle file for a chromosome
    $isql $port $user $password exec="ld_dir ('$path', '$f', 'http://example.org/vg');"
done
```

```bash
#turn off text indexing as that is use less for this data
$isql $port $user $password exec="DB.DBA.RDF_OBJ_FT_RULE_DEL (null, null, 'All');"
```

Then we want to load all the data, this is faster if auto checkpoints are turned of so we do.
We auto detect the number of CPUs and we launch a job for half of them. If you have less files than halve the number of CPUs, that will be the limiting factor in loading your data, other wise it is the largest file being the limiting factor.
```bash
$isql $port $user $password exec="checkpoint_interval (0)";
for i in $(seq 1 2 $(cat /proc/cpuinfo | grep processor | tail -n 1 | grep -oP "\d+"));
do
        #echo $i;
        $isql $port $user $password  exec="rdf_loader_run();" &
done
wait;
date
$isql $port $user $password exec="checkpoint;"
$isql $port $user $password exec="checkpoint_interval (60)";
```

For our machine, a 256 GB ram 3TB consumer SSD 64 core AMD cpu's from 2013 loads the data in 67 minutes.
```
11:25:19 PL LOG: Loader started
...
...
12:17:39 PL LOG: No more files to load. Loader has finished,
12:31:12 Checkpoint started
12:32:16 Checkpoint finished, log reused
```
The data on disk is just 51GB
```bash
du -hs  triples/*
51G     triples/virtuoso.db
4.0K    triples/virtuoso.lck
16K     triples/virtuoso.log
0       triples/virtuoso.pxa
2.0M    triples/virtuoso-temp.db
2.0G    triples/virtuoso.trx
```

```ini
;
;  virtuoso.ini
;

;  Database setup
;
[Database]
DatabaseFile                = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/triples/virtuoso.db
ErrorLogFile                = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/triples/virtuoso.log
LockFile                    = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/triples/virtuoso.lck
TransactionFile             = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/triples/virtuoso.trx
xa_persistent_file          = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/triples/virtuoso.pxa
ErrorLogLevel               = 7
FileExtend                  = 20000
MaxCheckpointRemap          = 20000
Striping                    = 0
TempStorage                 = TempDatabase



[TempDatabase]
DatabaseFile                = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/triples/virtuoso-temp.db
TransactionFile             = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/triples/virtuoso-temp.trx
MaxCheckpointRemap          = 2000
Striping                    = 0


;
;  Server parameters
;
[Parameters]
ServerPort                  = 1113
LiteMode                    = 1
DisableUnixSocket           = 0
DisableTcpSocket            = 0
ServerThreads               = 64
AsyncQueueMaxThreads        = 10
MaxClientConnections        = 64
CheckpointInterval          = 600
O_DIRECT                    = 0
UseAIO                      = 1
CaseMode                    = 2
MaxStaticCursorRows         = 5000
CheckpointAuditTrail        = 0
AllowOSCalls                = 0
SchedulerInterval           = 10
DirsAllowed                 = /scratch/uuw_sparql/jerven_rdf_vg_experiment_delete_in_2016/
ThreadCleanupInterval       = 0
ThreadThreshold             = 10
ResourcesCleanupInterval    = 0
FreeTextBatchSize           = 100000
SingleCPU                   = 0
VADInstallDir               = /dev/null
PrefixResultNames           = 0
dfFreeTextRulesSize        = 100
IndexTreeMaps               = 256
MaxMemPoolSize              = 134217728
TransactionAfterImageLimit  = 1000000000
ColumnStore                 = 1
QueryLog                    = /tmp/virtuoso-vg-query.log
NumberOfBuffers             = 14695720
MaxDirtyBuffers             = 11021790
ThreadsPerQuery             = 64
MaxQueryMem                 = 10G ; see UPS-131
VectorSize                  = 10000
MaxVectorSize               = 1000000
AdjustVectorSize            = 0 ; see UPS-131
CheckpointSyncMode          = 2

[AutoRepair]
BadParentLinks              = 0

[HTTPServer]
ClientIdString = Mozilla/4.0 (compatible; Virtuoso, running-at: sparql.uniprot.org)

[Client]
SQL_PREFETCH_ROWS           = 100
SQL_PREFETCH_BYTES          = 16000
SQL_QUERY_TIMEOUT           = 0
SQL_TXN_TIMEOUT             = 0

[VDB]
ArrayOptimization           = 0
NumArrayParameters          = 10
VDBDisconnectTimeout        = 1000
KeepConnectionOnFixedThread = 0

[SPARQL]
;ExternalQuerySource        = 1
;ExternalXsltSource         = 1
DefaultGraph                = http://example.org/vg/
ResultSetMaxRows            = 20000000000 ; 20 billion for now.
MaxQueryCostEstimationTime  = 86400  ; in seconds
MaxQueryExecutionTime       = 86400  ; in seconds
DefaultQuery                = select distinct ?Concept where {[] a ?Concept} LIMIT 100
DeferInferenceRulesInit     = 0  ; controls inference rules loading
```






