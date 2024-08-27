/*
    vcflib C++ library for parsing and manipulating VCF files

    Copyright © 2010-2020 Erik Garrison
    Copyright © 2020      Pjotr Prins

    This software is published under the MIT License. See the LICENSE file.
*/

#include "Variant.h"
#include "convert.h"

using namespace std;
using namespace vcflib;


double convertStrDbl(const string& s) {
    double r;
    convert(s, r);
    return r;
}

void printUsage()
{
    cerr << "usage: vcfflatten [options][file]" << endl
            << "  -h --help           display this help message and exit." << endl
            << "  -i --ignore-errors  do not flatten locus if 'AF' is not specified." << endl
            << endl
            << "Removes multi-allelic sites by picking the most common alternate.  Requires" << endl
            << "allele frequency specification 'AF' and use of 'G' and 'A' to specify the" << endl
            << "fields which vary according to the Allele or Genotype. VCF file may be" << endl
            << "specified on the command line or piped as stdin." << endl;
    cerr << endl << "Type: transformation" << endl << endl;
}

int main(int argc, char** argv) {

    int maxAlleles = 2;
    bool ignoreErrors = false;
    bool readFromStdin = false;
    string filename;

    VariantCallFile variantFile;

    if (argc == 1) {
        readFromStdin = true;
    } else if (argc == 2) {
        string arg1 = argv[1];
        if (arg1 == "--help" || arg1 == "-h") {
            printUsage();
            exit(1);
        } else if (arg1 == "--ignore-errors" || arg1 == "-i") {
            ignoreErrors = true;
            readFromStdin = true;
        } else {
            filename = argv[1];
        }
    } else if (argc == 3) {
        string arg1 = argv[1];
        if (arg1 == "--ignore-errors" || arg1 == "-i") {
            ignoreErrors = true;
            filename = argv[2];
        } else {
            printUsage();
            exit(1);
        }
    } else {
        printUsage();
        exit(1);
    }

    if (readFromStdin)
        variantFile.open(std::cin);
    else
        variantFile.open(filename);

    if (!variantFile.is_open()) {
        return 1;
    }

    cout << variantFile.header << endl;

    Variant var(variantFile);
    while (variantFile.getNextVariant(var)) {
        // count the number of alternates
        // if we have more than N, strip the lowest-frequency ones
        if (var.alleles.size() > maxAlleles) {
            if( !var.info["AF"].size() )
            {
                if(ignoreErrors)
                {
                    cout << var << endl;
                    continue;
                }
                cerr << "Error: cannot flatten line with more than 2 alleles and no 'AF' field." << endl;
                return 1;
            }

            multimap<double, string> alleleFrequencies;

            vector<string>& freqsstr = var.info["AF"];
            vector<double> freqs;
            freqs.resize(freqsstr.size());
            transform(freqsstr.begin(), freqsstr.end(), freqs.begin(), convertStrDbl);

            vector<double>::iterator f = freqs.begin();
            for (vector<string>::iterator a = var.alt.begin(); a != var.alt.end(); ++a, ++f) {
                alleleFrequencies.insert(pair<double, string>(*f, *a));
            }

            // pick the highest frequency alternate
            string bestalt = alleleFrequencies.rbegin()->second;
            // and get its index
            int bestaltIndex = var.getAltAlleleIndex(bestalt);
            int bestaltGenotypeIndex = bestaltIndex + 1; // per VCF spec

            // keep the RR, RA, and AA alleles for this alternate
            // generate the genotype index table for this variant
            map<pair<int, int>, int> genotypeIndexes = var.getGenotypeIndexesDiploid();

            // now get the genotype indexes we want to keep
            vector<int> alleleIndexes;
            alleleIndexes.push_back(0);
            alleleIndexes.push_back(bestaltGenotypeIndex);

            // add the reference allele index for generating genotype indexes
            int ploidy = 2;
            vector<vector<int> > genotypesToKeep = multichoose(ploidy, alleleIndexes);
            map<int, bool> genotypeIndexesToKeep;
            for (vector<vector<int> >::iterator k = genotypesToKeep.begin(); k != genotypesToKeep.end(); ++k) {
                pair<int, int> genotype = make_pair(k->front(), k->back()); // vectors are guaranteed to be diploid per multichoose
                genotypeIndexesToKeep[genotypeIndexes[genotype]] = true;
            }
            // we are diploid, so there should be exactly 3 genotypes
            assert(genotypeIndexesToKeep.size() == 3);

            // get the fields which have genotype order "G"
            // for all the infocounts
            // find the ones which are == GENOTYPE_NUMBER or ALLELE_NUMBER
            //     and fix em up
            for (map<string, int>::iterator c = variantFile.infoCounts.begin(); c != variantFile.infoCounts.end(); ++c) {
                int count = c->second;
                if (count == GENOTYPE_NUMBER) {
                    string key = c->first;
                    map<string, vector<string> >::iterator v = var.info.find(key);
                    if (v != var.info.end()) {
                        vector<string>& vals = v->second;
                        vector<string> tokeep;
                        int i = 0;
                        for (vector<string>::iterator g = vals.begin(); g != vals.end(); ++g, ++i) {
                            if (genotypeIndexesToKeep.find(i) != genotypeIndexesToKeep.end()) {
                                tokeep.push_back(*g);
                            }
                        }
                        vals = tokeep;
                    }
                } else if (count == ALLELE_NUMBER) {
                    string key = c->first;
                    map<string, vector<string> >::iterator v = var.info.find(key);
                    if (v != var.info.end()) {
                        vector<string>& vals = v->second;
                        vector<string> tokeep;
                        int i = 0;
                        for (vector<string>::iterator a = vals.begin(); a != vals.end(); ++a, ++i) {
                            if (i == bestaltIndex) {
                                tokeep.push_back(*a);
                            }
                        }
                        vals = tokeep;
                    }
                }
            }
            //
            // for all the formatcounts
            // find the ones which are == GENOTYPE_NUMBER or ALLELE_NUMBER
            //     for each sample, remove the new irrelevant values

            // for each sample
            //   remove info fields which now refer to nothing
            for (map<string, int>::iterator c = variantFile.formatCounts.begin(); c != variantFile.formatCounts.end(); ++c) {
                int count = c->second;
                if (count == GENOTYPE_NUMBER) {
                    string key = c->first;
                    for (map<string, map<string, vector<string> > >::iterator s = var.samples.begin(); s != var.samples.end(); ++s) {
                        map<string, vector<string> >& sample = s->second;
                        map<string, vector<string> >::iterator v = sample.find(key);
                        if (v != sample.end()) {
                            vector<string>& vals = v->second;
                            vector<string> tokeep;
                            int i = 0;
                            for (vector<string>::iterator g = vals.begin(); g != vals.end(); ++g, ++i) {
                                if (genotypeIndexesToKeep.find(i) != genotypeIndexesToKeep.end()) {
                                    tokeep.push_back(*g);
                                }
                            }
                            vals = tokeep;
                        }
                    }
                } else if (count == ALLELE_NUMBER) {
                    string key = c->first;
                    for (map<string, map<string, vector<string> > >::iterator s = var.samples.begin(); s != var.samples.end(); ++s) {
                        map<string, vector<string> >& sample = s->second;
                        map<string, vector<string> >::iterator v = sample.find(key);
                        if (v != sample.end()) {
                            vector<string>& vals = v->second;
                            vector<string> tokeep;
                            int i = 0;
                            for (vector<string>::iterator a = vals.begin(); a != vals.end(); ++a, ++i) {
                                if (i == bestaltIndex) {
                                    tokeep.push_back(*a);
                                }
                            }
                            vals = tokeep;
                        }
                    }
                }
            }

            var.alt.clear();
            var.alt.push_back(bestalt);

        }
        cout << var << endl;
    }

    return 0;

}
