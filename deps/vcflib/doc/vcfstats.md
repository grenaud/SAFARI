% VCFSTATS(1) vcfstats (vcflib) | vcfstats (VCF statistics)
% Erik Garrison and vcflib contributors

# NAME

**vcfstats**

# SYNOPSIS

**vcfstats** [options] <vcf file>

# DESCRIPTION

Prints statistics about variants in the input VCF file.



# OPTIONS

```


    -r, --region          specify a region on which to target the stats, requires a BGZF
                          compressed file which has been indexed with tabix.  any number of
                          regions may be specified.
    -a, --add-info        add the statistics intermediate information to the VCF file,
                          writing out VCF records instead of summary statistics
    -t, --add-type        only add the type= field to the info (faster than -a)
    -l, --no-length-frequency    don't out the indel and mnp length-frequency spectra
    -m, --match-score N          match score for SW algorithm
    -x, --mismatch-score N       mismatch score for SW algorithm
    -o, --gap-open-penalty N     gap open penalty for SW algorithm
    -e, --gap-extend-penalty N   gap extension penalty for SW algorithm


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

[vcfstats.cpp](https://github.com/vcflib/vcflib/blob/master/src/vcfstats.cpp)

# LICENSE

Copyright 2011-2022 (C) Erik Garrison and vcflib contributors. MIT licensed.

<!--
  Created with ./scripts/bin2md.rb scripts/bin2md-template.erb
-->
