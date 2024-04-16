libVCFH
------------
Eric T Dawson  
December 2015  

libVCFH is a set of data structures you can populate to print a VCf header. It should
be in spec with VCF4.1/4.2.

## Build it  
`make`


## Test it  
`make test; ./example.exe`


## Use it  
Just import vcfheader.hpp into your project and link against the library. This can be done with
``-L<PathToVCFHLib> -I<PathToHeader> -lvcfh``. Then, in your code you can build up a vcf header and
send it to an ostream.  

            #include <vcfheader.hpp>
            using namespace std;
            using namespace vcfh;
            int main(){
                Header h;
                h.set_date();
                h.set_source("MyProgram");
                h.set_reference("my_reference");
                h.set_contig("Contig");
                h.set_version("VCF4.2");
                struct vcfh_info vi;
                vi.id = "test";
                vi.number = "2";
                vi.type = "string";
                vi.description = "Placeholder";
                h.add_info(vi);
                cerr << h << endl;
            }

## Filing bugs and feature requests
Use the [github](https://github.com/edawson/libVCFH) if you need help!
