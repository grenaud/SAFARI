<!-- !test program bash -eo pipefail -->
Some vg formats, like GBZ, distinguish between paths of different types, or "senses".

For example, we can put a P-line path `chr1` and a haplotype for sample `sample` on contig `chr1` into a GBZ file:

<!-- !test check Build a demonstration GBZ -->
```sh
cat >demo.gfa <<EOF
H	VN:Z:1.1
S	1	GATTACT
S	2	A
S	3	T
S	4	CATTAG
L	1	+	2	+	*
L	1	+	3	+	*
L	2	+	4	+	*
L	3	+	4	+	*
P	chr1	1+,2+,4+	*
W	sample1	0	chr1	0	14	>1>3>4
W	sample2	0	chr1	0	14	>1>2>4
EOF
vg gbwt -G demo.gfa --gbz-format -g demo.gbz
rm demo.gfa
```

Then we can inspect the path metadata as a TSV with `vg paths -M`:

<!-- !test in Dump path metadata -->
```sh
vg paths -M -x demo.gbz
```

The result will be:

<!-- !test out Dump path metadata -->
```
#NAME	SENSE	SAMPLE	HAPLOTYPE	LOCUS	PHASE_BLOCK	SUBRANGE
chr1	GENERIC	NO_SAMPLE_NAME	NO_HAPLOTYPE	chr1	NO_PHASE_BLOCK	NO_SUBRANGE
sample1#0#chr1#0	HAPLOTYPE	sample1	0	chr1	0	NO_SUBRANGE
sample2#0#chr1#0	HAPLOTYPE	sample2	0	chr1	0	NO_SUBRANGE
```

Tools like `vg surject` won't operate on haplotype paths by default. If we want to change the graph to make a haplotype path into a reference path, we can apply a transformation when converting formats to promote haplotypes for a sample:

<!-- !test check Convert to HashGraph while changing path sense -->
```sh
vg convert -a --ref-sample sample1 demo.gbz >demo-promoted.vg
```

If we check the path metadata again:

<!-- !test in Dump path metadata a second time -->
```sh
vg paths -M -x demo-promoted.vg
```

The result will be:

<!-- !test out Dump path metadata a second time -->
```
#NAME	SENSE	SAMPLE	HAPLOTYPE	LOCUS	PHASE_BLOCK	SUBRANGE
chr1	GENERIC	NO_SAMPLE_NAME	NO_HAPLOTYPE	chr1	NO_PHASE_BLOCK	NO_SUBRANGE
sample1#chr1	REFERENCE	sample1	NO_HAPLOTYPE	chr1	NO_PHASE_BLOCK	NO_SUBRANGE
sample2#0#chr1#0	HAPLOTYPE	sample2	0	chr1	0	NO_SUBRANGE
```

Note that the sense of the path for sample `sample1` has changed to reference, and that its haplotype and phase block information have been removed. (It is safe to remove the haplotype because it is `0`, the value used for a haploid haplotype. If we were using a diploid sample, `1` and `2` would have been used for the haplotypes, and they would have been preserved.)

If we decide we don't want the old original `chr1` path in there anymore, we can remove all paths with a `chr1` prefix:

<!-- !test check Remove unwanted paths  -->
```sh
vg paths -x demo-promoted.vg -d -Q chr1 >demo-removed.vg
```

Then we can list the path names:

<!-- !test in Dump path names -->
```sh
vg paths -L -x demo-removed.vg
```

And we will see that it is no longer there:

<!-- !test out Dump path names -->
```
sample1#chr1
sample2#0#chr1#0
```

Converting the resulting graph back to GBZ format, for use with `vg giraffe`, can be done like this:

<!-- !test check Rebuild gbwt -->
```sh
vg gbwt -x demo-removed.vg --index-paths -g demo-reindexed.gbz --gbz-format 
```

Then we can look for generic paths:

<!-- !test in Count generic paths after reindexing -->
```sh
vg paths -L -G -x demo-reindexed.gbz | wc -l
```

And we will see that there aren't any:

<!-- !test out Count generic paths after reindexing -->
```
0
```

## Large GBZ and GBWT files with Many Haplotypes

When there are a lot of haplotypes stored in a GBWT or GBZ, it can be infeasible to convert to `.vg` format and back to manipulate the references. In this case, you can edit the GBWT or GBZ file to adjust how the stored paths are interpreted.

In a GBWT or GBZ, some samples have reference paths, and other samples have haplotype paths. The samples which have reference paths are stored in a `reference_samples` tag in the GBWT tags section, analogous to the `RS` GFA tag that vg supports. This tag contains a space-separated list of reference sample names.

If we look at this tag in the GBZ file with a reference that we just prepared:

<!-- !test in Dump tags -->
```sh
vg gbwt -Z --tags demo-reindexed.gbz | grep "^reference_samples"
```

The result will be:

<!-- !test out Dump tags -->
```
reference_samples	sample1
```

We can adjust the tag and write a modified GBZ file:

<!-- !test check Change tags -->
```sh
vg gbwt -Z --set-tag "reference_samples=sample2" --gbz-format -g demo-retagged.gbz demo-reindexed.gbz
```

After that, if we inspect the path metadata:

<!-- !test in Dump retagged path metadata -->
```sh
vg paths -M -x demo-retagged.gbz
```

The result will be:

<!-- !test out Dump retagged path metadata -->
```
#NAME	SENSE	SAMPLE	HAPLOTYPE	LOCUS	PHASE_BLOCK	SUBRANGE
sample1#0#chr1#0	HAPLOTYPE	sample1	0	chr1	0	NO_SUBRANGE
sample2#0#chr1	REFERENCE	sample2	0	chr1	NO_PHASE_BLOCK	NO_SUBRANGE
```

## Cleanup

To clean up:

<!-- !test check Clean up -->
```
rm demo.gbz demo-promoted.vg demo-removed.vg demo-reindexed.gbz demo-retagged.gbz
```
