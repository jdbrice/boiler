#ifndef CUT_SEQUENCE_H
#define CUT_SEQUENCE_H

#include <string>
#include <vector>
#include <limits>       // std::numeric_limits
using namespace std;

#include "XmlConfig.h"
#include "Logger.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"

namespace jdb {
	class CutSequence{

	public:


		class RangeCut{
		public:
			string name;	// name of variable to cut on in DataSource
			string title;	// for plotting / printing
			ConfigRange * cr;

			string toString(){
				return "< " + name + " : " + cr->toString() + " > ";
			}
		};

		string name;
		// The category of cuts - apply them to events or to tracks in an event etc.
		// for now only "event" and "track" are used
		string category;	
		string nodePath;
		vector<string> LeavesOfInterest; // LeavesOfInterest
		vector<RangeCut> rangeCuts;


		CutSequence( XmlConfig * cfg, string _nodePath ){

			Logger logger; // uses the global log level
			nodePath = _nodePath;
			if ( !cfg || !cfg->exists( nodePath ) ){
				logger.error(__FUNCTION__) << "Invalid nodePath " << endl;
				return;
			}

			// name is required 
			if ( !cfg->exists( nodePath + ":name" ) ){
				logger.error(__FUNCTION__) << "No name given in nodePath " << nodePath << endl;
				return;
			}

			category = cfg->getString( nodePath + ":category" );
			name = cfg->getString( nodePath + ":name" );
			LeavesOfInterest = cfg->getStringVector( nodePath + ".LeavesOfInterest" );
			
			logger.info( __FUNCTION__ ) << "<CutSequence name=" << name << " > " << endl;
			vector<string> children = cfg->childrenOf( nodePath, "RangeCut" );
			for ( int i = 0; i < children.size(); i++ ){
				
				if ( !cfg->exists( children[ i ] + ":name" )){
					logger.warn(__FUNCTION__) << "Skipping cut with no name" << endl;
					continue;
				}
				RangeCut rangeCut;
				rangeCut.cr = new ConfigRange( cfg, children[ i ], std::numeric_limits<double>::min(), std::numeric_limits<double>::max()   );
				rangeCut.name = cfg->getString( children[ i ] + ":name" );
				rangeCut.title = cfg->getString( children[ i ] + ":title", "" );
				rangeCuts.push_back( rangeCut );

				logger.info(__FUNCTION__) << "\t" << rangeCut.toString() << endl;
			}

		}

	};
}



#endif