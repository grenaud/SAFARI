#include "vcfheader.hpp"
using namespace std;
using namespace vcfh;




int main(){
    Header h;
    h.set_reference("x.fa");
    h.set_contig("c");
    h.set_date();
    h.set_source("mine");
    h.set_version("4.1");
    cerr << h << endl;
    return 0;
}
