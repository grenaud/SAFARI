This wiki page describes a graph-based alignment concept that we sometimes work with in `vg` and describes how to produce and work with this alignment concept using the `vg` software toolkit.

## The multipath alignment concept

Most of the sequence-to-graph alignment field has focused on aligning a sequence to a path through a graph. For instance, this is the alignment concept behind the GAM file format that is produced by `vg map`. In some applications, however, it can useful to work with a more general alignment concept, which we call a multipath alignment. The same way a genome reference graph is a compressed representation of a collection of genomes, a multipath alignment is a compressed representation of a collection of alignments to these genomes. We allow the multipath alignment to bifurcate and rejoin so that it can align the same part of a sequence to multiple paths through the graph. Another way to think about this is that the multipath alignment is a graph of partial alignments. If you concatenate the partial alignments along a path in the multipath alignment's graph, it forms a sequence-to-path alignment like those contained in a GAM record.

### A diagrammatic illustration

This diagram demonstrates the multipath alignment concept. The read (_top_) is aligned to a graph (_middle_) as a sequence-to-path alignment (_bottom left_) and a multipath alignment (_bottom right_). Notice that multipath alignment aligns the same part of the read to multiple places in the graph, all of which it considers plausible. Also notice that the sequence-to-path alignment corresponds to a single path through the multipath alignment.
![](https://github.com/vgteam/vg/blob/master/doc/figures/multipath.png)

### Multipath alignments in `vg`

In `vg`, multipath alignments are stored in the GAMP (Graph Alignment MultiPath) format, which uses the extension `.gamp`. Like many formats in `vg`, GAMP is a Protocol Buffer-based format with a schema defined in [vg.proto](https://github.com/vgteam/libvgio/blob/master/deps/vg.proto). To program using these objects, developers should look at our [IO library](https://github.com/vgteam/libvgio). For examining GAMPs by eye, they can be converted to a JSON object using `vg view`. Let's take a look at an example (with some manual formatting for readability).

```
# -K for GAMP input, -j for JSON output
> vg view -K -j example.gamp 
{"sequence":"GGGGTTTCACCGTGTTAGCCAGGATGGTC",
 "quality":"GyEhISEhGyEhISEhISEhISEhFhYbDwYGDwYGDw8=",
 "name":"NAME-OF-READ",
 "sample_name":"NAME-OF-SAMPLE",
 "read_group":"NAME-OF-GROUP",
 "start":[0],
 "subpath":[
    {"path":{"mapping":[{"position":{"node_id":"1613"},"edit":[{"from_length":3,"to_length":3}],"rank":"1"}]},
     "next":[1,2],
     "score":8
    },
    {"path":{"mapping":[{"position":{"node_id":"1615"},"edit":[{"from_length":1,"to_length":1,"sequence":"G"}],"rank":"2"}]},
     "next":[3],
     "score":-4
    },
    {"path":{"mapping":[{"position":{"node_id":"1614"},"edit":[{"from_length":1,"to_length":1}],"rank":"2"}]},
     "next":[3],
     "score":1
    },
    {"path":{"mapping":[{"position":{"node_id":"1616"},"edit":[{"from_length":5,"to_length":5}],"rank":"1"},
                        {"position":{"node_id":"1617"},"edit":[{"from_length":1,"to_length":1}],"rank":"2"},
                        {"position":{"node_id":"1619"},"edit":[{"from_length":1,"to_length":1}],"rank":"3"},
                        {"position":{"node_id":"1621"},"edit":[{"from_length":5,"to_length":5}],"rank":"4"},
                        {"position":{"node_id":"1622"},"edit":[{"from_length":1,"to_length":1}],"rank":"5"},
                        {"position":{"node_id":"1624"},"edit":[{"from_length":2,"to_length":2}],"rank":"6"},
                        {"position":{"node_id":"1625"},"edit":[{"from_length":1,"to_length":1}],"rank":"7"},
                        {"position":{"node_id":"1627"},"edit":[{"from_length":9,"to_length":9}],"rank":"8"}]
             },
      "score":30
     }
  ]
}
```
Some of the fields will be familiar to users of the SAM/BAM file formats: `sequence`, `name`, `sample_name`, and `read_group` all are analogous fields. The `quality` field refers to base quality, but the JSON rendering will express the values in [Base64 encoding](https://www.base64decode.org/) rather than Phred. The rest of the fields encode the topology of the multipath alignment. Each of the partial alignments corresponds to one `subpath` record. Each `subpath` contains three pieces of information:
* A `path` that expresses a sequence-to-path alignment of some portion of the read
* A `score` for that part of the alignment
* A list of `next` subpaths which could be concatenated to the end of this partial alignment (i.e. the edges of the multipath alignment graph), referred to by their 0-based index in the array of subpaths.

Finally, the optional field `starts` indicates all of the subpaths, again referred to by 0-based index, that could be the first subpath in a path of partial alignments (i.e. the source nodes in the multipath alignment graph).

In this example there was only one of these records in the GAMP file, but in general this is not the case. A GAMP file typically consists of a series of records, each indicating a multipath alignment of one sequence to a graph. 

### Converting between GAM and GAMP

A GAMP can be converted to a GAM using `vg view`. Since a GAM can only align to a single path, the path through the multipath alignment graph that has the highest score is used (ties broken arbitrarily):

```
# -K for GAMP input, -G for GAM output
> vg view -K -G example.gamp > example.gam
```

GAMs can also be converted to GAMP. This conversion is lossless. It produces a GAMP with a single `subpath`, which corresponds to the GAM's path.

```
# -a for GAM input, -k for GAMP output
> vg view -a -k example.gam > example.gamp
```

## Producing multipath alignments with `vg mpmap`

The `vg` toolkit has a subcommand implemented that can produce multipath alignments against a graph. It is best-optimized for short read sequencing data. However, it is functional for other sequencing types as well. 

The command line interface for `vg mpmap` is mostly similar to `vg map`. Like `vg map`, it requires an XG index of the graph and a GCSA index for substring search (see [Index Construction](https://github.com/vgteam/vg/wiki/Index-Construction)).

```
# -x for XG index, -g for GCSA index, -n for genomic DNA, -f for FASTQ reads
> vg mpmap -x graph.xg -g graph.gcsa -n DNA -f reads.fq > mapped_reads.gamp
```

### Paired end data

Paired end reads are also supported. Read pairs can be given either as two matched FASTQ files or as a FASTQ file with interleaved pairs.

```
# second -f for the matched FASTQ file containing the read pairs
> vg mpmap -x graph.xg -g graph.gcsa -n DNA -f reads_1.fq -f reads_2.fq > mapped_reads.gamp

# -i indicates interleaved read pairs
> vg mpmap -x graph.xg -g graph.gcsa -n DNA -f read_pairs.fq -i > mapped_reads.gamp
```

### Producing better multipath alignment topologies

`vg mpmap` always tries to identify "neighboring" parts of the graph that it should align the same part of the read to. However, this is a challenging problem without any additional information. It can do a much better job with some global information about the structure of the graph. In particular, it wants to know the graph's "snarl decomposition", which is essentially the structure of "bubbles" in the graph (see our [paper](https://www.ncbi.nlm.nih.gov/pubmed/29461862) or [preprint](https://www.biorxiv.org/node/28817.article-metrics) for a more rigorous explanation).

The snarl decomposition should first be computed using the `vg snarls` command:

```
> vg snarls graph.vg > graph.snarls
```

The snarl decomposition can then be passed to `vg mpmap` with the `-s` flag:

```
# -s for the snarl decomposition
> vg mpmap -x graph.xg -g graph.gcsa -s graph.snarls -f reads.fq > mapped_reads.gamp
```

### Using `vg mpmap` as a single path aligner

Since there is a simple conversion from GAMP to GAM, it is possible to use `vg mpmap` as a sequence-to-path mapper as well. To do so, there is an option `-F GAM` to execute in "single path mode", which produces GAM output and implements some heuristics that are more appropriate for single path alignments.

```
# -F GAM to produce GAM instead of GAMP
> vg mpmap -x graph.xg -g graph.gcsa -f reads.fq -F GAM > mapped_reads.gam
```

The `-F` argument can also project alignments onto the reference path with the `SAM` and `BAM` options.

### Configuring `vg mpmap` for different types of data

Like all read mappers, `vg mpmap` uses some computational heuristics to speed up read mapping. Invariably, tuning these heuristics requires making some assumptions about the kind of data the mapper is likely to see. By default, `vg mpmap` expects to see short-read NGS data for genomic DNA. However, the heuristics can be easily tuned using three intuitive presets that can be selected using command line options:

* `-n`/`--nt-type`: `RNA` for transcriptomic data (default), `DNA` for genomic DNA. 
* `-l`/`--read-length`: `very-short`, `short` (default), or `long` for reads that are approximately <50 bp, 50-500 bp, and >500 bp, respectively. 
* `-e`/`--error-rate`: `low` (default) or `high` for reads that have a base-wise accuracy of approximately PHRED >20 or PHRED <20, respectively.


### Additional considerations for using `vg mpmap` for to map RNA-seq reads to splice-variation graphs

`vg mpmap` is best optimized for mapping RNA sequencing data to a splice-variation graph, such as those produced by `vg rna`. For optimal performance, one additional index is needed (for efficiently measuring minimum distances, see our [preprint](https://www.biorxiv.org/content/10.1101/2019.12.20.884924v1?rss=1) or [paper](https://academic.oup.com/bioinformatics/article/36/Supplement_1/i146/5870464)). The total set of indexes can be constructed using default best-practices with `vg autoindex --workflow mpmap`. Alternatively, the indexing pipeline can be built manually at the command line.

To build the distance index, first compute the snarl decomposition of the graph, and then use the `vg index`.

```
# compute snarls, note: use of -T differs from above
> vg snarls -T graph.xg > graph.trivial.snarls
# compute the distance index
> vg index -x graph.xg -s graph.trivial.snarls -j graph.dist
```

A somewhat annoying point (as of April 2020): the snarls computed for the distance index must include "trivial snarls" using the `-T` option, which consist of a single edge along a non-branching path in the graph. However, the snarls we supply to `vg mpmap` (for producing good multipath alignments, see above) ideally should not include the trivial snarls, as they slow down the mapping somewhat. We hope to make this process simpler in a future release.

Now that the distance index is computed, we supply it to `vg mpmap` using the `-d` flag. The distance index encodes all of the information contained in the snarl decomposition. If `-d` is supplied, `-s` is unnecessary.

```
> vg mpmap -x graph.xg -g graph.gcsa -d graph.dist -n rna -f reads.fq > mapped_reads.gamp
```

As an aside, all of the modifications to the `vg mpmap` algorithm discussed above are also compatible with the distance index option.

### Using `vg mpmap` to map reads to graphs with structural variants

It turns out that this problem is very similar to mapping RNA-seq reads. After all, an intron is a type of large-scale deletion (in the transcript). Accordingly, it is possible to effectively map reads to graphs of structural variants using the approach described in the previous section. The only difference is that `-n DNA` should be provided as an option.
