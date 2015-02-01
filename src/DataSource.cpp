#include "DataSource.h"
#include "ChainLoader.h"


#include "TLeafElement.h"

namespace jdb{
	

	DataSource::DataSource( XmlConfig * _cfg, string _nodePath ){

		cfg = _cfg;
		nodePath = _nodePath;


		if ( cfg->exists( nodePath ) && cfg->exists( nodePath+":treeName" ) ){
			treeName = cfg->getString( nodePath + ":treeName", "" );
			if ( "" == treeName ){
				lg.warn(__FUNCTION__) << "Provide <DataSource treeName=\"theName\" /> " << endl;
			}

			chain = new TChain( treeName.c_str() );
			if ( cfg->exists( nodePath + ":url" ) )
				ChainLoader::load( chain, cfg->getString( nodePath+":url" ), cfg->getInt( nodePath + ":maxFiles", -1 ) );
			else if ( cfg->exists( nodePath + ":filelist" ) )
				ChainLoader::load( chain, cfg->getString( nodePath+":url" ), cfg->getInt( nodePath + ":maxFiles", -1 ) );
			else 
				lg.warn(__FUNCTION__) << "Provide a url to folder containing data or a filelist <DataSource url=\"...\" (or) filelist=\"list.lis\" /> " << endl;

			std::auto_ptr<int> aip( new int );

			lg.info(__FUNCTION__) << "Just made an auto pointer" << endl;

			/**
			 * Now create the data access structure
			 */
			 mapTree();




		} else {
			lg.error(__FUNCTION__) << "Could not create DataSource : Invalid Config " << endl;
		}


	}

	DataSource::~DataSource(){

		lg.info(__FUNCTION__) << "Cleaning up" << endl;

		typedef map<string, void*>::iterator svp_it_type;
		for(svp_it_type iterator = data.begin(); iterator != data.end(); iterator++) {

			if ( iterator->second ){
				lg.debug(__FUNCTION__) << "Releasing " << iterator->first << endl;
				delete[] (data[ iterator->first ]);
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
		for ( int i = 0; i < leafName.size(); i++ ){
			string name = leafName[ i ];
			leafLength[ name ] = chainLeafLength( name );

			lg.info("", false ) << endl;
		}

		lg.info(__FUNCTION__) << "Complete" << endl;
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

		lg.debug( __FUNCTION__ ) << "\tLength = " << l->GetLen() << endl;
		lg.debug( __FUNCTION__ ) << "\tMaximum = " << l->GetMaximum() << endl;
		lg.debug( __FUNCTION__ ) << "\tnElem = " << nElem << endl;
		if ( cl )
			lg.debug( __FUNCTION__ ) << "\tCount Leaf Maximum = " << cl->GetMaximum() << endl;

		if ( 1 == nElem && cl )	{ // Leaf like pt[ nTracks ] where nTracks is another leaf
			dim = cl->GetMaximum();
			if ( 0 == dim )
				dim = 1;
		} else if ( 1 <= nElem && !cl ){ // Leaf like channelHits[ 19 ] ( ie fixed array size )
			// could be single or multi-dim array based on the title
			// eg 	var[10][20] => nElem = 200
			// 		var[200] => nElem = 200
			// 		same memory footprint anyways so no worries really
			dim = nElem;
		} else {
			lg.debug( __FUNCTION__ ) << "\tCould not determine dimensions of " << name << endl;
			return -1;
		}

		lg.trace( __FUNCTION__ ) << "\t" << name << "@size ==> " << dim << endl;
		return dim;
	}


	int DataSource::chainLeafLength( string name ) {

		//The way root does the reporting it is necessary to 
		//find the max on each tree individually. The max for 
		//the entire chain is then the maximum value of each tree max 
		int max = -1;
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
		lg.debug( __FUNCTION__) << name << "@size = " << max << endl;

		return 0;
	}

	void DataSource::addressLeaves() {

		for ( int i = 0; i < leafName.size(); i++ ){

			string name = leafName[ i ];
			if ( "" == name )
				continue;

			data[ name ] = malloc( memSize( name ) );

		}

	}

}