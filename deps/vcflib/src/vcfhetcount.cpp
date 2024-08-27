/*
    vcflib C++ library for parsing and manipulating VCF files

    Copyright © 2010-2020 Erik Garrison
    Copyright © 2020      Pjotr Prins

    This software is published under the MIT License. See the LICENSE file.
*/

#include "Variant.h"
#include "split.h"
#include <string>
#include <iostream>

using namespace std;
using namespace vcflib;


int main(int argc, char** argv) {

  if (argc == 2) {
    string h_flag = argv[1];
    if (h_flag == "-h" || h_flag == "--help") {
      cerr << "usage: " << argv[0] << " <vcf file>" << endl << endl
           << "Calculate the heterozygosity rate: " << endl
           << "count the number of alternate alleles in heterozygous genotypes in all records in the vcf file" << endl
           << "outputs a count for each individual in the file" << endl;
      cerr << endl << "Type: metrics" << endl << endl;
        return 1;
    }
  }


    string inputFilename;
    VariantCallFile variantFile;
    if (optind == argc - 1) {
        inputFilename = argv[optind];
        variantFile.open(inputFilename);
    } else {
        variantFile.open(std::cin);
    }

    if (!variantFile.is_open()) {
        return 1;
    }

    unsigned int hetAlleleCount = 0;
    map<string, unsigned int> hetCounts;
    for (vector<string>::iterator s = variantFile.sampleNames.begin(); s != variantFile.sampleNames.end(); ++s) {
        hetCounts[*s] = 0;
    }

    Variant var(variantFile);
    while (variantFile.getNextVariant(var)) {
        //cout << var << endl;
        for (map<string, map<string, vector<string> > >::iterator s = var.samples.begin(); s != var.samples.end(); ++s) {
            string name = s->first;
            map<string, vector<string> >& sample = s->second;
            string& genotype = sample["GT"].front();
            vector<string> gt = split(genotype, "|/");
            int alt = 0;
            for (vector<string>::iterator g = gt.begin(); g != gt.end(); ++g) {
                if (*g != "0")
                    ++alt;
            }
            if (alt != gt.size()) {
                hetCounts[name] += alt;
                //hetAlleleCount += alt;
            }
        }
    }

    //cout << hetAlleleCount << endl;
    for (vector<string>::iterator s = variantFile.sampleNames.begin(); s != variantFile.sampleNames.end(); ++s) {
        cout << (s == variantFile.sampleNames.begin() ? "" : "\t") << *s;
    }
    cout << endl;
    for (vector<string>::iterator s = variantFile.sampleNames.begin(); s != variantFile.sampleNames.end(); ++s) {
        cout << (s == variantFile.sampleNames.begin() ? "" : "\t") << hetCounts[*s];
    }
    cout << endl;

    return 0;

}
