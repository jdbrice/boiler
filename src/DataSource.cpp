#include "DataSource.h"
#include "ChainLoader.h"
#include "Utils.h"
#include <stdio.h>


#include "TLeafElement.h"


namespace jdb{
	

	DataSource::DataSource( XmlConfig * _cfg, string _nodePath, string _fileList ){

		cfg = _cfg;
		nodePath = _nodePath;
		fileList = _fileList;
		cache = NULL;

		if ( cfg->exists( nodePath ) && cfg->exists( nodePath+":treeName" ) ){
			treeName = cfg->getString( nodePath + ":treeName", "" );
			if ( "" == treeName ){
				lg.warn(__FUNCTION__) << "Provide <DataSource treeName=\"theName\" /> " << endl;
			}

			chain = new TChain( treeName.c_str() );

			if ( "" == fileList ){
				if ( cfg->exists( nodePath + ":url" ) ){
					lg.info(__FUNCTION__) <<"Loading from url " << fileList << endl;
					ChainLoader::load( chain, cfg->getString( nodePath+":url" ), cfg->getInt( nodePath + ":maxFiles", -1 ) );
				} else if ( cfg->exists( nodePath + ":filelist" ) ){
					lg.info(__FUNCTION__) <<"Loading from filelist " << cfg->getString( nodePath+":filelist" ) << endl;
					ChainLoader::loadList( chain, cfg->getString( nodePath+":filelist" ), cfg->getInt( nodePath + ":maxFiles", -1 ) );
				}
				else 
					lg.warn(__FUNCTION__) << "Provide a url to folder containing data or a filelist <DataSource url=\"...\" (or) filelist=\"list.lis\" /> " << endl;
			} else {
				lg.info(__FUNCTION__) <<"Loading from filelist " << fileList << endl;
				ChainLoader::loadList( chain, fileList, cfg->getInt( nodePath + ":maxFiles", -1 ) );
			}

			/**
			 * Now create the data access structure
			 */
			if ( cacheExists() ){
				lg.info(__FUNCTION__) << "Loading cache from " << (".DataSource_" + treeName + ".xml") << endl;
				cache = new XmlConfig( ".DataSource_" + treeName + ".xml" );
			} else
				lg.info(__FUNCTION__) << "No cache found for " << treeName  << endl;
			mapTree();
			addressBranches();
			addressLeaves();

			initializeBranchStatus();
			makeAliases();
			makeEvaluatedLeaves();


			cacheTreeInfo();


		} else {
			lg.error(__FUNCTION__) << "Could not create DataSource : Invalid Config " << endl;
		}
	}

	DataSource::~DataSource(){

		lg.info(__FUNCTION__) << "Cleaning up" << endl;

		if (cache)
			delete cache;

		typedef map<string, void*>::iterator svp_it_type;
		for(svp_it_type iterator = data.begin(); iterator != data.end(); iterator++) {

			if ( iterator->second ){
				lg.debug(__FUNCTION__) << "Releasing " << iterator->first << endl;
				free( data[ iterator->first ] );
			}

		}

		typedef map<string, EvaluatedLeaf*>::iterator sel_it_type;
		for(sel_it_type it = evalLeaf.begin(); it != evalLeaf.end(); it++) {

			if ( it->second ){
				lg.debug(__FUNCTION__) << "Releasing " << it->first << endl;
				delete evalLeaf[ it->first ];
			}

		}

		lg.debug(__FUNCTION__) << "Memory Released" << endl;

	}


	void DataSource::mapTree(){

		lg.info( __FUNCTION__ ) << endl;

		if ( !chain ){
			lg.error(__FUNCTION__) << "Invalid Chain" << endl;
			return;
		}

		nEntries = chain->GetEntries();
		nTrees = chain->GetNtrees();
		lg.info(__FUNCTION__) << "Chain contains " << nTrees << " TTrees" << endl;


		/**
		 * Investigate the Branches
		 */
		TObjArray * brs = chain->GetListOfBranches();
		int l = brs->GetEntries();

		for ( int i = 0; i < l; i++ ){
			string name = ((TBranchElement*)brs->At( i ))->GetName();
			lg.debug() << "Branch " << name << endl;
			branchName.push_back( name );
		}


		
		/**
		 * Investigate the Leaves
		 */
		TObjArray * lfs = chain->GetListOfLeaves();
		l = lfs->GetEntries();

		for ( int i = 0; i < l; i++ ){

			// get info on this leaf
			string tName = ((TLeafElement*)lfs->At( i ))->GetTypeName();
			string name = ((TLeafElement*)lfs->At( i ))->GetName();
			string title = ((TLeafElement*)lfs->At( i ))->GetTitle();

			lg.debug() << tName << " " << name << endl;
			leafName.push_back( name );

			leafType[ name ] = tName; 
			
		}

		/**
		 * Now get the lengths of every leaf for the whole chain. 
		 * This must be done in a sep loop bc the leaf pointers are 
		 * not consistent accross TTrees
		 */
		if ( cache && cache->getInt( "TreeInfo:nTrees" ) >= nTrees ){
			loadLengthsFromCache();
		} else {
			TaskTimer tt;
			tt.start();
			for ( int i = 0; i < leafName.size(); i++ ){
				string name = leafName[ i ];
				leafLength[ name ] = chainLeafLength( name );
			}
			lg.info(__FUNCTION__) << "Finding Lengths : Elapsed " << tt.elapsedTime() << endl;
		}
		lg.info(__FUNCTION__) << "Complete" << endl;
	}

	void DataSource::initializeBranchStatus(){

		vector<string> children = cfg->childrenOf( nodePath );

		for ( int i = 0; i < children.size(); i++ ){

			// we are only interested in the branch status nodes
			if ( "BranchStatus" == cfg->tagName( children[ i ] )){
				string path = children[ i ];
				int status = cfg->getInt( path + ":status", 0 );
				string active = "Active";
				if ( !status )
						active = "Inactive";

				vector<string> bNames = cfg->getStringVector( path );

				for ( int ib = 0; ib < bNames.size(); ib++ ){
					lg.info(__FUNCTION__) << "Setting " << bNames[ ib ] << " to " << active << endl;
					chain->SetBranchStatus( bNames[ ib ].c_str(), status );
				} // loop on branch names
			} // require node to be a BranchStatus node
		} // loop on children of nodepPath
	}
	void DataSource::makeAliases(){
		vector<string> children = cfg->childrenOf( nodePath );

		for ( int i = 0; i < children.size(); i++ ){

			// we are only interested in the branch status nodes
			if ( "Alias" == cfg->tagName( children[ i ] )){
				string path = children[ i ];
				
				string aName = cfg->getString( path + ":name" );
				string pointsTo = cfg->getString( path + ":pointsTo" );

				addAlias(aName, pointsTo );

			} // require node to be a Alias node
		} // loop on children of nodepPath
	}

	void DataSource::makeEvaluatedLeaves(){
		vector<string> children = cfg->childrenOf( nodePath );

		for ( int i = 0; i < children.size(); i++ ){
			string path = children[ i ];
			// we are only interested in the branch status nodes
			if ( "EvaluatedLeaf" == cfg->tagName( path ) && cfg->exists( path+":name" ) && cfg->exists( path+":value" ) ){
				string name = cfg->getString( path+":name" );
				EvaluatedLeaf * evl = new EvaluatedLeaf( cfg, path );
				evalLeaf[ name ] = evl;

			} // require node to be a Alias node
		} // loop on children of nodepPath


	}

	int DataSource::singleTreeLeafLength( string name ){


		int dim = 0;

		TLeaf * l = chain->GetLeaf( name.c_str() );
		if ( !l ){
			lg.debug( __FUNCTION__ ) << name << " is not a leaf" << endl;
			return dim;
		}

		int nElem = 0;
		TLeaf* cl = l->GetLeafCounter( nElem );
		Int_t length = l->GetLen();
		if ( 0 >= length)
			length = 1;

		lg.debug( __FUNCTION__ ) << "\t" << l->GetTitle() << endl;
		lg.debug( __FUNCTION__ ) << "\tLength = " << length << endl;
		lg.debug( __FUNCTION__ ) << "\tMaximum = " << l->GetMaximum() << endl;
		lg.debug( __FUNCTION__ ) << "\tnElem = " << nElem << endl;
		if ( cl )
			lg.debug( __FUNCTION__ ) << "\tCount Leaf Maximum = " << cl->GetMaximum() << endl;

		if ( 1 == nElem && cl )	{ // Leaf like pt[ nTracks ] where nTracks is another leaf
			dim = cl->GetMaximum();
			if ( 0 == dim )
				dim = 1;
			lg.debug(__FUNCTION__) << "\t\tCase 1" << endl;
		} else if ( 1 <= nElem && !cl ){ // Leaf like channelHits[ 19 ] ( ie fixed array size )
			// could be single or multi-dim array based on the title
			// eg 	var[10][20] => nElem = 200
			// 		var[200] => nElem = 200
			// 		same memory footprint anyways so no worries really
			dim = nElem;
			lg.debug(__FUNCTION__) << "\t\tCase 2" << endl;
		} else {
			lg.debug( __FUNCTION__ ) << "\tCould not determine dimensions of " << name << endl;
			return -1;
		}

		lg.trace( __FUNCTION__ ) << "\t" << name << "@size ==> " << dim * length << endl;
		return dim * length;
	}


	int DataSource::chainLeafLength( string name ) {

		//The way root does the reporting it is necessary to 
		//find the max on each tree individually. The max for 
		//the entire chain is then the maximum value of each tree max 
		int max = 0;
		int nt = chain->GetNtrees();
		lg.debug(__FUNCTION__) << "Looping over " << nt << " trees" << endl;
		for ( int i = 0; i < nt; i ++ ){
			Long64_t first = chain->GetTreeOffset()[i];
			chain->LoadTree( first );
			lg.debug(__FUNCTION__) << "Getting leaf length for tree " << i << endl;
			int stl = singleTreeLeafLength( name );
			if ( stl > max) 
				max = stl;
			
		}
		// reload the first tree to put us back at the beg.
		chain->LoadTree( 0 );
		lg.trace( __FUNCTION__) << name << "@size = " << max << endl;

		return max;
	}

	void DataSource::addressBranches(){

		// Set the Tree into decomposed object mode
		chain->SetMakeClass(1);

		for ( int i = 0; i < branchName.size(); i ++ ){

			// get the data type from the "leaf" stored as "BranchName_"
			string bName = branchName[ i ];
			string lName = bName + "_";

			int ms = memSize( lName );
			data[bName] = malloc( ms );


			chain->SetBranchAddress( bName.c_str(), data[bName] );

			lg.debug( __FUNCTION__ ) << leafType[ lName ] << " " << bName << "(" << lName << ") addressed to " << ms << " bytes" << endl;

		}

	}

	void DataSource::addressLeaves() {

		for ( int i = 0; i < leafName.size(); i++ ){

			string name = leafName[ i ];
			
			//Skip "leaves" for top level branches since these are already taken care of
			TString addressName(name);
			if ( addressName.EndsWith( "_" ) )	
				continue;

			if ( "" == name )
				continue;

			int ms = memSize( name );
			lg.debug(__FUNCTION__) << "Allocating " << ms << " bytes for " << name << endl;

			if ( 0 == ms )
				continue;

			// Allocate the memory
			data[ name ] = malloc( ms );

			//Change fixed length arrays from "array" to "array[length]" since that is how they are addressed
			TLeaf * l = chain->GetLeaf( name.c_str() );
			if ( l ){
				Int_t len = l->GetLen();
				if ( len >= 2 ){
					string nName = name + "[" + ts(len) + "]";
					lg.debug(__FUNCTION__) << name << " --> " << nName << endl;
					name = nName;
				}	
			}
			
			// Address the leaves
			branches[ name ] = 0;
			chain->SetBranchAddress( name.c_str(), data[ name ], &branches[ name ] );
			lg.debug(__FUNCTION__) << "Addressed " << name << endl;
		}
	}

	double DataSource::get( string name, int i ){


		// Aliased leaves
		// Allows possible recursive alias (good or bad? )
		if ( alias.find( name ) != alias.end() ){ // name is an alias
			name = alias[ name ];
			return get( name, i );
		}
		// Evaluated leaves
		if ( evalLeaf.find( name ) != evalLeaf.end() ){
			return evalLeaf[ name ]->eval( this, i );
		}

		if ( i >= leafLength[ name ] || i < 0 ){
			lg.debug(__FUNCTION__) << name << "[ " << i << " ] Out Of Bounds" << endl;
			return numeric_limits<double>::quiet_NaN();
		}
		if ( !data[ name ]){
			lg.debug(__FUNCTION__) << name << "[ " << i << " ] Invalid data" << endl;
			return numeric_limits<double>::quiet_NaN();	
		}

		// we need to lookup the data pointer, re-cast it and look it up	
		if ( "Int_t" == leafType[ name ] ){
			Int_t * tData = (Int_t*)data[ name ];
			return tData[ i ];
		}else if ( "UInt_t" == leafType[ name ] ){
			UInt_t * tData = (UInt_t*)data[ name ];
			return tData[ i ];
		}else if ( "Short_t" == leafType[ name ] ){
			Short_t * tData = (Short_t*)data[ name ];
			return tData[ i ];
		}else if ( "UShort_t" == leafType[ name ] ){
			UShort_t * tData = (UShort_t*)data[ name ];
			return tData[ i ];
		}else if ( "Char_t" == leafType[ name ] ){
			Char_t * tData = (Char_t*)data[ name ];
			return tData[ i ];
		}else if ( "UChar_t" == leafType[ name ] ){
			UChar_t * tData = (UChar_t*)data[ name ];
			return tData[ i ];
		}else if ( "Float_t" == leafType[ name ] ){
			Float_t * tData = (Float_t*)data[ name ];
			return tData[ i ];
		}else if ( "Double_t" == leafType[ name ] ){
			Double_t * tData = (Double_t*)data[ name ];
			return tData[ i ];
		}	

		return numeric_limits<double>::quiet_NaN();

	}

	void DataSource::cacheTreeInfo(){


		if ( cacheExists() && ( (cache) && cache->getInt( "TreeInfo:nTrees" ) >= nTrees) ){
			lg.info(__FUNCTION__) << "No need to recache" << endl;
			// no need to recache
			return;
		}
		
		/*if ( cacheExists() ){
			
			delete cache;
			cache = NULL;
			remove( (".DataSource_" + treeName + ".xml").c_str() );
		}*/

		ofstream cacheFile( ".DataSource_" + treeName + ".xml" );

		cacheFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		cacheFile << "<root>" << endl;
		cacheFile << "\t<TreeInfo nTrees=\"" << nTrees << "\">" << endl;
		typedef map<string, int>::iterator si_it_type;
		for(si_it_type iterator = leafLength.begin(); iterator != leafLength.end(); iterator++) {

			if ( iterator->second ){
				cacheFile << "\t\t<LeafInfo name=\"" << iterator->first << "\" size=\"" << iterator->second <<"\" /> " << endl; 
			}
		}
		cacheFile << "\t</TreeInfo>" << endl;
		cacheFile << "</root>" << endl;

		cacheFile.close();

	}

	void DataSource::loadLengthsFromCache(){

		lg.info(__FUNCTION__)<< endl;
		vector<string> cli = cache->childrenOf( "TreeInfo" );
		for ( int i = 0; i < cli.size(); i++ ){

			string ln = cache->getString( cli[ i ] + ":name" );
			int ls = cache->getInt( cli[ i ] +":size" );
			leafLength[ ln ] = ls;
		}
	}


}