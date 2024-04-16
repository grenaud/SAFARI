### Maciek's VG/Fuseki/SPARQL recipe.

## 1. Generate a VG graph, export it to a turtle file

in VG root directory, make a graph from a multi-sequence FASTA:
```
vg msga -f test/GRCh38_alts/FASTA/HLA/B-3106.fa -D > b3106.vg
```
then convert it to a Turtle file:
```
vg view -p -r http://github.com/vgteam/vg -t b3106.vg > b3106.ttl
```

There are pipe symbols in the gi names. This is a problem at the moment see vg#245.
```
sed -ie 's/\|/\%7C/' b3106.ttl 
```

## 2. Install & provision the Fuseki server.

I'm on MacOS X, using the homebrew package manager:
```
brew install fuseki
```
Start it up in update mode, so I can interactively feed it the turtle file:
```
fuseki-server --update --mem /b3106
```
Feed the turtle to Fuseki:
```
s-put -v http://localhost:3030/b3106/data default b3106.ttl
```

## 3. Make SPARQLy queries against it, using the local Fuseki web interface:

```
http://localhost:3030/control-panel.tpl
``` 
then select the b3106 dataset, to get to this page:
```
http://localhost:3030/sparql.tpl
```

### First query: Return all the bases for a given path
```
PREFIX vg:<http://example.org/vg/>
PREFIX rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#>
SELECT ?path (group_concat(?sequence; separator="") as ?pathSeq)
WHERE {
    BIND(<http://example.org/vg/path/gi%7C568815592:31353871-31357211> as ?path)
    ?step vg:path ?path;
    vg:node ?node;
    vg:rank ?rank.
    ?node rdf:value ?sequence
}
GROUP BY ?path
ORDER BY ?rank
```
### Second query: Return all paths passing through a given node together with the rank this node has in each path
```
PREFIX vg:<http://example.org/vg/>
PREFIX rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#>
SELECT ?path ?rank
WHERE {
  ?step vg:node <http://example.org/vg/node/485>;
       vg:rank ?rank;
       vg:path ?path.
}
```
### Third query: Attempt to find all nodes within some given distance (10) of a given node.
```
PREFIX vg:<http://example.org/vg/>
PREFIX rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#>
PREFIX vgnode:<http://example.org/vg/node/>
SELECT ?nearbynode ?hopcount
WHERE {
     SELECT ?nearbynode (count(?hop) as ?hopcount)
     WHERE {
        vgnode:261 vg:linksForwardToForward* ?hop.
        ?hop vg:linksForwardToForward+ ?nearbynode .
    }
    GROUP BY ?nearbynode
    HAVING (?hopcount < 10)

    ORDER BY ?hopcount
}
```
Problem is, it doesn't exactly return what you may think:
It returns all nodes such that there are at most 10 nodes in any paths between the returned node and the selected node. 

### Fourth query: But I _can_ obtain a subgraph between any two given nodes (261 and 280).
I'm returning pairs of nodes connected by an edge in the graph, to get at the full graph topology.
```
PREFIX vg:<http://example.org/vg/>
PREFIX rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#>
PREFIX vgnode:<http://example.org/vg/node/>
SELECT ?subgraphnode_l ?subgraphnode_r
WHERE {
       vgnode:261 vg:linksForwardToForward* ?subgraphnode_l .
       ?subgraphnode_l vg:linksForwardToForward ?subgraphnode_r .
       ?subgraphnode_r vg:linksForwardToForward* vgnode:280 .
}
ORDER BY ?subgraphnode_l ?subgraphnode_r
```

