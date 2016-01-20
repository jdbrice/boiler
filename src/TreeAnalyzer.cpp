#include "TreeAnalyzer.h"
#include "ChainLoader.h"

namespace jdb{

	TreeAnalyzer::TreeAnalyzer( XmlConfig _config, string _nodePath, int _jobIndex )
		: TaskRunner( _config, _nodePath, _jobIndex ){
		
		//Set the Root Output Level
		//gErrorIgnoreLevel = kSysError;

		// Save inputs
		INFO( classname(), "Got config with nodePath = " << nodePath );
		init( _config, _nodePath, _jobIndex );
		
	}

	TreeAnalyzer::~TreeAnalyzer(){
		DEBUG( classname(), "" );
		if ( book )
			delete book;
		DEBUG( classname(), "Deleted book" );
		if ( reporter )
			delete reporter;
		DEBUG( classname(), "Deleted Reporter" );
	}


	void TreeAnalyzer::init( XmlConfig _config, string _nodePath, int _jobIndex ){
		DEBUG( classname(), "( " << _config.getFilename() << ", " << nodePath << ", " << _jobIndex << " )" );

		TaskRunner::init( _config, _nodePath, _jobIndex );

		this->config 	= _config;
		// makes sure it is in the right form
		// not ending in '.' or ':attribute' etc.
		this->nodePath = this->config.basePath( _nodePath );

		initHistoBook();
		initReporter();
		initDataSource( _jobIndex );

	}

	void TreeAnalyzer::initHistoBook() {

		outputPath = config[ config.join( nodePath, "output:path" ) ];
		string outputDataPath = config[ config.join( nodePath, "output", "data" ) ];

		// create the book
	    INFO( "TreeAnalyzer", " Creating book for datafile : " << outputDataPath );
	    
	    // should we skip make?
	    skipMake = config.getBool( nodePath + ".SkipMake", false );
	    INFO( classname(), "Skip Make == " << skipMake );

	    if ( !skipMake )
		    book = new HistoBook( outputPath + jobPrefix + outputDataPath, config, "", "" );
		else
			book = new HistoBook( outputPath + jobPrefix + "_copy_" + outputDataPath , config, outputPath + jobPrefix + outputDataPath, "" );
	}


	void TreeAnalyzer::initReporter(){
		INFO( classname(), "Creating Reporter" );
		string pRepOut = config.join( nodePath, "Reporter", "output:url" );
		string outputURL = config[ pRepOut ];

	   	// Default reporter
	    if ( "" == jobPrefix && config.exists( pRepOut ) ) {
		    reporter = new Reporter( config, config.join( nodePath, "Reporter" ), jobPrefix ); // TODO: is reporter's path handeling broken?
		    INFO( classname(), "Creating report @" << outputURL );
	    } else{
	    	INFO( classname(), "No Reporter created" );
			reporter = nullptr;
	    }
	}


	void TreeAnalyzer::initDataSource( int _jobIndex ){
	    /**
	     * Sets up the input, should switch seemlessly between chain only 
	     * and a DataSource 
	     */
	    INFO( classname(), "Creating Data Adapter" );

	    // DataSource auto-tree mapper mehh
	    if ( config.exists( nodePath + ".DataSource" ) ){
	    
	    	// TODO: Data source shouldn't need config pointer
	    	ds = new DataSource( &config, config.join(nodePath, ".DataSource") , fileList );
	    	chain = ds->getChain();

	    	DEBUG( classname(), "Datasrouce for chain : " << chain );
	    
	    } else if ( config.exists( config.join( nodePath, "input", "dst" ) ) ) {
	    	
	    	// create the Chain object
	    	chain = new TChain( this->config.getString( nodePath + ".input.dst:treeName" ).c_str() );
		    string url = this->config.getString( nodePath + ".input.dst:url" );

		    // load from a file list!
		    if ( url.find( '.lis' ) != std::string::npos ){
		    	
		    	INFO( classname(), " Loading data from listfile : " << url );
		    	// parallel job - load from list range
		    	if ( _jobIndex >= 0 ){

		    		int splitBy = config.getInt( nodePath + ".input.dst:splitBy", 50 );
		    		INFO( classname(), " Parallel Job index=" << _jobIndex << " splitBy=" << splitBy );
		    		ChainLoader::loadListRange( chain, url, _jobIndex, splitBy );

		    	} else { // load the entire list (up to maxFiles )

		    		int maxFiles = this->config.getInt( nodePath + ".input.dst:maxFiles", -1 );
		    		ChainLoader::loadList( 	chain, url, maxFiles );

		    	}

		    } else { // NOT a list file - a directory or file
				INFO( classname(), " Loading data from file or dir: " << url );
				int maxFiles = this->config.getInt( nodePath + ".input.dst:maxFiles", -1 );
				ChainLoader::load( 	chain, url, maxFiles ); 
		    }	
	    } else {
	    	chain = nullptr;
	    	ERROR( classname(), "No Chain was created" );
	    }
	}


	void TreeAnalyzer::make(){

		if ( !chain ){
			ERROR( classname(), "Invalid chain object" );
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
			nEventsToProcess = config.getInt( nodePath+"input.dst:nEvents", nEvents );
			
			// if neg then process all
			if ( nEventsToProcess < 0 )
				nEventsToProcess = nEvents;

			// check for datastore
			if ( ds )
				nEventsToProcess = config.getInt( nodePath+"DataSource:maxEvents", nEvents );
			if ( nEventsToProcess > nEvents )
				nEventsToProcess = nEvents;
			
			INFO( classname(), "Loaded: " << nEventsToProcess << " events " );
			
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
			INFO( classname(), "Completed in " << t.elapsed() );
		}
		/**
		 * Run the post event loop
		 */
		postEventLoop();
	}

	void TreeAnalyzer::preEventLoop(){

		// make some histos here
		if ( config.exists( nodePath+".histograms" ) )
			book->makeAll( nodePath+".histograms" );
	}

	/**
	 * Reads data from the chain - cuts events
	 * @return true or false
	 */
	bool TreeAnalyzer::keepEvent() {


		return true;
	}
}