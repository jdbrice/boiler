#include "TreeAnalyzer.h"
#include "ChainLoader.h"

namespace jdb{

	TreeAnalyzer::TreeAnalyzer( XmlConfig * config, string np, string fileList, string _jobPrefix ){
		
		//Set the Root Output Level
		//gErrorIgnoreLevel = kSysError;

		// Save config
		cfg = config;
		jobPrefix = _jobPrefix;

		// We want our nodePath in the form of TreeAnalyzer.  
		// ie with trailing dot
		// make sure this is the case
		TString tsnp = np;
		nodePath = np;
		if ( tsnp.EndsWith( "." ) ){
			nodePath = np;	// this is the form we want
		} else if ( tsnp.EndsWith( ":" ) ){
			// remove the trailing : assuming it is good and add the .
			nodePath = np.substr( 0, np.length()-1) + ".";
		} else { // assume it is just the nodeName ie TreeAnalyzer with no trailing dot or colon
			// just add the trailing dot that we want
			nodePath = np + ".";
		}

		//get the output path
		outputPath = cfg->getString( nodePath + "output:path", "" );
		
		// make the Logger
		logger =LoggerConfig::makeLogger( cfg, nodePath + "Logger" );
		logger->setClassSpace( "TreeAnalyzer" );
		logger->info(__FUNCTION__) << "Got config with nodePath = " << nodePath << endl;
		
		/**
		 * Check for valid input
		 */
		if ( !cfg->exists( nodePath+"DataSource" ) && !cfg->exists( nodePath+"input.dst" )){
			logger->error(__FUNCTION__) << "Invalid nodePath. Cannot find" << nodePath+"DataSource" << " or " << nodePath+"input.dst" << " Make sure that there is an input specified and the node path is correct. " << endl;
			return;
		}

	    // create the book
	    logger->info(__FUNCTION__) << " Creating book " << config->getString( nodePath +  "output.data", "TreeAnalyzer" ) << endl;
	    skipMake = cfg->getBool( nodePath + "SkipMake", false );
	    INFO( "TreeAnalyzer", "Skip Make == " << skipMake );
	    if ( !skipMake )
		    book = new HistoBook( outputPath + jobPrefix + config->getString( nodePath +  "output.data", "TreeAnalyzer" ), config, "", "" );
		else
			book = new HistoBook( outputPath + jobPrefix + "_copy_" + config->getString( nodePath +  "output.data", "TreeAnalyzer" ) , config, outputPath + jobPrefix + config->getString( nodePath +  "output.data", "TreeAnalyzer" ), "" );
	    	    
	   	INFO( "TreeAnalyzer", "Creating Reporter" );
	   	// Default reporter
	    if ( "" == jobPrefix && cfg->exists( np+"Reporter.output:url" ) ) {
		    reporter = new Reporter( cfg, np+"Reporter.", jobPrefix );
		    logger->info(__FUNCTION__) << "Creating report " << config->getString( nodePath+"Reporter.output:url" ) << endl;
	    } else
			reporter = nullptr;

	    /**
	     * Sets up the input, should switch seemlessly between chain only 
	     * and a DataSource 
	     */
	    INFO( "TreeAnalyzer", "Creating DataSource" );
	    if ( cfg->exists( nodePath+"DataSource" ) ){
	    	ds = new DataSource( cfg, nodePath +  "DataSource", fileList );
	    	chain = ds->getChain();
	    	logger->debug(__FUNCTION__) << "Chain " << chain << endl;
	    } else {
	    	chain = new TChain( cfg->getString( nodePath+"input.dst:treeName" ).c_str() );
		    if ( "" == fileList ){
		    	logger->info(__FUNCTION__) << " Loading data from " << config->getString( nodePath +  "input.dst:url" ) << endl;
		    	ChainLoader::load( chain, cfg->getString( nodePath+"input.dst:url" ), cfg->getInt( nodePath+"input.dst:maxFiles", -1 ) );
		    } else {
		    	logger->info(__FUNCTION__) << " Parallel Job From " << fileList << ", prefix : " << jobPrefix << endl;
		    	ChainLoader::loadList( chain, fileList, cfg->getInt( nodePath+"input.dst:maxFiles", -1 ) );
		    }	
	    }
	}

	TreeAnalyzer::~TreeAnalyzer(){
		logger->info(__FUNCTION__) << endl;
		if ( book )
			delete book;
		logger->debug(__FUNCTION__) << "Deleted book" << endl;
		if ( reporter )
			delete reporter;
		logger->debug(__FUNCTION__) << "Deleted Reporter" << endl;

		logger->debug(__FUNCTION__) << "Deleting Logger" << endl;
		delete logger;
	}

	void TreeAnalyzer::make(){

		if ( !chain ){
			logger->error(__FUNCTION__) << " ERROR: Invalid chain " << endl;
			return;
		}

		/**
		 * Run the pre event loop
		 */
		if ( !skipMake ){
			book->cd( );
			preEventLoop();


			TaskTimer t;
			t.start();

			Int_t nEvents = (Int_t)chain->GetEntries();
			nEventsToProcess = cfg->getInt( nodePath+"input.dst:nEvents", nEvents );
			if ( ds )
				nEventsToProcess = cfg->getInt( nodePath+"DataSource:maxEvents", nEvents );
			if ( nEventsToProcess > nEvents )
				nEventsToProcess = nEvents;
			
			logger->info(__FUNCTION__) << "Loaded: " << nEventsToProcess << " events " << endl;
			
			TaskProgress tp( "Event Loop", nEventsToProcess );
			// loop over all events
			for(Int_t i=0; i<nEventsToProcess; i++) {
		    	chain->GetEntry(i);

		    	tp.showProgress( i );

		    	analyzeEventBeforeCuts();

		    	if ( !keepEvent() ){
		    		analyzeRejectedEvent();
		    		continue;
		    	}

		    	analyzeEvent();
		    	
		    	
			} // end loop on events
			logger->info(__FUNCTION__) << "Completed in " << t.elapsed() << endl;
		}
		/**
		 * Run the post event loop
		 */
		postEventLoop();
	}

	void TreeAnalyzer::preEventLoop(){

		// make some histos here
		if ( cfg->exists( nodePath+"histograms" ) )
			book->makeAll( nodePath+"histograms" );
	}

	/**
	 * Reads data from the chain - cuts events
	 * @return true or false
	 */
	bool TreeAnalyzer::keepEvent() {


		return true;
	}
}