This document sets out the high-level tasks which the `vg` development team hopes to accomplish in the next few versions of `vg` and beyond.

# By Time

These are the things we hope to achieve on several planning horizons:

### End of Year (12/21)
- [X] Publish `vg mpmap`: revisions (Jordan, Jonas)
- [X] Implement Distance Index 2, which also works as a snarl manager (Xian)
- [X] Functional surject pipeline
    - [X] GBZ Paths (Adam)
    - [X] Functional surject pipeline when coming from GBZ first; need something to surject onto.

### Next 3 Months (3/22)
- [ ] HPRC transcriptomics with `vg mpmap` and `rpvg` (Jordan)
- [ ] Fast HPRC Giraffe (Xian)
- [ ] Normalize a whole-genome graph and see if it improves mapping/calling (Robin)
- [ ] vg deconstruct and Beagle to impute genotypes into partially-mapped and called data, as a PanGenie alternative (Erik, Andrea)
- [ ] vg deconstruct PanGenie tag generation (Erik)

- [ ] User-facing, under-test Giraffe docs for HPRC graphs (Jordan)
    - [ ] Include getting alignments out in input GFA coordinates
- [ ] Long read Giraffe (Stephen)
- [ ] Adapt all snarl usage to go through libsnarls (Adam, Jordan)
    - [ ] Shim `Snarl*` as a non-Protobuf adapter type?
    - [ ] DI2 snarl manager with new API in libsnarls
- [ ] Less cruft in `vg index` [#3144](https://github.com/vgteam/vg/issues/3144) (Adam, Jordan)
- [ ] Idiomatic Giraffe on long-node rGFA
    - [ ] Input and Output "stable" coordinates in GAF
    - [ ] Output rGFA-style cover of vg graph, using prioritization of existing paths as option, inventing paths if necessary
    - [ ] Node auto-chopping from rGFA is the last missing piece of [#3126](https://github.com/vgteam/vg/issues/3126)
    - [ ] Chopping overlay (Jordan)
    - [ ] Saving, loading, and using coordinate translations to/from node-coalesced, string-ID'd input GFA space
        - [ ] HG interface and implementation in libbdsg graphs?
    - [ ] Back-translate mappings
    - [ ] Implicit node chopping on GFA input
- [ ] RNA project
  - [ ] dbGAP access
  - [ ] Better splice junction identification/graph distance (Jordan)
- [ ] HPRC tooling
  - [ ] Let surject generate supplementary alignments for e.g. mappings over inversions
    - [ ] Eliminate intermediate `Alignment`, go graph `Alignment` -> BAM record(s)

Also:

- [ ] Eliminate `vg::VG` (Jordan)
    - [ ] Steal all the things only it can do away from it
- [ ] Default everything to GAF instead of GAM
    - [ ] mpGAF (Jordan, Jonas)
    - [ ] Also [pgvf](https://github.com/pangenome/pgvf-spec) (Graph to graph)
    - [ ] Calls and snarls in one of these?
- [ ] Get rid of GFAKluge!
- [ ] Full subpath support in vg (Adam, Jordan)
    - [ ] HG API support (see old Github issue on handlegraph)
    - [ ] Plugging in to tools
- [ ] Drop pinchesAndCacti and sonlib
    - [ ] Drop Cactus-library-based snarl finder (Adam)
- [ ] Python bindings for `libhandlegraph` algorithms
    - [ ] Are they the right algorithms?


### Next Year (9/22)
- [ ] Pipeline plan for long read Giraffe (Stephen)
    - [ ] Chain minimizers (Xian)
    - [ ] Generalize DP
    - [ ] Replace gapless extension with gapped WFA
- [ ] Giraffe on really complex graphs: Non-minimum distance index? Linear layout distance index? Inject from GBWT path set mappings?
- [ ] Use memory-mapped graphs
    - [ ] For tube map, to enable interactive whole-genome use (Future data vis enthusiast)
    - [ ] For Giraffe
- [ ] Get GBWT build working in under 200 GB memory on 100m variants with fancy disk-backed in-progress GBWT implementation (need 300m random access vectors that grow independently)
- [ ] Support Erik's multi-level graph format when mature
- [ ] Redesign and reorganize little tools (Where should each manipulation live? Should some just be scripts you write?)
    - [ ] `vg mod`
    - [ ] `vg chunk`
    - [ ] `vg circularize`
    - [ ] `vg view`
    - [ ] `vg paths`

## Running Projects

These are things we are working on, with no particular delivery date goal.

- [ ] Use of MCMC techniques in the genotyper with multipath alignments 

## Wishlist

These are things we would like to do eventually.

- Alignment
    - [ ] Adoption of the multipath alignment paradigm as the default
    - [ ] Graph-to-graph mapping (Xian)
- Variant Calling
    - [ ] Implementation of an HHGA-like machine learning based variant caller
    - [ ] Integration of variant calling and assembly polishing processes
    - [ ] Prune the zoo of TraversalFinders, and expose the useful ones to Python
- Visualization
    - [ ] Browser-free tube map
    - [ ] Better tube map handling of edge cases
        - [ ] No haplotypes on a node
        - [ ] Starting on a rare haplotype
- Infrastructure
    - [ ] Destructively modernize and unify IO
        - [ ] Eliminate VPKG framing if possible in favor of magic numbers everywhere
            - [ ] Resolve ensuing questions about GAM format
                - [ ] Just use GAF?
            - [ ] Handle things like GFA that need to manually sniff
        - [ ] Just save from the object; no more `save_handle_graph`
        - [ ] Magic format registration for `libvgio` magic numbers for loading
        - [ ] Depend on `libvgio` in `libbdsg` to do the IO there and pick the right handle graph implementation
    - [ ] Replace Protobuf internal formats with faster ones
    - [ ] Revision of ID assignment logic to allow deterministic node breaking
    - [ ] Accept gzipped GFA if practical (can't mmap)
    - [ ] Improved HandleGraph API
        - [ ] Abstract away node boundaries
        - [ ] View all sequence as C++17 string_views instead of sequence-owning strings
        - [ ] O(1) reverse complement DNAStringView
    - [ ] CMake-ify the main vg build
    - [ ] Eliminate old systems and their associated submodules, or factor them out into their own projects
        - [ ] `vg vectorize` could be its own project
            - [ ] Update `vg vectorize` to modern, system Vowpal Wabbit
            - [ ] Or pull it out into its own submodule and remove Vowpal Wabbit dependency from vg
        - [ ] Eliminate RocksDB from vg; everybody using `vg map` uses GCSA indexes now.
        - [ ] `vg genotype`
        - [ ] `vg srpe`
    - [ ] More cross-language support
        - [ ] Interoperate with Rust handle graph users/providers
        - [ ] Interoperate with Java handle graph users/providers
        

    








