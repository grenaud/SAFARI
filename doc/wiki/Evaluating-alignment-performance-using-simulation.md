## Simulating the truth

Understanding mapper performance is difficult in the case of real data. We can't easily know the true position of a read in a reference genome, and if we could it would only be possible by using a read aligner to find the likely positions. In consequence, we need to simulate reads in order to be certain of the true alignment position. Here we walk through a process that allows us to evaluate mapper performance using reads sampled with error from a pangenome. Scripts included in the vg repository's `scripts/` directory simplify this process.

### The pangenome

We can use any variation graph as a basis for comparison, but our comparison will ultimately rely on embedded reference paths in the graph in order to generate comparisons with non-graph aligners. In effect this limits us to using VCF-based variation graphs as our basis graph.

### Using vg sim

`vg sim` allows us to sample reads from a variation graph with configurable levels of error. Reads can be single or paired ended. A basic pair model that is typical for Illumina short read data is set by default, and the mean and variance may be configured as desired.

## Annotating the truth with positions

```bash
vg annotate -x SGRP2/SGRP2-cerevisiae.xg -p -a F271.100k.ilv.gam | vg view -a - | pv -l | jq -r '[.name, .refpos[0].name, .refpos[0].offset] | @tsv' | sort >F271.100k.ilv.true_pos
```

## Aligning with vg map and friends

Uses `scripts/pos_compare.py` as a helper.

### vg map

```bash
vg map -iG F271.100k.ilv.gam -x SGRP2/SGRP2-cerevisiae.pathonly.xg -g SGRP2/SGRP2-cerevisiae.pathonly.gcsa -t 4 2>err | vg annotate -x SGRP2/SGRP2-cerevisiae.pathonly.xg -p -a - | vg view -a - | pv -l | jq -rc  '[.name, .refpos[0].name, .refpos[0].offset, if .mapping_quality == null then 0 else .mapping_quality end ] | @tsv'  | sed s/null/0/g | sort >x.pos; join F271.100k.ilv.true_pos x.pos | ./pos_compare.py >vg-ref.compare
```

```bash
vg map -iG F271.100k.ilv.gam -x SGRP2/SGRP2-cerevisiae.xg -g SGRP2/SGRP2-cerevisiae.gcsa -t 4 2>err | vg annotate -x SGRP2/SGRP2-cerevisiae.xg -p -a - | vg view -a - | pv -l | jq -rc  '[.name, .refpos[0].name, .refpos[0].offset, if .mapping_quality == null then 0 else .mapping_quality end ] | @tsv'  | sed s/null/0/g | sort >x.pos; join F271.100k.ilv.true_pos x.pos | ./pos_compare.py >vg-pan.compare
```

### bwa mem

```bash
bwa mem SGRP2/SGD_2010.fasta F271.100k.ilv_1.fq.gz F271.100k.ilv_2.fq.gz | grep -v ^@ | perl -ne '@val = split("\t", $_); print @val[0] . "_" . (@val[1] & 64 ? "1" : @val[1] & 128 ? "2" : "?"), "\t" . @val[2] . "\t" . @val[3] . "\t" . @val[4] . "\n";' | pv -l | sort >x.pos; join F271.100k.ilv.true_pos x.pas | ./pos_compare.py | awk 'BEGIN { OFS="\t"; print "correct", "mq", "aligner"; } { print $2, $3, "bwa.mem" }' >bwa.compare
```

Combine the results:

```bash
cat bwa.compare >x.tsv
file=vg-ref.compare ; cat $file | awk 'BEGIN { OFS="\t"; } { print $2, $3, "'$file'" }' >>x.tsv
file=vg-pan.compare ; cat $file | awk 'BEGIN { OFS="\t"; } { print $2, $3, "'$file'" }' >>x.tsv
```

## Generating the ROC and Q-Q plots