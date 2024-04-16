In [[vg construct]], we take a VCF file and the reference sequence it was based on and generate a variation graph suitable for use by other tools in vg. In the construction process we interpret the VCF+ref combination as a [directed acyclic sequence graph](https://en.wikipedia.org/wiki/Directed_acyclic_graph). Variants in the input are decomposed using pairwise alignment between the reference and alternate, so that the input is interpreted as if it had been passed through [vcfallelicprimitives](https://github.com/ekg/vcflib#vcfallelicprimitives).

The construction process can be distributed, as we'll construct the whole genome graph one chromosome at a time. We specify which chromosome we want to work on by giving `vg construct` the `-R`/`--region` parameter, which limits the constructed graph to a particular reference range. Any samtools-format region specifier will work, but here we'll use whole chromosomes. Presently, it's advisable to break the construction apart due to memory requirements of vg--- it is optimized for high performance mutable graphs, and stores them in memory in a very non-succinct way.

Now we make a file `$i.vg` for each chromosome `$i`:

```bash
ref=hs37d5.fa
vars=ALL.wgs.phase3_shapeit2_mvncall_integrated_v5b.20130502.sites.vcf.gz

echo constructing graph
(seq 1 22; echo X; echo Y) | parallel -j 24 "time vg construct -C -R {} -r $ref -v $vars -t 1 -m 32 >{}.vg"
```

This step should take about half an hour when using 24 cores. The result will be around 4.5G of `.vg` files, which are compressed [protobufv3](github.com/google/protobuf) defined by the vg [format specification](https://github.com/ekg/vg/blob/master/vg.proto) with a chunked on-disk format implemented by [stream.hpp](https://github.com/ekg/stream).