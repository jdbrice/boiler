#ifndef HISTO_ANALYZER_H
#define HISTO_ANALYZER_H


/**
 * JDB 
 */
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Reporter.h"
using namespace jdb;

/**
 * ROOT
 */
#include "TFile.h"
#include "TError.h"

namespace jdb{

	/* Base class for creating histogram analysis projects
	 *
	 * Sublcassing HistoAnalyzer provides a good starting point for
	 * modules that take a root file containing histograms as input.
	 *
	 */
	class HistoAnalyzer
	{
	// protected properties
	protected:
		//The logging object for the job
		Logger 		*logger;
		// Xml Config
		XmlConfig 	*cfg;
		// Node Path
		string 		nodePath;
		// HistoBook used to organize ROOT objects
		HistoBook 	*book;
		// Reporter for generating pdf reports
		Reporter 	*reporter;
		// File containing input
		TFile 		*inFile;

	// public methods
	public:

		/* Creates a HistoAnalyzer from an Xml Config
		 * @nodePath The path to the node containing the HistoAnalyzer data
		 * 
		 */
		HistoAnalyzer( XmlConfig * config, string nodePath );

		/* Destructor
		 * Saves output and closes input
		 */
		~HistoAnalyzer();

		/* The Maker function
		 *
		 * Use this public function to start the job.
		 * Subclasses should override.
		 */
		virtual void make() {}
		
	};
}

#endif