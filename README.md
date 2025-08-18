# SAFARI

## Overview
**vg SAFARI (Sensitive Alignments from a RYmer Index)** is a modified version of the subcommand vg giraffe from the vg toolkit (https://github.com/vgteam/vg).
SAFARI is modified specifically to recover more alignments from ancient DNA samples, which suffer from characteristic substitution patterns due to chemical damage.
SAFARI is a modification of giraffe from a frozen version of vg (Solara, version 1.44). 

## Compilation

`git clone --recurse-submodules https://github.com/grenaud/SAFARI && cd SAFARI && git submodule update --init --recursive && (cd deps/sparsehash && ./configure) && ./source_me.sh && make -j [threads]`

Please note that SAFARI requires many dependencies, including non-standard dependencies like protobuf and jansson, which means that non-root users will likely be unable to compile on their own. We hope these users will either avail themselves of the static binary provided here, or else request compilation from someone with root access.

### Basic Options

- `-Z, --gbz-name FILE` : use this GBZ file (GBWT index + GBWTGraph)
- `-m, --minimizer-name FILE` : use this minimizer index
- `-q, --rymer-name FILE` : use this rymer index
- `-d, --dist-name FILE` : cluster using this distance index
- `-p, --progress` : show progress

### Input Options

- `-G, --gam-in FILE` : read and realign GAM-format reads from FILE
- `-f, --fastq-in FILE` : read and align FASTQ-format reads from FILE (two are allowed, one for each mate)
- `-i, --interleaved` : GAM/FASTQ input is interleaved pairs, for paired-end alignment
- `--deam-3p FILE` : 3' end deamination rate matrix (must end in .prof)
- `--deam-5p FILE` : 5' end deamination rate matrix (must end in .prof)

### Alternate Indexes

- `-x, --xg-name FILE` : use this xg index or graph
- `-g, --graph-name FILE` : use this GBWTGraph
- `-H, --gbwt-name FILE` : use this GBWT index

### Output Options

- `-M, --max-multimaps INT` : produce up to INT alignments for each read [1]
- `-N, --sample NAME` : add this sample name
- `-R, --read-group NAME` : add this read group
- `-o, --output-format NAME` : output the alignments in NAME format (gam / gaf / json / tsv / SAM / BAM / CRAM) [gam]
- `--ref-paths FILE` : ordered list of paths in the graph, one per line or HTSlib .dict, for HTSLib @SQ headers
- `--named-coordinates` : produce GAM outputs in named-segment (GFA) space
- `-P, --prune-low-cplx` : prune short and low complexity anchors during linear format realignment
- `-n, --discard` : discard all output alignments (for profiling)
- `--output-basename NAME` : write output to a GAM file beginning with the given prefix for each setting combination
- `--report-name NAME` : write a TSV of output file and mapping speed to the given file
- `--show-work` : log how the mapper comes to its conclusions about mapping locations

### Algorithm Presets

- `-b, --parameter-preset NAME` : set computational parameters (fast / default) [default]

### Computational Parameters

- `-c, --hit-cap INT` : use all minimizers with at most INT hits [10]
- `-C, --hard-hit-cap INT` : ignore all minimizers with more than INT hits [500]
- `-F, --score-fraction FLOAT` : select minimizers between hit caps until score is FLOAT of total [0.9]
- `-D, --distance-limit INT` : cluster using this distance limit [200]
- `-e, --max-extensions INT` : extend up to INT clusters [800]
- `-a, --max-alignments INT` : align up to INT extensions [8]
- `-s, --cluster-score INT` : only extend clusters if they are within INT of the best score [50]
- `-S, --pad-cluster-score INT` : also extend clusters within INT of above threshold to get a second-best cluster [0]
- `-u, --cluster-coverage FLOAT` : only extend clusters if they are within FLOAT of the best read coverage [0.3]
- `-U, --max-min INT` : use at most INT minimizers [500]
- `--num-bp-per-min INT` : use maximum of number minimizers calculated by READ_LENGTH / INT and --max-min [1000]
- `-v, --extension-score INT` : only align extensions if their score is within INT of the best score [1]
- `-w, --extension-set INT` : only align extension sets if their score is within INT of the best score [20]
- `-O, --no-dp` : disable all gapped alignment
- `--align-from-chains` : chain up extensions to create alignments, instead of doing each separately
- `-r, --rescue-attempts` : attempt up to INT rescues per read in a pair [15]
- `-A, --rescue-algorithm NAME` : use algorithm NAME for rescue (none / dozeu / gssw) [dozeu]
- `-L, --max-fragment-length INT` : assume that fragment lengths should be smaller than INT when estimating the fragment length distribution
- `--exclude-overlapping-min` : exclude overlapping minimizers
- `--fragment-mean FLOAT` : force the fragment length distribution to have this mean (requires --fragment-stdev)
- `--fragment-stdev FLOAT` : force the fragment length distribution to have this standard deviation (requires --fragment-mean)
- `--paired-distance-limit FLOAT` : cluster pairs of read using a distance limit FLOAT standard deviations greater than the mean [2.0]
- `--rescue-subgraph-size FLOAT` : search for rescued alignments FLOAT standard deviations greater than the mean [4.0]
- `--rescue-seed-limit INT` : attempt rescue with at most INT seeds [100]
- `--track-provenance` : track how internal intermediate alignment candidates were arrived at
- `--track-correctness` : track if internal intermediate alignment candidates are correct (implies --track-provenance)
- `-j, --posterior-threshold FLOAT` : cutoff for posterior on correct alignment when using RYmers
- `-V, --spurious-prior FLOAT` : prior on spurious alignment when using RYmers
- `-t, --threads INT` : number of mapping threads to use

## Damage Matrix (.prof) File Format

SAFARI requires to have initial estimates of the damage rates for the 5' and 3' end of the aDNA fragments. However, this is difficult to obtain a priori. There are a ways you can solve this:


1) Initial damage profile estimates can be obtained by first aligning to a linear reference and then running bam2prof (https://github.com/grenaud/bam2prof).

2) Use damage rates previously estimated on other samples. We have provided in the `profs` directory a number of such profiles from  high-visibility papers for samples of various ages sequenced with various library protocols. Of course, preservation conditions will affect these profiles as well.
 

To illustrate the format an example damage matrix file is provided below for the 3' end.

```
A>C     A>G     A>T     C>A     C>G     C>T     G>A     G>C     G>T     T>A     T>C     T>G
0       0       0       0       0       0       0.32891 0       0       0       0       0
0       0       0       0       0       0       0.223405        0       0       0       0       0
0       0       0       0       0       0       0.188599        0       0       0       0       0
0       0       0       0       0       0       0.164419        0       0       0       0       0
0       0       0       0       0       0       0.146352        0       0       0       0       0
```

For the remainder of the molecule, the value of the last line will be copied over onto the remaining positions. e.g. in the example above, we only have 5 lines and at 6 bp away from the 3' end, 0.146352 will be used as the rate of G->A substitution.

## Quickstart

First, get the test files:

```bash
wget -nc -r -l1 --no-parent -nH --cut-dirs=2 -P SAFARI_graph ftp://ftp.healthtech.dtu.dk:/public/SAFARI_graph/
```

Then create the index files:

```bash
bin/vg minimizer -d SAFARI_graph/hominin.dist -g SAFARI_graph/hominin.gbwt -t [# threads] -p -o SAFARI_graph/hominin.min SAFARI_graph/hominin.og

bin/vg rymer -d SAFARI_graph/hominin.dist -g SAFARI_graph/hominin.gbwt -t [# threads] -p -o SAFARI_graph/hominin.ry SAFARI_graph/hominin.og
```

Then run:

```bash
bin/vg safari -f test/SAFARI/reads.fq.gz \
-m SAFARI_graph/hominin.min \
-q SAFARI_graph/hominin.ry \
-Z SAFARI_graph/hominin.giraffe.gbz \
-d SAFARI_graph/hominin.dist \
--deam-3p test/SAFARI/dhigh3p.prof \
--deam-5p test/SAFARI/dhigh3p.prof > SAFARI_test.gam
```

To check the GAM file, you can do

```bash

bin/vg stats -a SAFARI_test.gam

```

## How to get a bam file?

You can obtain a bam file using the command "surject" on the resulting gam file from the vg toolkit (https://github.com/vgteam/vg).

## Dependencies

**BBHash**

Limasset, A., Rizk, G., Chikhi, R., & Peterlongo, P. (2017). Fast and scalable minimal perfect hashing for massive key sets. arXiv preprint arXiv:1702.03154.

**DYNAMIC**

Prezza, N. (2017). A framework of dynamic data structures for string processing. arXiv preprint arXiv:1701.07238.

**FlameGraph**

brendangregg. (2025). FlameGraph [Computer software]. GitHub. https://github.com/brendangregg/FlameGraph

**atomic_queue**

max0x. (2025). atomic_queue [Computer software]. GitHub. https://github.com/max0x7ba/atomic_queue

**backward-cpp**

bombela. (2025). backward-cpp [Computer software]. GitHub. https://github.com/bombela/backward-cpp

**bash-tap**

illusori. (2025). bash-tap [Computer software]. GitHub. https://github.com/illusori/bash-tap

**dozeu**

ocxtal. (2025). dozeu [Computer software]. GitHub. https://github.com/ocxtal/dozeu

**elfutils**

roolebo. (2025). elfutils [Computer software]. GitHub. https://github.com/roolebo/elfutils

**fastahack**

ekg. (2025). fastahack [Computer software]. GitHub. https://github.com/ekg/fastahack

**fermi-lite**

edawson. (2025). fermi-lite [Computer software]. GitHub. https://github.com/edawson/fermi-lite

**gbwt**

Sirén, J., Garrison, E., Novak, A. M., Paten, B., & Durbin, R. (2020). Haplotype-aware graph indexes. Bioinformatics, 36(2), 400-407.

**gbwtgraph**

Sirén, J., Monlong, J., Chang, X., Novak, A. M., Eizenga, J. M., Markello, C., ... & Paten, B. (2021). Pangenomics enables genotyping of known structural variants in 5202 diverse genomes. Science, 374(6574), abg8871.

**gcsa2**

Sirén, J. (2017). Indexing variation graphs. In 2017 Proceedings of the ninteenth workshop on algorithm engineering and experiments (ALENEX) (pp. 13-27). Society for Industrial and Applied Mathematics.

**gssw**

Zhao, M., Lee, W. P., Garrison, E. P., & Marth, G. T. (2013). SSW library: an SIMD Smith-Waterman C/C++ library for use in genomic applications. PloS one, 8(12), e82138.

**htslib**

Bonfield, J. K., Marshall, J., Danecek, P., Li, H., Ohan, V., Whitwham, A., ... & Davies, R. M. (2021). HTSlib: C library for reading/writing high-throughput sequencing data. Gigascience, 10(2), giab007.

**ips4o**

Axtmann, M., Witt, S., Ferizovic, D., & Sanders, P. (2017). In-place Parallel Super Scalar Samplesort (IPS $^ 4$ o). arXiv preprint arXiv:1705.02257.

**jemalloc**

jemalloc. (2025). jemalloc [Computer software]. GitHub. https://github.com/jemalloc/jemalloc

**libVCFH**

edawson. (2025). libVCFH [Computer software]. GitHub. https://github.com/edawson/libVCFH

**libbdsg**

Eizenga, J. M., Novak, A. M., Kobayashi, E., Villani, F., Cisar, C., Heumos, S., ... & Garrison, E. (2020). Efficient dynamic variation graphs. Bioinformatics, 36(21), 5139-5144.

**libdeflate**

ebiggers. (2025). libdeflate [Computer software]. GitHub. https://github.com/ebiggers/libdeflate

**libhandlegraph**

Eizenga, J. M., Novak, A. M., Kobayashi, E., Villani, F., Cisar, C., Heumos, S., ... & Garrison, E. (2020). Efficient dynamic variation graphs. Bioinformatics, 36(21), 5139-5144.

**libvgio**

vgteam. (2025). libvgio [Computer software]. GitHub. https://github.com/vgteam/libvgio

**lru_cache**

ekg. (2025).  lru_cache [Computer software]. GitHub. https://github.com/ekg/lru_cache

**mio**

**mmmultimap**

ekg. (2025). mmmulti [Computer software]. GitHub. https://github.com/ekg/mmmulti

**pinchesAndCacti**

ComparativeGenomicsToolkit. (2025). pinchesAndCacti [Computer software]. GitHub. https://github.com/ComparativeGenomicsToolkit/pinchesAndCacti

**progress_bar**

htailor. (2025). cpp_progress_bar [Computer software]. GitHub. https://github.com/htailor/cpp_progress_bar

**raptor**

dajobe. (2025). raptor [Computer software]. GitHub. [https://github.com/htailor/cpp_progress_bar](https://github.com/dajobe/raptor)

htailor. (2025). cpp_progress_bar [Computer software]. GitHub. https://github.com/htailor/cpp_progress_bar

**sdsl-lite**

Gog, S., Beller, T., Moffat, A., & Petri, M. (2014, June). From theory to practice: Plug and play with succinct data structures. In International Symposium on Experimental Algorithms (pp. 326-337). Cham: Springer International Publishing.

**sha1**

**snappy**

Google. (2025). snappy [Computer software]. GitHub. https://github.com/google/snappy

**sonLib**

benedictpaten. (2025). sonLib [Computer software]. GitHub. https://github.com/benedictpaten/sonLib

**sparsehash**

sparsehash. (2025). sparsehash [Computer software]. GitHub. https://github.com/sparsehash/sparsehash

**sparsepp**

**ssw**

Zhao, M., Lee, W. P., Garrison, E. P., & Marth, G. T. (2013). SSW library: an SIMD Smith-Waterman C/C++ library for use in genomic applications. PloS one, 8(12), e82138.

**structures**

jeizenga (2025). structures [Computer software]. GitHub. https://github.com/jeizenga/structures

**sublinearLi-Stephens**

Rosen, Y. M., & Paten, B. J. (2019). An average-case sublinear forward algorithm for the haploid Li and Stephens model. Algorithms for Molecular Biology, 14(1), 11.

**tabixpp**

vcflib. (2025). tabixpp [Computer software]. GitHub. https://github.com/vcflib/tabixpp

**vcflib**

Garrison, E., Kronenberg, Z. N., Dawson, E. T., Pedersen, B. S., & Prins, P. (2022). A spectrum of free software tools for processing the VCF variant call format: vcflib, bio-vcf, cyvcf2, hts-nim and slivar. PLoS computational biology, 18(5), e1009123.

**xg**

vgteam. (2025). xg [Computer software]. GitHub. https://github.com/vgteam/xg

## Contact

For questions, contact Joshua Rubin (jdru@dtu.dk) or Gabriel Renaud (gabriel.reno@gmail.com)

