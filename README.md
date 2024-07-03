# SAFARI

## Overview
**vg SAFARI (Sensitive Alignments from a RYmer Index)** is a modified version of the subcommand vg giraffe from the vg toolkit.
SAFARI is modified specifically to recover more alignments from ancient DNA samples, which suffer from characteristic substitution patterns due to chemical damage.
SAFARI is a modification of giraffe from a frozen version of vg (Solara, version 1.44). 

## Compilation

`git clone --recursive https://github.com/grenaud/SAFARI && cd SAFARI && ./source_me.sh && make -j [threads] || true && make clean && make -j [threads]`

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

Initial damage profile estimates can be obtained by first aligning to a linear reference and then running bam2prof (https://github.com/grenaud/bam2prof).


To illustrate the format an example damage matrix file is provided below.

```
A>C     A>G     A>T     C>A     C>G     C>T     G>A     G>C     G>T     T>A     T>C     T>G
0       0       0       0       0       0       0.32891 0       0       0       0       0
0       0       0       0       0       0       0.223405        0       0       0       0       0
0       0       0       0       0       0       0.188599        0       0       0       0       0
0       0       0       0       0       0       0.164419        0       0       0       0       0
0       0       0       0       0       0       0.146352        0       0       0       0       0
```


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
bin/vg safari -f test/SAFARI/reads.fq
-m SAFARI_graph/hominin.min
-q SAFARI_graph/hominin.ry
-Z SAFARI_graph/hominin.giraffe.gbz
-d SAFARI_graph/hominin.dist
--deam-3p test/SAFARI/dhigh3p.prof
--deam-5p test/SAFARI/dhigh3p.prof > SAFARI_test.gam
```

To check the GAM file, you can do

```bash

bin/vg stats -a SAFARI_test.gam

```

## Contact

For questions, contact Joshua Rubin (jdru@dtu.dk) or Gabriel Renaud (gabriel.reno@gmail.com)

