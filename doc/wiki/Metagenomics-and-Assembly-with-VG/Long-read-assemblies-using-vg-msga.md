## long read assembly

`vg` can be use to combine long sequences into a single graph. A typical use case would be where we want to make a graph from homologous chromosomes or sequences assembled into finished sequences. We can then use functions provided by `vg` to construct a graph from these sequences. The process is the fundamental one in `vg`; we align, edit, and index the developing graph repeatedly to build out an assembly. We can execute these operations in a programmatic fashion on graphs and sequences in FASTA files using `vg msga`.

We start with a graph. This is either given as input (`vg msga -g x.vg`) or generated from the first sequence we want to use in the assembly.

1. From there, we progressively build out the assembly by adding a single long read at a time using the banded alignment functionality provided by the `vg::Mapper`.
2. After each alignment is made, we edit the graph to include it using `vg::VG::edit()`,
3. cut the nodes so they are under a maximum length (say 32 or 64 bases),
4. ensure the editing process produces a valid graph and the sequence we've aligned was correctly embedded in the graph,
5. rebuild the XG and GCSA2 indexes, and continue the process with the next sequence.

## example

Using the [HGVM pilot regions](http://public.gi.ucsc.edu/~anovak/hgvm/), we can make an assembly of the GRCh38 alternate haplotypes for the MHC.

```bash
wget http://hgwdev.sdsc.edu/~anovak/hgvm/hgvm.tar.gz
tar xzvf hgvm.tar.gz
cd hgvm/MHC
cat ref.fa G*fa >MHC.fa
```

Now, you will need a large-RAM machine. At least 20G should be safe, but 16G may work.

Unfortunately, MSGA is sensitive to parameters, and they are not easy to dynamically determine. I have also not set the most sensible default parameters for generating assemblies from very long sequences. So, as of e2e92c2f, here is how I've been able to make a graph from the MHC region in reasonable time:

```bash
vg msga -f MHC.fa \ # use MHC.fa as our input
    -b ref \        # the >ref sequence is our basis
    -B 128 \        # align 128-mer bands of the sequences
    -K 11 \         # use 16-mers as the basis for our indexing
    -X 2 \          # double twice in GCSA2, generating a 44-mer deBruijn graph
    -E 3 \          # remove edges from the graph that would allow us to cross 3 bifurcations in 11bp
    -G -S 0.95 \    # greedily accept alignments when they match at 95% of our maximum alignment score
    -H 5 \          # ignore MEMs which have more than 5 hits
    -D \            # basic debugging output, describing progression through the process
    >MHC.vg         # write the output to MHC.vg
```

This takes about 1 hour on my 32-core system, and makes a graph that's 18M on disk and includes about 10.8M of sequence:

```bash
vg stats -lz MHC.vg
#nodes   668465   
#edges   740774   
#length  10802468
```

I can also tell that much of the graph is in strongly connected (cyclic) components:

```bash
vg stats -c MHC.vg | wc -l
# 8917  (many fewer components than nodes)
```

It often makes sense to run a round of normalization on the graph. This removes some redundant sequences--- for instance nodes with identical graph contexts and sequences can be safely merged. Normalization will also merge nodes across edges that are singular between the nodes. We then chop the nodes to be small again, which makes it compatible with the design currently used in vg and GCSA2, where nodes are not extremely long (GCSA2 implements a hard maximum of 1024, and vg's mapper may not perform well with very long nodes).

```bash
vg mod -n MHC.vg | vg mod -X 32 - | vg ids -c - >MHC.norm.vg
```