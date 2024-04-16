## 1. See [Apache Fuseki example](Example:-Serving-out-an-RDF-version-of-a-VG-graph-with-Apache-Fuseki)

## 2. Install & provision the Blazegraph server

See https://www.blazegraph.com/download/

Download the blazegraph.jar

Start the server
```
java -Xmx2G -jar blazegraph.jar 
``` 

Feed the data using the upload file option at [http://localhost:9999/blazegraph/#update]. Set format to turtle and select rdf.

## 3. & 4. See [Apache Fuseki example](Example:-Serving-out-an-RDF-version-of-a-VG-graph-with-Apache-Fuseki)

## 5. Finding nodes within 15 hops of start node

```
prefix gas: <http://www.bigdata.com/rdf/gas#>
PREFIX vg:<http://example.org/vg/>
PREFIX rdf:<http://www.w3.org/1999/02/22-rdf-syntax-ns#>
PREFIX vgnode:<http://example.org/vg/node/>
SELECT ?nearbynode ?hopcount
WHERE {
   SERVICE gas:service {
     gas:program gas:gasClass "com.bigdata.rdf.graph.analytics.BFS" .
     gas:program gas:in vgnode:261 . # one or more times, specifies the initial frontier.
     gas:program gas:out ?nearbynode . # exactly once - will be bound to the visited vertices.
     gas:program gas:out1 ?hopcount . # exactly once - will be bound to the depth of the visited vertices.
     gas:program gas:link vg:linkForwardToForward . # only hops along the forward to forward are followed
  }
  FILTER (?hopcount < 15)
}
```