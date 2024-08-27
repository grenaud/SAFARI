% GENOTYPESUMMARY(1) genotypeSummary (vcflib) | genotypeSummary (VCF statistics)
% Erik Garrison and vcflib contributors

# NAME

**genotypeSummary**

# SYNOPSIS

genotypeSummmary --type PL --target 0,1,2,3,4,5,6,7 --file my.vcf --snp

# DESCRIPTION

Generates a table of genotype counts. Summarizes genotype counts for bi-allelic SNVs and indel



# OPTIONS

```


output: table of genotype counts for each individual.
required: t,target     -- a zero based comma separated list of target individuals corresponding to VCF columns        
required: f,file       -- proper formatted VCF                                                                        
required, y,type       -- genotype likelihood format; genotype : GL,PL,GP                                             
optional, r,region     -- a tabix compliant region : chr1:1-1000 or chr1                                              
optional, s,snp        -- Only count SNPs                                              
optional, a,ancestral  -- describe counts relative to the ancestral allele defined as AA in INFO

Type: statistics


```





# EXIT VALUES

**0**
: Success

**not 0**
: Failure

# SEE ALSO



[vcflib](./vcflib.md)(1)



# OTHER

## Source code

[genotypeSummary.cpp](https://github.com/vcflib/vcflib/blob/master/src/genotypeSummary.cpp)

# LICENSE

Copyright 2011-2022 (C) Erik Garrison and vcflib contributors. MIT licensed.

<!--
  Created with ./scripts/bin2md.rb scripts/bin2md-template.erb
-->
