#ifndef TREE_ANALYZER_H
#define TREE_ANALYZER_H

//STL
#include <memory>
//ROOBARB
#include "Utils.h"
// #include "XmlConfig.h"
// #include "Logger.h"
// #include "LoggerConfig.h"
#include "HistoBook.h"
// #include "ConfigRange.h"
// #include "ConfigPoint.h"
#include "Reporter.h"
#include "DataSource.h"
#include "TaskRunner.h"
#include "Utils.h"

	// Interfaces
	#include "IConfig.h"
	#include "IObject.h"

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
	class TreeAnalyzer : public TaskRunner
	{
	// protected properties
	protected:
		
		
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

		// Skips the making phase
		// loads the output file of the same name as the input ( must exist )
		// and skips to the postEventLoop() method
		bool skipMake = false;
		

		int jobIndex;
		string jobModifier;


	// public methods
	public:
		virtual const char* classname() const { return "TreeAnalyzer";}

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
		TreeAnalyzer();
		~TreeAnalyzer();

		virtual void init( XmlConfig _config, string _nodePath="", int _jobIndex = -1);
		virtual void init( XmlConfig _config, string _nodePath="", string _fileList = "", string _jobPostfix = "" );
		virtual void initialize() { DEBUG( classname(), "TreeAnalyzer" ); };
		

		
	protected:
		void initHistoBook( string _jobPostfix );
		void initReporter( string _jobPostfix );
		void initDataSource( int _jobIndex );
		void initDataSource( string _fileList );
		void initLogger(  );


		/*The maker function for publicly starting the job
		 *
		 */
		virtual void make();



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