#ifndef HISTO_ANALYZER_H
#define HISTO_ANALYZER_H


// ROOBARB
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Reporter.h"
// Interface
	#include "IObject.h"
	#include "IConfig.h"

using namespace jdb;

// ROOT
#include "TFile.h"
#include "TError.h"

// STL
#include <memory>
using namespace std;

namespace jdb{

	/* Base class for creating histogram analysis projects
	 *
	 * Sublcassing HistoAnalyzer provides a good starting point for
	 * modules that take a root file containing histograms as input.
	 *
	 */
	class HistoAnalyzer : public IConfig, public IObject
	{
	// protected properties
	protected:
		
		// HistoBook used to organize ROOT objects
		shared_ptr<HistoBook> 	book;
		// Reporter for generating pdf reports
		shared_ptr<Reporter> 	reporter;
		// File containing input
		TFile 		*inFile;

	// public methods
	public:

		virtual const char* classname() { return "HistoAnalyzer"; }

		/* Creates a HistoAnalyzer from an Xml Config
		 * @nodePath The path to the node containing the HistoAnalyzer data
		 * 
		 */
		HistoAnalyzer( XmlConfig * config, string nodePath, bool setup = true );

		/* Destructor
		 * Saves output and closes input
		 */
		~HistoAnalyzer();

		/* Sets up the analyzer
		 *
		 */
		void setup();

		/* The Maker function
		 *
		 * Use this public function to start the job.
		 * Subclasses should override.
		 */
		virtual void make() {}
		
	};
}

#endif