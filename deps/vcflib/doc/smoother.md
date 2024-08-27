% SMOOTHER(1) smoother (vcflib) | smoother (VCF transformation)
% Erik Garrison and vcflib contributors

# NAME

**smoother**

# SYNOPSIS

**smoother** --format pFst --file GPA.output.txt

# DESCRIPTION

smoothes is a method for window smoothing many of the GPAT++ formats.



# OPTIONS

```


      **smoother** averages a set of scores over a sliding genomic window.            
      **smoother** slides over genomic positions not the SNP indices. In other words  
      the number of scores within a window will not be constant. The last         
      window for each seqid can be smaller than the defined window size.          
      **smoother** automatically analyses different seqids separately.                
Output : 4 columns :     
     1. seqid            
     2. window start     
     2. window end       
     3. averaged score   

required: f,file     -- argument: a file created by GPAT++                           
required: o,format   -- argument: format of input file, case sensitive               
                              available format options:                                    
                                wcFst, pFst, bFst, iHS, xpEHH, abba-baba, col3             
optional: w,window   -- argument: size of genomic window in base pairs (default 5000)
optional: s,step     -- argument: window step size in base pairs (default 1000)      
optional: t,truncate -- flag    : end last window at last position (zero based)      

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

[smoother.cpp](https://github.com/vcflib/vcflib/blob/master/src/smoother.cpp)

# LICENSE

Copyright 2011-2022 (C) Erik Garrison and vcflib contributors. MIT licensed.

<!--
  Created with ./scripts/bin2md.rb scripts/bin2md-template.erb
-->
