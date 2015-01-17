#ifndef CUT_VARIABLE_H
#define CUT_VARIABLE_H

#include "TreeMap.h"
#include "XmlConfig.h"
#include "Logger.h"
using namespace jdb;

#include "TF1.h"
#include <string>
using namespace std;

#include "TF1.h"

namespace jdb{


	class CutVariable{

	protected:
		TF1 * fun;
		TreeMap * tm;
		XmlConfig * cfg;
		string nodePath;
		Logger lg;
		vector<string> params;
	public:
		/* Creates a calculated variable from existing ones
			@tm TreeMap of data
			@cfg An XmlConfig containg data
			@nodePath Path to node containing a "<CutVariable />" node
		*/
		CutVariable( TreeMap * _tm, XmlConfig * _cfg, string _nodePath ){

			lg.info(__FUNCTION__) << "CutVariable( TreeMap, XmlConfig, " << _nodePath << " )" << endl;
			tm = _tm;
			cfg = _cfg;
			nodePath = _nodePath;

			string name = cfg->getString( nodePath + ":name" );
			string value = cfg->getString( nodePath + ":value" );

			fun = new TF1( "cvf", value.c_str(), -10, 10 );

			for ( int i = 0; i < 10; i ++ ){
				if ( cfg->exists( nodePath + ":p" + ts( i ) ) ){
					lg.info(__FUNCTION__ ) << "Mapping p" << i << " to " << cfg->getString( nodePath + ":p" + ts( i ) ) <<endl;
					params.push_back( cfg->getString( nodePath + ":p" + ts( i ) ) );
				}
			}
		}
		~CutVariable() {
			delete fun;
		};

		double eval() {

			for ( int i = 0; i < 10; i ++ ){
				if ( cfg->exists( nodePath + ":p" + ts( i ) ) ){
					double p = (*tm)[ params[ i ] ];
					fun->SetParameter( i, p );
				}
			}

			return fun->Eval( 0 );

		}

	protected:


	};


}

#endif