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
#include <fstream>
using namespace std;


// ROOT
#include "TChain.h"
#include "TBranchElement.h"

namespace jdb{


	/* Automatically maps tree branches and leaves
	 * DataSource automatically decomposes and addresses 
	 * TTree structures so you don't need to make a new
	 * class for every new TTree structure.
	 *
	 */
	class DataSource {

	protected:

		// Logger object
		Logger lg;
		// Path to <DataSource ...> ... </DataSource>
		string nodePath;
		// Xml Config
		XmlConfig * cfg;
		// The file list containg input files if given
		string fileList;
		// The Xml Config used to load the cache if found
		XmlConfig * cache;
		
		/**
		 * Tree Info
		 */
		
		// The name of the tree
		string treeName;
		// The Chain used to link trees
		TChain * chain;
		// The Number of Trees loaded
		int nTrees;
		// The number of total entries
		Long64_t nEntries;

		/*
		 * Data type mapping 
		 */

		// Number of elements stored by leaf ie array length
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
		// value = name of branch pointed to
		map< string, string> alias;
		//Evaluated leaves allow new leaves
		map< string, EvaluatedLeaf * > evalLeaf;

	public:

		// Ctor
		// Creates a DataSource from an XML node
		// @_cfg 		The XmlConfig 
		// @_nodePath 	Path to the <DataSource> node
		// @_fileList 	Optional filelist provided via sys arg - Default = ""
		// 
		// Creates a DataSource if the given node is valid
		DataSource( XmlConfig * _cfg, string _nodePath, string _fileList = "" );
		
		// Dtor
		~DataSource();

		// Get the chain used to store Trees
		// @return TChain * to chain used
		TChain * getChain() { return chain; }



		template <typename T>
		inline T get(string name, int i = 0 ){
			DEBUG( "Checking for valid pointer at " << name );
			if ( !data[ name ]){
				lg.debug(__FUNCTION__) << name << "[ " << i << " ] Invalid data" << endl;
				return numeric_limits<T>::quiet_NaN();	
			}
			DEBUG( "Checking for valid length" )
			if ( i >= leafLength[ name ] || i < 0 ){
				lg.debug(__FUNCTION__) << name << "[ " << i << " ] Out Of Bounds" << endl;
				return numeric_limits<T>::quiet_NaN();
			}
			DEBUG( "Casting to datatype and returning" )
			T * pData = (T*)data[ name ];
			return pData[ i ];
		}

		// Get a pointer to the given data
		// @name 	Name of variable to get
		// @return 	void * of data. You need to reference it with the proper type indirection.
		void * getPointer( string name ) {
			return data[ name ];
		}

		// General purpose data retrival
		// @name 	Name of variable
		// @i 		Index - for data arrays
		// Gets any type of data and casts it to a double. 
		// This method is required for Alias or Evaluated Leaves
		// @return 	double value of requested data
		double get( string name, int i = 0 );
		
		// () overload
		// @lName 	variable name - see get(...)
		// @index 	Index for data arrays - see get(...)
		// @return  The data cast to a double - see get(...)
		double operator() ( string lName, int index = 0) { return get( lName, index ); };

		// Gets the total number of entries in the chain
		// @return 	The number of entries in Chain
		Long64_t getEntries() { return nEntries; }

		// Loads an entry into memory
		// @return  	True - entry was loaded, False otherwise.
		bool getEntry( Long64_t i ) {
			if ( chain && chain->GetEntry( i ) > 0 )
				return true;
			return false; 
		}

		// Adds an aliased leaf to the DataSource
		// @aliasName 	Name of aliased leaf
		// @bName 		Name of leaf that the leaf points to
		void addAlias( string aliasName, string bName ){ alias[ aliasName ] = bName; }

		string getTreeName() const { return treeName; }


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

		// Determines if the Cache file for this tree exists
		bool cacheExists(){
			ifstream cacheFile( ".DataSource_" + treeName + ".xml" );
			if ( cacheFile.good() ){
				cacheFile.close();
				return true;
			}
			cacheFile.close();
			return false;
		}

		// Caches the tree metadata if needed
		void cacheTreeInfo();

		// Loads the tree metadata from the Cache
		void loadLengthsFromCache();

		// maps the sub branches and leaves of a branch
		void mapBranch( TBranchElement * br, string name );

		// Addresses all branches in the Tree 
		void addressBranches();
		// Addresses all leaves in the Tree
		void addressLeaves();
		// Maps out the branch and leaf attributes of the tree
		void mapTree();
		// Gets the length of a data leaf accross all trees
		// @name 	Leaf name
		// @return 	Number of elements in data leaf
		int chainLeafLength( string name );
		// Gets the length of a data leaf in a single Tree
		// @name 	Leaf name
		// @return 	Number of elements in data leaf
		int singleTreeLeafLength( string name );
		// Gets the memory footprint size for the given 
		// leaf based on its type and length
		// @name 	Leaf name
		// @return 	The memory block size
		int memSize( string name ){

			lg.debug(__FUNCTION__) << "SizeType " << leafType[ name ] << endl;
			lg.debug(__FUNCTION__) << "Length of Leaf : " << leafLength[ name ] << endl;
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