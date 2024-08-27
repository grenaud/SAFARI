% VCFCREATEMULTI(1) vcfcreatemulti (vcflib) | vcfcreatemulti (VCF transformation)
% Erik Garrison and vcflib contributors

# NAME

**vcfcreatemulti**

# SYNOPSIS

**vcfcreatemulti** [options] [file]

# DESCRIPTION

If overlapping alleles are represented across multiple records, merge them into a single record. Currently only for indels.



# OPTIONS

```


Type: transformation

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

[vcfcreatemulti.cpp](https://github.com/vcflib/vcflib/blob/master/src/vcfcreatemulti.cpp)

# LICENSE

Copyright 2011-2022 (C) Erik Garrison and vcflib contributors. MIT licensed.

<!--
  Created with ./scripts/bin2md.rb scripts/bin2md-template.erb
-->
