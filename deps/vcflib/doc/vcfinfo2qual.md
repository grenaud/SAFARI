% VCFINFO2QUAL(1) vcfinfo2qual (vcflib) | vcfinfo2qual (VCF transformation)
% Erik Garrison and vcflib contributors

# NAME

**vcfinfo2qual**

# SYNOPSIS

**vcfinfo2qual** [key] [vcf_file]

# DESCRIPTION

Sets QUAL from info field tag keyed by [key]. The VCF file may be omitted and read from stdin. The average of the field is used if it contains multiple values.



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

[vcfinfo2qual.cpp](https://github.com/vcflib/vcflib/blob/master/src/vcfinfo2qual.cpp)

# LICENSE

Copyright 2011-2022 (C) Erik Garrison and vcflib contributors. MIT licensed.

<!--
  Created with ./scripts/bin2md.rb scripts/bin2md-template.erb
-->
