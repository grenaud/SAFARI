## Update (2020-09-02):

This wiki is out of date and contains broken links.  Please see https://github.com/vgteam/vg/issues/2969 for details.  

## Installing Cactus and hal2vg
 
[hal2vg](https://github.com/ComparativeGenomicsToolkit/hal2vg/blob/master/README.md) can be used to import multiple sequence alignments into vg format.  Assuming vg has already been built, the steps for building hal2vg are as follows

1) Download and build Cactus and source its envoronment
```
git clone https://github.com/glennhickey/progressiveCactus.git --recursive
cd progressiveCactus
make
. environment
```

2) Download and build hal2vg (from within the Progressive Cactus environment)
```
cd ..
git clone https://github.com/ComparativeGenomicsToolkit/hal2vg.git --recursive
cd hal2vg
# EDIT INCLUDE.MK SO THAT VGDIR POINTS TO THE PATH OF VG ON YOUR COMPUTER
make
```
## Aligning Yeast assemblies and converting to vg

These are the steps that were used in the manuscript.  They refer to configuration files found here (that must be edited to fix input paths): https://github.com/ComparativeGenomicsToolkit/hal2vg/tree/master/example_data
### genome alignment
```
progressiveCactus/bin/runProgressiveCactus.sh ./cerevisiae.txt  ./work2 ./work2/yeast.hal --configFile ./similar_yeast_config.xml --maxCpus 20

hal2vg work2/yeast.hal --noAncestors --rootGenome Anc01 --refGenome S288c --inMemory > work2/yeast_h2v.vg
vg mod -X 32 work2/yeast_h2v.vg | vg ids -s - > work2/cactus_yeast.vg
```

### chromosome at a time alignment
```
for i in I II III IV V VI VII VIII IX X XI XII XIII XIV XV XVI
do
cat cerevisiae_chrom.txt | sed -e "s/CHROM/chr${i}/g" > cerevisiae_chr${i}.txt
~/genomes/progressiveCactus/bin/runProgressiveCactus.sh ./cerevisiae_chr${i}.txt  ./work3_chrom${i} ./work3_chrom${i}/yeast_chrom${i}.hal --configFile ./similar_yeast_config.xml --maxCpus 20
done

for i in I II III IV V VI VII VIII IX X XI XII XIII XIV XV XVI
do
hal2vg work3_chrom${i}/yeast_chrom${i}.hal  --noAncestors --rootGenome Anc01 --refGenome S288c --inMemory > work3_chrom${i}/yeast_chrom${i}_c2v.vg
vg mod -X 32 work3_chrom${i}/yeast_chrom${i}_c2v.vg | vg ids -s - > work3_chrom${i}/cactus_yeast_chrom${i}.vg
done
vg ids -j $(for i in I II III IV V VI VII VIII IX X XI XII XIII XIV XV XVI; do echo work3_chrom${i}/cactus_yeast_chrom${i}.vg; done)
```
