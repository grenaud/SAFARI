## Intro

`vg` allows you to visualize graphs by generating a graphviz-formatted output stream. Here, I'll show you some techniques that are available for the visualization of various elements in the `vg` universe.

## Let's all visualize!

Basic visualization in `vg` can be accomplished by processing the graphviz output from `vg view`. For example, starting in the `test/` directory, we can use the following to visualize the graph in chromium:

```shell
vg construct -v tiny/tiny.vcf.gz -r tiny/tiny.fa \
    | vg view -d - \
    | dot -Tsvg -o x.svg
chromium-browser x.svg
```

![screenshot from 2015-10-28 12 49 54](https://cloud.githubusercontent.com/assets/145425/10789042/0534eac2-7d73-11e5-92bd-310d4f1d9f24.png)

You will need to install graphviz tools (such as via `sudo apt-get install graphviz` on linux).

## Visualizing bidirectional sequence graphs

Variation graphs in `vg` are "train track graphs"--- 

<img src="https://cloud.githubusercontent.com/assets/145425/10789288/5eb6071a-7d74-11e5-8783-0015f69a4391.png" width=50%>

They implicitly include their reverse complement, and also edges can "reverse" and go from the forward to reverse strand, akin to the way that two rails on a train track work.

<!-- ![image](https://cloud.githubusercontent.com/assets/145425/10789301/7bcf2ed0-7d74-11e5-9e40-54bbf1f669f8.png) -->

This lets us represent inversions without duplicating the inverted sequence, which achieves one of the goals of using variation graphs: that annotations and information about variation can be represented with minimal duplication. Similarly, it means we can avoid the multiple mapping problem that might result if there were two disparate positions in the graph that encode a particular sequence. Finally, this is a standard way of modeling graphs and exactly matches the model encoded in [GFA](https://github.com/pmelsted/GFA-spec), so this ensures we can use graphs from any source that produces GFA.

If we refer to the two sides of our nodes as "start" and "end", if we go from the start to the end in the forward direction and from the end to the start in the reverse, and if we allow edges to connect either of the two ways then we get four types of edges.

We record the edge type in protobuf/json by indicating which ends are in the non-standard orientation using the `from_start` and `to_end` flags.

The default goes "from the end to the start", or `"from_start": false, "to_end": false` in our serialization format:

<img src="https://cloud.githubusercontent.com/assets/145425/10790623/45d073e0-7d7c-11e5-8782-adfdf4e29738.png" width=25%>

Where if we want to express an transition from the forward strand of one node to the reverse of the next, we'd say `"from_start": false, "to_end": true`:

<img src="https://cloud.githubusercontent.com/assets/145425/10790624/45d0be4a-7d7c-11e5-81cc-44e875b809c2.png" width=25%>

And a transition from the reverse strand of one node to the forward of the next, would be `"from_start": true, "to_end": false`:

<img src="https://cloud.githubusercontent.com/assets/145425/10790622/45cef92a-7d7c-11e5-9d3c-6a900e8a7c3b.png" width=25%>

We render the reverse strand version of the default (`"from_start": true, "to_end": true`) the same way as the forward strand, as it doesn't provide any additional information. It's just an alternative way of saying the same thing.

<img src="https://cloud.githubusercontent.com/assets/145425/10790625/45d4a32a-7d7c-11e5-9d7b-9b9c17cdffe3.png" width=25%>

These edge types can all be represented in vg format, in GFA, and also in the graphviz output which was used to render these images. In the graphviz output, the different types of edges are modeled using [graphviz ports](http://graphviz.org/category/tags/port), which let us attach an edge to a particular corner of a node.

Note that in practice we don't usually need to render the node arrows, although this can sometimes help with ambiguous visualizations as in the preceding examples. You can add them back in by piping the graphviz output from vg through `sed s/arrowhead=none/arrowhead=normal/g`.

## Some complex examples

There are a few test cases which we've used during the extension of `vg` to handle cyclic graphs and bidirectional edges. They exhibit a mixture of cases that were initially problematic, and here can provide an example of what's possible to express and visualize with `vg`:

<img src="https://cloud.githubusercontent.com/assets/145425/10791789/d71e899a-7d81-11e5-850b-72e55beedc94.png" width=50%>

![screenshot from 2015-10-28 14 39 52](https://cloud.githubusercontent.com/assets/145425/10791790/d71f143c-7d81-11e5-8438-f6c3f88c6903.png)

<img src="https://cloud.githubusercontent.com/assets/145425/10791788/d71af096-7d81-11e5-97b4-fea95093fa32.png" width=50%>


## Extending `vg view -d` to visualize paths

The graph isn't just nodes and edges. We also have paths, which are a critical component of the variation graph reference architecture. We can render them in a few ways by combining the `-d` flag with `-p` (show paths as external subgraphs), `-n` (label edges), and `-w` (walk edges, adding a new edge to the graph for each path between two nodes).

Here they are in action on a trivial test graph produced above (`vg construct -v tiny/tiny.vcf.gz -r tiny/tiny.fa`):

`vg view -dp`

![image](https://cloud.githubusercontent.com/assets/145425/10790880/a7d9458e-7d7d-11e5-90d6-a73adb84987c.png)

`vg view -dn`

![screenshot from 2015-10-28 14 11 25](https://cloud.githubusercontent.com/assets/145425/10790902/ccde91b8-7d7d-11e5-9499-b046e5f1f109.png)

`vg view -dw`

![screenshot from 2015-10-28 14 12 07](https://cloud.githubusercontent.com/assets/145425/10790922/e3dd3838-7d7d-11e5-9ca1-d28be55ca8dd.png)

They can be combined. I find `vg view -dpn` to be very useful:

![screenshot from 2015-10-28 14 12 51](https://cloud.githubusercontent.com/assets/145425/10790936/fd778492-7d7d-11e5-9445-23a6f3a5df4c.png)

## Path labeling in visualization

Right now you may be wondering where the grey saxophone is coming from. For visualizing many graphs `vg` uses 766 unicode pictographs (emoji) and the 8 colors of the Brewer dark28 palette to generate 6128 possible color/symbol combinations that allow easy differentiation of the paths based on tiny symbols. The color/symbol combination is chosen on the basis of a hash of the path name, so as long as the path names are unique we should typically expect unique symbolic identifiers within graphs with reasonable numbers of paths.

This doesn't really help for single paths, but as the number of paths increases it can really help debugging. For instance, this is a fragment of the MHC which has 9 haplotypes:

![image](https://cloud.githubusercontent.com/assets/145425/10791417/49ad1668-7d80-11e5-83fe-7c67eea529f9.png)

(You can render this with `vg msga -f GRCh38_alts/FASTA/HLA/K-3138.fa -B 256 -k 22 -K 11 -X 1 -E 4 -Q 22 -D | vg view -dpn - | dot -Tsvg -o K-3138.svg`.)

## Viewing alignments

Given the importance of alignments in sequence analysis, it should be easy to view them. Existing tools that work on linear references really won't cut it in the graph world. A solution is to treat the alignments like paths and add some visual indicators that help us interpret the alignment orientation and mismatches between the alignment and the graph.

If you have your alignments in GAM (graph alignment / map) format, you can visualize them against a graph using another extension to `vg view -d`:

```
vg construct -v tiny.vcf.gz -r tiny.fa >t.vg
vg index -x t.xg -g t.gcsa -k 11 t.vg         
vg sim -l 20 -n 10 -e 0.05 -i 0.02 -x t.xg t.vg  >t.reads
vg map -T t.reads -x t.xg -g t.gcsa >t.gam
vg view -d t.vg -A t.gam | dot -Tsvg -o aln.svg
```

The result shows blue segments for exact matches, yellow for mismatches, and green and purple ends to the alignments to indicate their relative orientation.

![image](https://cloud.githubusercontent.com/assets/145425/10791631/21715398-7d81-11e5-83ea-32d0a3fbde4c.png)

## Large graphs

When working with a larger graph (for example, a whole-genome graph), it can become impractical to try and render the entire graph at once. Even if you were willing to wait around for vg to load the whole graph into memory and convert it to dot format, Graphviz will unceremoniously segfault when asked to lay out graphs beyond a certain size, and there are not many good tools for working with enormous SVG images.

### Visualizing subgraphs

One approach to dealing with large graphs is to subset them down to just the part you are interested in, and to draw that. If you are interested in multiple subregions of the same graph, the best way to do this is to make an xg index of the graph with `vg index` and to pull out and visualize subgraphs around interesting nodes or path regions with `vg find`. You can do that like this:

```
# Build an xg index of the graph
vg index -x graph.xg graph.vg

# Extract a subgraph centered on node 1234 (-n 1234), looking out
# a context of 3 nodes (-c 3) and visualize it with paths.
vg find -x graph.xg -n 1234 -c 3 | vg view -dp - | dot -Tsvg -o subgraph.svg

# View a subgraph around multiple nodes of interest.
# Note that if the contexts extracted for the source nodes don't
# touch you will get a graph with multiple disconnected pieces.
# Paths may end up jumping from piece to piece in a misleading way.
vg find -x graph.xg -n 1234 -n 5678 -c 3 | vg view -dp - | dot -Tsvg -o subgraph2.svg

# View a subgraph centered on part of a path ("ref", from position 5000 to position 5100)
vg find -x graph.xg -p ref:5000-5100 -c 3 | vg view -dp - | dot -Tsvg -o subgraph3.svg
```

If you have a vg file and you are only wanting to pull out a single subgraph, it can be easier to use `vg mod` to directly subset your graph, without making an xg index:

```
# Extract a subgraph centered on node 1234 (-g 1234), looking out
# a context of 3 nodes (-x 3) and visualize it with paths.
vg mod -g 1234 -x 3 graph.vg | vg view -dp - | dot -Tsvg -o subgraph.svg
```


### Visualizing large subgraphs

If you find that you *do* want to draw more graph at a time than `dot` can handle, you can use `neato`, also from graphviz, to draw the graph. In addition to `neato` you can greatly reduce layout time by running the [mars graph layout algorithm](https://github.com/marckhoury/mars) prior to feeding the graphviz format graph into neato.

### Using `vg viz`

If you have a large graph in xg format, and you really *do* want to lay it all out in one image, you can use `vg viz` to draw it. `vg viz` uses a simple linear layout of all the nodes, which it can precalculate before rendering anything, and so it can be much faster. You can run it like this:

```
vg viz -x graph.xg --out graph.svg
```