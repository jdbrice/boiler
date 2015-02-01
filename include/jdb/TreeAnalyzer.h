#ifndef TREE_ANALYZER_H
#define TREE_ANALYZER_H


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

/**
 * ROOT
 */
#include "TChain.h"
#include "TError.h"

namespace jdb{


	/* Boiler for analyzing data distributed accross TTrees in many files
	 *
	 * Automatically loads the TTrees from each file into a TChain.
	 * Formalizes the structure for the config file, automatically loading
	 * the config, logger, reporter, and histobook objects. These become available to any subclass
	 */
	class TreeAnalyzer
	{
	// protected properties
	protected:
		
		//The logging object for the job
		Logger 		*logger;
		//Project config
		XmlConfig 	*cfg;
		//Node path in config file to the root for everything related to this job
		string 		nodePath;
		//Store project histograms and data
		HistoBook 	*book;
		//For generating generic reports
		Reporter 	*reporter;
		//The chain object which gets automatically set up
		TChain 		*chain;
		// the maximum number of events to process. If not set all events will be processed 
		int nEventsToProcess;

	// public methods
	public:
		/*Creates the TreeAnalyzer and sets up members
		 *
		 * Creates the Reporter, HistoBook and initializes the TChain
		 * If not filelist is given then data is loaded from the directory specified in the
		 * config file. Otherwise the files in the filelist are used.
		 *
		 * Optionally a jobPrefix can be used to allow for parallel execution such that 
		 * each job writes out to different files.
		 */
		TreeAnalyzer( XmlConfig * config, string np, string fileList ="", string jobPrefix ="");
		~TreeAnalyzer();

		/*The maker function for publicly starting the job
		 *
		 * This is the main entry point of any job
		 */
		
		virtual void make();

	protected:
		/*jdoc{
		"name" : "virtual bool keepEvent()",
		"params" : [ ],
		"paramDesc" : [ ],
		"returns" : [ "true : Event should be processed", "false : Event should be skipped" ],
		"desc" : "Central function for event cuts"
		}*/
		virtual bool keepEvent();

		/*jdoc{
		"name" : "virtual void preEventLoop()",
		"params" : [ ],
		"paramDesc" : [ ],
		"returns" : [  ],
		"desc" : "Called before the event loop for intialization"
		}*/
		virtual void preEventLoop();

		/*jdoc{
		"name" : "virtual void postEventLoop()",
		"params" : [ ],
		"paramDesc" : [ ],
		"returns" : [  ],
		"desc" : "Called after the event loop for cleanup, report generation, etc."
		}*/
		virtual void postEventLoop(){}

		/*jdoc{
		"name" : "virtual void analyzeEvent()",
		"params" : [ ],
		"paramDesc" : [ ],
		"returns" : [  ],
		"desc" : "Analyzes a single event in the chain"
		}*/
		virtual void analyzeEvent(){}


		virtual void analyzeEventBeforeCuts(){}
		
	};

}


#endif