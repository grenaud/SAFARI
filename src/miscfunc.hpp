#pragma once
#include "libgab.hpp"
#include "gzstream.hpp"

#define MIN2(a,b) (((a)<(b))?(a):(b))
#define MAX2(a,b) (((a)>(b))?(a):(b))
#define probably_true(x) __builtin_expect(!!(x), 1)

typedef struct {
    long double s[12];
 } substitutionRates;


//  model->obs
//  0  A->A
//  1  A->C
//  2  A->G
//  3  A->T
//  4  C->A
//  5  C->C
//  6  C->G
//  7  C->T
//  8  G->A
//  9  G->C
//  10 G->G
//  11 G->T
//  12 T->A
//  13 T->C
//  14 T->G
//  15 T->T

typedef struct {
    long double s[16];
} probSubstition;


typedef struct {
    long double p[4][4];
} diNucleotideProb;



static void readNucSubstitionRatesFreq(const string filename,vector<substitutionRates> & subVec){
    igzstream subFP;

    subFP.open(filename.c_str(), ios::in);

    //    unsigned int counterCont=0;
    if (subFP.good()){
	vector<string> fields;
	string line;

	//header
	if ( !getline (subFP,line)){
	    throw std::runtime_error("Unable to open file "+filename);
	}
	fields = allTokens(line,'\t');

	if(fields.size() != 12){
	    throw std::runtime_error("line from error profile does not have 12 fields "+line);
	}


	//probs
	while ( getline (subFP,line)){

	    fields = allTokens(line,'\t');

	    if(fields.size() != 12){
		throw std::runtime_error("line from error profile does not have 12 fields "+line);
	    }

	    substitutionRates tempFreq;


	    for(unsigned int k=0;k<12;k++){
		//for(unsigned int t=0;t<=2;t++){
		tempFreq.s[k]=destringify<long double>(fields[k]);
		//}
	    }



	    subVec.emplace_back( tempFreq );
	}
	subFP.close();
    }else{
	throw std::runtime_error("Unable to open file "+filename);
    }



}


