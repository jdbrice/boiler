#ifndef DATA_SOURCE
#define DATA_SOURCE

/**
 * JDB
 */
#include "XmlConfig.h"
#include "Logger.h"

/**
 * STL
 */
#include <string>
#include <map>
#include <vector>
#include <memory>

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
		int nEntries;

		/**
		 * Data type mapping 
		 */
		// # of elements stored by leaf ie array length
		std::map< string, int > leafLength;
		// The leaf data type in string form 
		map<string, string > leafType;
		// A list of leaf names - fully qaulified Branch.Branch.Leaf
		vector<string> leafName;
		// The data allocated for each leaf
		map< string, void * > data;


	public:

		DataSource( XmlConfig * _cfg, string _nodePath );
		~DataSource();

		TChain * getChain() { return chain; }

	protected:

		void addressLeaves();
		void mapTree();
		int chainLeafLength( string name );
		int singleTreeLeafLength( string name );
		int memSize( string name ){
			if ( "Int_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "UInt_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "Short_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "UShort_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "Char_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "UChar_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "Float_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
			else if ( "Double_t" == leafType[ name ] )
				return sizeof(Int_t) * leafLength[ name ];
		}

	};


}

#endif