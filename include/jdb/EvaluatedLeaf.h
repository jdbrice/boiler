#ifndef EVALUATED_LEAF_H
#define EVALUATED_LEAF_H

#include "XmlConfig.h"
#include "Logger.h"
#include "Utils.h"
using namespace jdb;

#include "TF1.h"
#include <string>
using namespace std;

#include "TF1.h"



namespace jdb{

	class DataSource;
	class EvaluatedLeaf{

	protected:
		TF1 * fun;
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
		EvaluatedLeaf( XmlConfig * _cfg, string _nodePath );
		~EvaluatedLeaf() {
			delete fun;
		};

		/**
		 * Evaluates the calculated value with the current values 
		 * in the tree mapp
		 * @return the evaluated value in double format
		 */
		double eval( DataSource * ds );

	protected:


	};


}

#endif