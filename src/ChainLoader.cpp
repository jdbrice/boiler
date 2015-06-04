

#include "ChainLoader.h"
#include <fstream>

using namespace std;

namespace jdb{

	void ChainLoader::load( 
							TChain * chain, 	// the chain object to fill
							string ntdir, 	// the directory in which to look for ntuples
							int maxFiles 
							) {
		

		Logger * logger = new Logger( Logger::llGlobal, "ChainLoader" );

		if ( ntdir.find( ".root" ) != std::string::npos ){

			chain->AddFile( ntdir.c_str() );


		} else {
			logger->info(__FUNCTION__) << "Searching " << ntdir << " for ntuples" << endl;

			if ( ntdir[ ntdir.length()-1] != '/' ){
				logger->info(__FUNCTION__) << "Appending / to path" << endl;
				ntdir += "/";
			}

			if (maxFiles <= 0)
				maxFiles = 10000000;

			uint nFiles = 0;
			DIR *dir;
			struct dirent *ent;
			bool go = true;
			if ( (dir = opendir ( ntdir.c_str() ) ) != NULL) {
				
				while ( go && (ent = readdir ( dir) ) != NULL) {

			    	if ( strstr( ent->d_name, "root") ){
			    		
			    		if ( nFiles >= maxFiles ) {
			    			go = false;
			    			break;
			    		}

			    		char fn[ 1024 ];
			    		sprintf( fn, "%s%s", ntdir.c_str(), ent->d_name );
			    		chain->Add( fn );

			    		//cout << "[ChainLoader] Adding file " << fn << " to chain" << endl;
			    		nFiles++;

			    	}
			  	}
			  	
			  	logger->info( __FUNCTION__ ) << nFiles << " files loaded into chain" << endl;
			  	delete logger;

			  	closedir (dir);
			}
		} // if a directory was given

	}

	void ChainLoader::loadRange( 
							TChain * chain, 	// the chain object to fill
							string ntdir, 		// the directory in which to look for ntuples
							int firstFile,
							int nFiles
							) {
		

		Logger * logger = new Logger( Logger::llGlobal, "ChainLoader" );

		logger->info(__FUNCTION__) << "Searching " << ntdir << " for ntuples" << endl;

		if ( ntdir[ ntdir.length()-1] != '/' ){
			logger->info(__FUNCTION__) << "Appending / to path" << endl;
			ntdir += "/";
		}
        int iFile = 0;
		DIR *dir;
		struct dirent *ent;
		bool go = true;
		if ( (dir = opendir ( ntdir.c_str() ) ) != NULL) {
			
			while ( go && (ent = readdir ( dir) ) != NULL) {

		    	if ( strstr( ent->d_name, "root") ){
		    		
		    		if ( iFile >= firstFile && iFile - firstFile < nFiles ) {
		    			string fullName = ntdir + ent->d_name;	
		    			chain->Add( fullName.c_str() );
		    		}
		    		iFile++;
		    	} // is file a .root file

		  	} // while files in directory
		  	
		  	logger->info( __FUNCTION__ ) << iFile << " files loaded into chain" << endl;
		  	delete logger;

		  	closedir (dir);
		}

	}	



	void ChainLoader::loadList(  TChain * chain, string listFile, int maxFiles ){
		
		Logger * logger = new Logger( Logger::llDefault, "ChainLoader" );

		logger->info(__FUNCTION__) << "Opening " << listFile << " for list of ntuples" << endl;

		uint nFiles = 0;

		string line;
		ifstream fListFile( listFile.c_str());
		if ( fListFile.is_open() ){

			while ( getline( fListFile, line ) ){
				chain->Add( line.c_str() );
				nFiles++;

				if ( maxFiles >= 1 && nFiles >= maxFiles ){
					break;
				}

			}
			fListFile.close();
				

		} else {
			cout << " Could not open " << listFile << endl;
		}

		logger->info( __FUNCTION__ ) << nFiles << " files loaded into chain" << endl;

		delete logger;

	}
}
