#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

/**
 * JDB
 */
#include "XmlConfig.h"
#include "Logger.h"
#include "EvaluatedLeaf.h"

/**
 * STL
 */
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>
using namespace std;

/**
 * ROOT
 */
#include "TChain.h"

namespace jdb{


	/*
	 * A Data source backed by a TChain of root files.
	 *
	 * Automatically maps tree branches and leaves
	 * 
	 */

	class DataSource {

	protected:

		// Logger object
		Logger lg;
		string nodePath;
		XmlConfig * cfg;
		
		/**
		 * Tree Info
		 */
		string treeName;
		TChain * chain;
		int nTrees;
		Long64_t nEntries;

		/**
		 * Data type mapping 
		 */
		// # of elements stored by leaf ie array length
		std::map< string, int > leafLength;
		// The leaf data type in string form 
		map<string, string > leafType;
		// A list of leaf names - fully qaulified Branch.Branch.Leaf
		vector<string> leafName;
		// A list of branches
		vector<string> branchName;
		// The data allocated for each leaf
		map< string, void * > data;
		// The pointer to each leaf's branch
		map< string, TBranch*> branches;
		//Aliases - allows a branch to be accessed from another name
		// key = alias name
		// value = name of branch pointe to
		map< string, string> alias;
		//Evaluated leaves allow new leaves
		map< string, EvaluatedLeaf * > evalLeaf;

	public:

		DataSource( XmlConfig * _cfg, string _nodePath );
		~DataSource();

		TChain * getChain() { return chain; }

		double get( string name, int i = 0 );
		double operator() ( string lName, int index = 0) { return get( lName, index ); };

		Long64_t getEntries() { return nEntries; }
		bool getEntry( Long64_t i ) {
			if ( chain && chain->GetEntry( i ) > 0 )
				return true;
			return false; 
		}

		void addAlias( string alaisName, string bName ){ alias[ alaisName ] = bName; }


	protected:

		/*
		 * Generates the inital branch status from the config file
		 */
		void initializeBranchStatus();
		/*
		 * Makes the aliases defined in the config file
		 */
		void makeAliases();
		/*
		 * Makes evaluated leafs from existing ones from teh config file
		 */
		void makeEvaluatedLeaves();


		void addressBranches();
		void addressLeaves();
		void mapTree();
		int chainLeafLength( string name );
		int singleTreeLeafLength( string name );
		int memSize( string name ){

			if ( "Int_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "UInt_t" == leafType[ name ] )
				return sizeof(UInt_t) * leafLength[ name ];
			else if ( "Short_t" == leafType[ name ] )
				return sizeof(Short_t) * leafLength[ name ];
			else if ( "UShort_t" == leafType[ name ] )
				return sizeof(UShort_t) * leafLength[ name ];
			else if ( "Char_t" == leafType[ name ] )
				return sizeof(Char_t) * leafLength[ name ];
			else if ( "UChar_t" == leafType[ name ] )
				return sizeof(UChar_t) * leafLength[ name ];
			else if ( "Float_t" == leafType[ name ] )
				return sizeof(Float_t) * leafLength[ name ];
			else if ( "Double_t" == leafType[ name ] )
				return sizeof(Double_t) * leafLength[ name ];

			return 0;
		}

	};


}

#endif