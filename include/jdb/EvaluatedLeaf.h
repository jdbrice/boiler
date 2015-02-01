#ifndef EVALUATED_LEAF_H
#define EVALUATED_LEAF_H

//#include "TreeMap.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "Utils.h"
using namespace jdb;

#include "TF1.h"
#include <string>
using namespace std;

#include "TF1.h"

class TreeMap;

namespace jdb{


	class EvaluatedLeaf{

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
			@nodePath Path to node containing a "<EvaluatedLeaf />" node
		*/
		EvaluatedLeaf( TreeMap * _tm, XmlConfig * _cfg, string _nodePath ){

			lg.info(__FUNCTION__) << "EvaluatedLeaf( TreeMap, XmlConfig, " << _nodePath << " )" << endl;
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
		~EvaluatedLeaf() {
			delete fun;
		};

		/**
		 * Evaluates the calculated value with the current values 
		 * in the tree mapp
		 * @return the evaluated value in double format
		 */
		double eval() {

			for ( int i = 0; i < 10; i ++ ){
				if ( cfg->exists( nodePath + ":p" + ts( i ) ) ){
					double p = tm->get( params[ i ] );
					fun->SetParameter( i, p );
				}
			}

			return fun->Eval( 0 );

		}

	protected:


	};


}

#endif