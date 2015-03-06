#include "TreeAnalyzer.h"
#include "ChainLoader.h"

namespace jdb{

	TreeAnalyzer::TreeAnalyzer( XmlConfig * config, string np, string fileList, string jobPrefix ){
		
		//Set the Root Output Level
		//gErrorIgnoreLevel = kSysError;

		// Save config
		cfg = config;

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

		
		// make the Logger
		logger = LoggerConfig::makeLogger( cfg, np + "Logger" );
		Logger::setGlobalLogLevel( logger->getLogLevel() );
		logger->setClassSpace( "TreeAnalyzer" );
		logger->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;
		
		/**
		 * Check for valid input
		 */
		if ( !cfg->exists( np+"DataSource" ) && !cfg->exists( np+"input.dst" )){
			logger->error(__FUNCTION__) << "Invalid nodePath. Cannot create TreeAnalyzer. Make sure that there is an input specified and the node path is correct. " << endl;
			return;
		}

	    // create the book
	    logger->info(__FUNCTION__) << " Creating book " << config->getString( np + "output.data", "TreeAnalyzer" ) << endl;
	    book = new HistoBook( jobPrefix + config->getString( np + "output.data", "TreeAnalyzer" ), config, "", "" );
	    	    
	    if ( "" == jobPrefix && cfg->exists( np+"Reporter.output:url" ) ) {
		    reporter = new Reporter( cfg, np+"Reporter.", jobPrefix );
		    logger->info(__FUNCTION__) << "Creating report " << config->getString( np+"Reporter.output:url" ) << endl;
	    }

	    /**
	     * Sets up the input, should switch seemlessly between chain only 
	     * and a DataSource 
	     */
	    if ( cfg->exists( np+"DataSource" ) ){
	    	ds = new DataSource( cfg, np + "DataSource", fileList );
	    	chain = ds->getChain();
	    	logger->debug(__FUNCTION__) << "Chain " << chain << endl;
	    } else {
	    	chain = new TChain( cfg->getString( np+"input.dst:treeName" ).c_str() );
		    if ( "" == fileList ){
		    	logger->info(__FUNCTION__) << " Loading data from " << config->getString( np + "input.dst:url" ) << endl;
		    	ChainLoader::load( chain, cfg->getString( np+"input.dst:url" ), cfg->getInt( np+"input.dst:maxFiles", -1 ) );
		    } else {
		    	logger->info(__FUNCTION__) << " Parallel Job From " << fileList << ", prefix : " << jobPrefix << endl;
		    	ChainLoader::loadList( chain, fileList, cfg->getInt( np+"input.dst:maxFiles", -1 ) );
		    }	
	    }
	}

	TreeAnalyzer::~TreeAnalyzer(){
		if ( book )
			delete book;
		if ( reporter )
			delete reporter;

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