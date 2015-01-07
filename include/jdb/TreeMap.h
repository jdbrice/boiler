#ifndef TREE_MAP_H
#define TREE_MAP_H

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
using namespace std;

#include "TChain.h"
#include "TBranch.h"

#include "Logger.h"

/**
 * TTree Data Types to support
 *
 * 	- C : a character string terminated by the 0 character
	- B : an 8 bit signed integer (Char_t)
	- b : an 8 bit unsigned integer (UChar_t)
	- S : a 16 bit signed integer (Short_t)
	- s : a 16 bit unsigned integer (UShort_t)
	- I : a 32 bit signed integer (Int_t)
	- i : a 32 bit unsigned integer (UInt_t)
	- F : a 32 bit floating point (Float_t)
	- D : a 64 bit floating point (Double_t)
	- L : a 64 bit signed integer (Long64_t)
	- l : a 64 bit unsigned integer (ULong64_t)
	- O : [the letter 'o', not a zero] a boolean (Bool_t)
 */


namespace jdb{

	class TreeMap
	{
	protected:


		Logger lg;

		TChain * chain;
		int nEntries;
		int nTrees;


		// list of branch names
		vector<string> names;

		// is the leaf usable
		map<string, bool> usable;

		// maps of basic data types
		map<string, Int_t> ints;
		map<string, UInt_t> uints;
		map<string, Char_t> chars;
		map<string, UChar_t> uchars;
		map<string, Short_t> shorts; 
		map<string, UShort_t> ushorts;
		map<string, Float_t> floats;
		map<string, Double_t> doubles;
		

		/*
		 *
		 * **Key** -   
		 * **Value** -
		 */
		map<string, Int_t*> intArrays;
		map<string, UInt_t*> uintArrays;
		map<string, Char_t*> charArrays;
		map<string, UChar_t*> ucharArrays;
		map<string, Short_t*> shortArrays;
		map<string, UShort_t*> ushortArrays;
		map<string, Float_t*> floatArrays;
		map<string, Double_t*> doubleArrays;

		/* 
		 * Map of the length variables
		 * **Key** is leaf name    
		 * **Value** is the length of the array. Either a fixed number or the name of another leaf
		 */
		map<string, string> lengthName;
		
		/*
		 *
		 * **Key** -   
		 * **Value** -
		 */
		map<string, Int_t> maxLength;

		/*
		 *
		 * **Key** -   
		 * **Value** -
		 */
		map<string, TBranch*> branches;

		/*
		 *
		 * **Key** -   
		 * **Value** -
		 */
		map<string, bool> isArray;

		/*
		 *
		 * **Key** -   
		 * **Value** -
		 */
		map<string, string> type;

	public:
		TreeMap( TChain * chain );
		~TreeMap();

		double get( string name, int index = 0 ); 
		double operator[]( string bName );

		int getEntries() { return nEntries; }
		void getEntry( int i ) { chain->GetEntry( i ); }

	protected:
		void mapTree();
		int findMax( string name );
		void findLengths();
		void setAddresses();


		
	};

}



#endif