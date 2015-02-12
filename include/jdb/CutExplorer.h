#ifndef CUT_EXPLORER_H
#define CUT_EXPLORER_H

#include "DataSource.h"
#include "XmlConfig.h"
#include "TreeAnalyzer.h"
#include "CutSequence.h"
using namespace jdb;

#include "TF1.h"

/*
	STL
 */
#include <string>
#include <map>
using namespace std;

namespace jdb{

	class CutExplorer : public TreeAnalyzer {
	protected:
		map<string, CutSequence*> cutSequences;
		// Maps the cut name to the cut category
		// ie name="eventCuts" -> category="event"
		map<string, string> cutCategory; 

		mapp<string, vector<double> > data;

	public:

		/* Creates CutExplorer object
		 * CutExplorer is a specialized TreeAnalyzer that uses a DataSource
		 * to analyze trees and build cut summaries.
		 * 
		 */
		CutExplorer( XmlConfig * cfg, string nodePath, string fileList = "", string jobPrefix = "" );
		~CutExplorer();

	protected:

		/* Called for each event
		 * Function decides whether an event should be kept or not
		 * Applies cuts to data
		 * @return 	True 	- Keep Event
		 *          False 	- Skip event  
		 *
		 * In CutExplorer this method applies the cuts and snapshots for event cuts 
		 */
		virtual bool keepEvent();


		
	};

}


#endif