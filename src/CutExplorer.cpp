#include "CutExplorer.h"
#include "Utils.h"


namespace jdb{

	CutExplorer::CutExplorer( XmlConfig * _cfg, string nodePath, string fileList, string jobPrefix ) : 
							TreeAnalyzer( _cfg, nodePath, fileList, jobPrefix ) {

		logger->setClassSpace( "CutExplorer" );
		logger->info(__FUNCTION__) << endl;


		/**
		 * Make the CutSequences
		 */
		vector<string> cuts = cfg->childrenOf( nodePath, "CutSequence" );
		logger->info(__FUNCTION__) << cuts.size() << endl;
		for ( int i = 0; i < cuts.size(); i++ ){
			if ( cfg->exists( cuts[ i ] + ":name" ) ){
				string cName = cfg->getString( cuts[ i ] + ":name" );
				cutSequences[ cName ] = new CutSequence( cfg, cuts[ i ] );
			}
			
		}
	}

	CutExplorer::~CutExplorer(){

		typedef map<string, CutSequence*>::iterator scsp_it_type;
		for(scsp_it_type iterator = cutSequences.begin(); iterator != cutSequences.end(); iterator++) {

			if ( iterator->second ){
				logger->debug(__FUNCTION__) << "Releasing Cut Sequence " << iterator->first << endl;
				free( cutSequences[ iterator->first ] );
			}

		}
	}

	bool CutExplorer::keepEvent(){

		// loop through cutSequences
		// work with any where category="event"
		typedef map<string, CutSequence*>::iterator scsp_it_type;
		for(scsp_it_type iCut = cutSequences.begin(); iCut != cutSequences.end(); iCut++) {
			string cName = iCut->first;
			CutSequence * cs = iCut->second;

			// Snapshot - No Cuts
			snapshot( cName, "none" );

			/**
			 * Loop through and snapshot each individual cut
			 */
			/*if ( "event" == cs->category ){
				logger->debug( __FUNCTION__ ) << "Processing " << cName << endl;

				for ( int iRC = 0; iRC < cs->rangeCuts.size(); iRC++ ){
					logger->debug( __FUNCTION__) << "Applying " << cs->rangeCuts[ iRC ].toString() << endl;

					apply( ... );
					snapshot( cName, name );
				}
			}*/
		
			/**
			 * Loop through and snapshot cuts in a sequence
			 * eg A-B, A-C, A-D etc.
			 */
			if ( cs->rangeCuts.size() >= 3 ){
				if ( "event" ==  cs->category ){
					for ( int iRC = 3; iRC <= cs->rangeCuts.size(); iRC++ ){
						bool pass = true;
						for ( int iAC = 0; iAC < iRC; iAC++ ){
							if ( !passes( ... ) ){
								pass = false;
							}
						}
						// apply in squence
						snapshot( cName, name1, name2 ); 	
					}
				}
			}
			
	


			// Snapshot - All cuts applied
		 	snapshot( cName, "all" );
		}

	}


}