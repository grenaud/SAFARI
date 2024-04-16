<!-- !test program bash -eo pipefail -->
# 1000GP index construction

We assume the following directory structure:

* The `vg` tool is on your PATH
* `${CONSTRUCTION}/data` constains the reference (`hs37d5.fa`) and VCF files (`chrN.vcf.gz`) with phasing information.
* `${CONSTRUCTION}/graphs` contains the graphs.
* `${CONSTRUCTION}/indexes` contains the indexes.
* `${CONSTRUCTION}/logs` contains the log files.
* `${LARGE_DISK_TMP}` is a temporary directory with at least 1.5 TB space.

## Graph construction

<!-- !test check Construct an hs37d5 graph -->
```
#!/bin/bash

# If you just want to follow along without really building a whole genome
# graph, from the vg test directory, you can set:
CONSTRUCTION=./wiki-data/mock-hs37d5
LARGE_DISK_TMP="$(dirname "$(mktemp -u)")"

DATA=${CONSTRUCTION}/data
REFERENCE=${DATA}/hs37d5.fa
GRAPHS=${CONSTRUCTION}/graphs
MAPPING=${GRAPHS}/node_mapping

mkdir -p ${GRAPHS}

LOGS=${CONSTRUCTION}/logs
mkdir -p ${LOGS}
LOGFILE=${LOGS}/vg_all.log
rm -f $LOGFILE

START=$(date +%s)
echo "Start time: $START" >> $LOGFILE

# Build the VG graphs
(seq 1 22; echo X; echo Y) | parallel -j 24 \
    "vg construct -r $REFERENCE -v ${DATA}/chr{}.vcf.gz -R {} -C -a > ${GRAPHS}/chr{}.vg"

STOP=$(date +%s)
echo >> $LOGFILE
echo "Checkpoint: $STOP" >> $LOGFILE
echo "Construction: $(($STOP-$START)) seconds" >> $LOGFILE

# Harmonize the node ids
vg ids -j -m $MAPPING $(for i in $(seq 1 22; echo X; echo Y); do echo ${GRAPHS}/chr${i}.vg; done)

STOP=$(date +%s)
echo >> $LOGFILE
echo "Finish time: $STOP" >> $LOGFILE
echo "Total time: $(($STOP-$START)) seconds" >> $LOGFILE
```

## GBWT construction

```
#!/bin/bash

LOGS=${CONSTRUCTION}/logs
DATA=${CONSTRUCTION}/data
GRAPHS=${CONSTRUCTION}/graphs
INDEXES=${CONSTRUCTION}/indexes
JOBS=14

export TMPDIR=${LARGE_DISK_TMP}

OUTPUT=all
GBWT=${INDEXES}/${OUTPUT}.gbwt

LOGFILE=${LOGS}/gbwt_${OUTPUT}.log
rm -f $LOGFILE

START=$(date +%s)
echo "Start time: $START" >> $LOGFILE
echo "Jobs: $JOBS" >> $LOGFILE
echo >> $LOGFILE

XG_ALTS=${TMPDIR}/graph-with-alts.xg

# Build a temporary XG index with alt paths.
vg index -x $XG_ALTS -L -p \
    $(for i in $(seq 1 22; echo X; echo Y); do echo ${GRAPHS}/chr${i}.vg; done) 2>> $LOGFILE

MID=$(date +%s)
echo >> $LOGFILE
echo "Checkpoint: $MID" >> $LOGFILE
echo "Temporary XG construction: $(($MID-$START)) seconds" >> $LOGFILE
echo >> $LOGFILE

# Build the GBWT.
vg gbwt -x $XG_ALTS -o $GBWT --preset 1000gp --num-jobs $JOBS -p -v \
    $(for i in $(seq 1 22; echo X; echo Y); do echo ${DATA}/chr${i}.vcf.gz; done) 2>> $LOGFILE

rm -f $XG_ALTS

STOP=$(date +%s)
echo "Finish time: $STOP" >> $LOGFILE
echo "GBWT construction: $(($STOP-$MID)) seconds" >> $LOGFILE
echo "Total time: $(($STOP-$START)) seconds" >> $LOGFILE
```

## XG construction

```
#!/bin/bash

GRAPHS=${CONSTRUCTION}/graphs
INDEXES=${CONSTRUCTION}/indexes
LOGS=${CONSTRUCTION}/logs

export TMPDIR=${LARGE_DISK_TMP}

OUTPUT=all
XG=${INDEXES}/${OUTPUT}.xg

LOGFILE=${LOGS}/xg_${OUTPUT}.log
rm -f $LOGFILE

START=$(date +%s)
echo "Start time: $START" >> $LOGFILE
echo >> $LOGFILE

vg index -x $XG $(for i in $(seq 1 22; echo X; echo Y); do echo ${GRAPHS}/chr${i}.vg; done)

STOP=$(date +%s)
echo "Finish time: $STOP" >> $LOGFILE
echo "Total time: $(($STOP-$START)) seconds" >> $LOGFILE
```

## Graph pruning

Using a whole-genome GBWT for unfolding haplotypes requires VG **version 1.30.0**.

```
#!/bin/bash

GRAPHS=${CONSTRUCTION}/graphs
INDEXES=${CONSTRUCTION}/indexes
INPUT_MAPPING=${GRAPHS}/node_mapping
OUTPUT_MAPPING=${INDEXES}/node_mapping

export TMPDIR=${LARGE_DISK_TMP}

INPUT=all
GBWT=${INDEXES}/${INPUT}.gbwt

LOGS=${CONSTRUCTION}/logs
LOGFILE=${LOGS}/prune_${INPUT}.log
rm -f $LOGFILE

START=$(date +%s)
echo "Start time: $START" >> $LOGFILE
echo >> $LOGFILE

# Unfold haplotypes
cp $INPUT_MAPPING $OUTPUT_MAPPING
for i in $(seq 1 22; echo X; echo Y); do
  CHR=${GRAPHS}/chr${i}
  GBWT_INDEX=${INDEXES}/${INPUT}.gbwt
  $VG prune -u -g $GBWT -a -m $OUTPUT_MAPPING -p ${CHR}.vg > ${CHR}_unfolded.vg 2>> $LOGFILE
  echo >> $LOGFILE
done

STOP=$(date +%s)
echo "Finish time: $STOP" >> $LOGFILE
echo "Total time: $(($STOP-$START)) seconds" >> $LOGFILE
```

## GCSA construction

```
#!/bin/bash

GRAPHS=${CONSTRUCTION}/graphs
INDEXES=${CONSTRUCTION}/indexes
MAPPING=${INDEXES}/node_mapping
OUTPUT=all

export TMPDIR=${LARGE_DISK_TMP}

LOGS=${CONSTRUCTION}/logs
LOGFILE=${LOGS}/gcsa_${OUTPUT}.log
rm -f $LOGFILE

START=$(date +%s)
echo "Start time: $START" >> $LOGFILE
echo >> $LOGFILE

vg index -g ${INDEXES}/${OUTPUT}.gcsa -f $MAPPING -p \
    $(for i in $(seq 1 22; echo X; echo Y); do echo ${GRAPHS}/chr${i}_unfolded.vg; done) 2>> $LOGFILE
rm -f ${TMPDIR}/vg-kmers-tmp-*
rm -f ${TMPDIR}/gcsa_*

STOP=$(date +%s)
echo >> $LOGFILE
echo "Finish time: $STOP" >> $LOGFILE
echo "Total time: $(($STOP-$START)) seconds" >> $LOGFILE
```
