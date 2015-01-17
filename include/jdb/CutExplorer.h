#ifndef CUT_EXPLORER_H
#define CUT_EXPLORER_H

#include "TreeMap.h"
#include "XmlConfig.h"
using namespace jdb;

#include "TF1.h"
#include <string>
using namespace std;

namespace jdb{

	class CutExplorer
	{

	public:

		CutExplorer( TreeMap * tm, XmlConfig * cfg );
		~CutExplorer(){}

	protected:
		void makeVariables( string nodePath );



		
	};

}


#endif