This page tracks all changes since the last release of VG.

When you merge a PR, add a bullet point to this page.

When you do a release, copy the changelog from this page and clear it out.

# Changes For Next Release
* `vg inject` will report an error when paths don't exist in the graph, instead of doing undefined behavior.
* vg now has metadata for paths; haplotype paths can now exist in places other than a GBWT. Try `vg paths -M -x <graph>` on a vg, GFA, or GBZ file to see it.
* `vg convert` now has a `--ref-sample` option for promoting a haplotype to a reference during conversion.
* `vg gbwt` and `vg index` no longer have `--paths-as-samples`; prepare input with paths of the appropriate senses instead.
* vg will interpret W lines in input GFA files as haplotypes
* vg will interpret W lines with `*` as the sample name as generic named paths.
* vg will interpret PanSN P line names in input GFA files as haplotypes
* vg will interpret PanSN P line names with `#<number>` after them as (pieces of) haplotypes
* vg will interpret string-type `RS` tags in GFA headers as space-separated lists of samples whose paths are references, not haplotypes.
* vg will generate GFA files using W lines for reference and haplotype paths, and containing `RS` header tags to distinguish them.


# Updated Submodules
* The `dozeu` submodule URL was changed. You will need to `git submodule sync`.

# New Submodules

# Removed submodules
* Removed `gfakluge` submodule.



