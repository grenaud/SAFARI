/*
    vcflib C++ library for parsing and manipulating VCF files

    Copyright © 2010-2020 Erik Garrison
    Copyright © 2020      Pjotr Prins

    This software is published under the MIT License. See the LICENSE file.
*/

#include "Variant.h"

using namespace std;
using namespace vcflib;

int main(int argc, char** argv) {
if (argc == 2) {
  string h_flag = argv[1];
  if (h_flag == "-h" || h_flag == "--help") {
      cerr << R"(
List sample names

Usage: vcfsamplenames

Type: transformation

      )";
      exit(1);
    }
  }

    VariantCallFile variantFile;

    if (argc > 1) {
        string filename = argv[1];
        variantFile.open(filename);
    } else {
        variantFile.open(std::cin);
    }

    if (!variantFile.is_open()) {
        return 1;
    }

    for (vector<string>::iterator sample = variantFile.sampleNames.begin();
            sample != variantFile.sampleNames.end(); ++sample) {
        cout << *sample << endl;
    }

    return 0;

}
