#ifndef TREE_ANALYZER_H
#define TREE_ANALYZER_H

//STL
#include <memory>
//ROOBARB
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Reporter.h"
#include "DataSource.h"

//ROOT
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
		Logger  	*logger = nullptr;
		//Project config
		XmlConfig 	*cfg = nullptr;
		//Node path in config file to the root for everything related to this job
		string 		nodePath;
		//Basepath for output of data and reports
		string 		outputPath;
		//Store project histograms and data
		HistoBook 	*book = nullptr;
		//For generating generic reports
		Reporter 	*reporter = nullptr;
		//The chain object which gets automatically set up
		TChain 		*chain = nullptr;
		// the maximum number of events to process. If not set all events will be processed 
		int nEventsToProcess;
		// Sets up a DataSource if there is a DataSource node
		DataSource * ds = nullptr;
		// Prefix for this job ( optional, used for parallel)
		string jobPrefix;
		// Skips the making phase
		// loads the output file of the same name as the input ( must exist )
		// and skips to the postEventLoop() method
		bool skipMake = false;

	// public methods
	public:
		/*Creates the TreeAnalyzer and sets up members
		 * @config 	XmlConfig with a node for the TreeAnalyzer config
		 * @np 		Path to TreeAnalyzer node in config 
		 * @fileList Optional : filelist containg files to load into chain. 
		 * 			 If not given then a directory should be specified in the config
		 * @jobPrefix Prefix for output files - useful for running in parallel.
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
		/* Called for each event
		 * Function decides whether an event should be kept or not
		 * Applies cuts to data
		 * @return 	True 	- Keep Event
		 *          False 	- Skip event     
		 */
		virtual bool keepEvent();

		/* Called before event loop
		 *	Used for setup + initialization before 
		 *	processing the entire loop
		 * 
		 */
		virtual void preEventLoop();

		/*
		 * Called after the event loop for cleanup, 
		 * report generation, etc."
		 */
		virtual void postEventLoop(){}

		/* Used to analyze events
		 * Analyzes a single event in the chain
		 */
		virtual void analyzeEvent(){}


		/* Used to analyze all events
		 * Events that do not pass the keepEvent are still processed
		 */
		virtual void analyzeEventBeforeCuts(){}

		/* Used to analyze all events
		 * Events that do not pass the keepEvent are still processed
		 */
		virtual void analyzeRejectedEvent(){}

		
	};

}


#endif