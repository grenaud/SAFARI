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
#include <set>

using namespace std;
using namespace vcflib;

int main(int argc, char** argv) {

    if (argc < 3) {
      cerr << "usage: " << argv[0] << " <vcf file> [FIELD1] [FIELD2] ..." << endl << endl
             << "To decrease file size remove INFO fields not listed on the command line" << endl;
      cerr << endl << "Type: transformation" << endl << endl;
        return 1;
    }

    string filename = argv[1];

    set<string> fieldsToKeep;
    for (int i = 2; i < argc; ++i) {
        fieldsToKeep.insert(argv[i]);
    }

    VariantCallFile variantFile;
    if (filename == "-") {
        variantFile.open(std::cin);
    } else {
        variantFile.open(filename);
    }

    if (!variantFile.is_open()) {
        return 1;
    }

    Variant var(variantFile);

    vector<string> fieldsToErase;
    vector<string> infoIds = variantFile.infoIds();
    for (vector<string>::iterator i = infoIds.begin(); i != infoIds.end(); ++i) {
        if (!fieldsToKeep.count(*i)) {
            fieldsToErase.push_back(*i);
            variantFile.removeInfoHeaderLine(*i);
        }
    }

    // write the header
    cout << variantFile.header << endl;

    // print the records, filtering is done via the setting of varA's output sample names
    while (variantFile.getNextVariant(var)) {
        for (map<string, vector<string> >::iterator i = var.info.begin(); i != var.info.end(); ++i) {
            if (!fieldsToKeep.count(i->first)) {
                i = var.info.erase(i);
            }
        }
        for (map<string, bool>::iterator i = var.infoFlags.begin(); i != var.infoFlags.end(); ++i) {
            if (!fieldsToKeep.count(i->first)) {
                i = var.infoFlags.erase(i);
            }
        }
        cout << var << endl;
    }

    return 0;

}
