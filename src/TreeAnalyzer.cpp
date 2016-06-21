#include "TreeAnalyzer.h"
#include "ChainLoader.h"

namespace jdb{

	TreeAnalyzer::TreeAnalyzer(){
		DEBUG( classname(), "" );
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


	void TreeAnalyzer::init( XmlConfig &_config, string _nodePath, int _jobIndex ){
		DEBUG( classname(), "( " << _config.getFilename() << ", " << _nodePath << ", " << _jobIndex << " )" );

		TaskRunner::init( _config, _nodePath, _jobIndex );

		string jobPostfix = "_" + ts( _jobIndex ) + ".root";
		this->jobModifier = "job_" + ts( _jobIndex ) +"_";
 		if ( -1 == _jobIndex ){
 			jobPostfix = ".root";
 			this->jobModifier = "";
 		}

 		

		initDataSource( _jobIndex );
		if ( chain ){
			initHistoBook( jobPostfix );
			initReporter( jobPostfix );
			initLogger();	
		}
		nEventLoops = config.getInt( nodePath + ":nEventLoops", 1 );
		DEBUG( classname(), "Common Initialization" );
		initialize();
	}

	void TreeAnalyzer::sharedInit( XmlConfig &_config, string _nodePath, int _jobIndex, TChain * _chain ){
		DEBUG( classname(), "( " << _config.getFilename() << ", " << _nodePath << ", " << _jobIndex << ", chain=" << _chain << " )" );

		TaskRunner::init( _config, _nodePath, _jobIndex );

		string jobPostfix = "_" + ts( _jobIndex ) + ".root";
		this->jobModifier = "job_" + ts( _jobIndex ) +"_";
 		if ( -1 == _jobIndex ){
 			jobPostfix = ".root";
 			this->jobModifier = "";
 		}

 	
		this->chain = _chain;
		sharedTree = true;
		if ( chain ){
			initHistoBook( jobPostfix );
			initReporter( jobPostfix );
			initLogger();	
		}
		nEventLoops = config.getInt( nodePath + ":nEventLoops", 1 );
		DEBUG( classname(), "Common Initialization" );
		initialize();
	}

	void TreeAnalyzer::init( XmlConfig &_config, string _nodePath, string _fileList, string _jobPostfix ){
		DEBUG( classname(), "( " << _config.getFilename() << ", " << _nodePath << ", \"" << _fileList << "\", \"" << _jobPostfix << "\" )" );

		TaskRunner::init( _config, _nodePath, _fileList, _jobPostfix );

		this->jobModifier = _jobPostfix;

		initDataSource( _fileList );
		if (  chain ){
			initHistoBook( _jobPostfix );
			initReporter( _jobPostfix );
			initLogger();
		}
		nEventLoops = config.getInt( nodePath + ":nEventLoops", 1 );
		DEBUG( classname(), "Common Initialization" );
		initialize();
	}

	void TreeAnalyzer::initHistoBook( string _jobPostfix ) {

		string jobPrefix = "";  // will we ever use this?

		outputPath = config[ nodePath + ".output:path" ];
		// string outputDataPath = config[ config.join( nodePath, "output", "data" ) ];
		string name = config[ config.join( nodePath, "output", "data" ) ];

		// add in the inline output node
		if ( config.exists( nodePath + ".output:name" ) )
			name = config[ nodePath + ".output:name" ];


		// remove .root from the name if it is in there
		// the jobPostfix will add it back
		// Warning - this assumes that the '.root' is at the end of the string
		string ext = ".root";
		size_t extPos = name.find_last_of( ext );
		INFO( classname(), "name = \"" << name << "\"");
		if ( extPos != std::string::npos )
			name = name.substr( 0, extPos - (ext.length() - 1) );
		INFO( classname(), "name = \"" << name << "\"");


		string full = outputPath + jobPrefix + name + _jobPostfix;
		string fullCopy = outputPath + jobPrefix + name + "_copy" + _jobPostfix;
		
		// create the book
	    INFO( "TreeAnalyzer", " Creating book : " << full );
	    
	    // should we skip make?
	    skipMake = config.getBool( nodePath + ".SkipMake", false );
	    INFO( classname(), "Skip Make == " << skipMake );

	    if ( !skipMake )
		    book = new HistoBook( full, config );
		else {
			book = new HistoBook( fullCopy, config, full );
		}
	}


	void TreeAnalyzer::initReporter( string _jobPostfix ){

		INFO( classname(), "Creating Reporter" );
		string pRepOut = config.join( nodePath, "Reporter", "output:url" );
		string outputURL = config[ pRepOut ];

	   	// Default reporter
	    if ( ".root" == _jobPostfix && config.exists( pRepOut ) ) {
		    reporter = new Reporter( config, config.join( nodePath, "Reporter" ), "" ); // TODO: is reporter's path handeling broken?
		    INFO( classname(), "Creating report @" << outputURL );
	    } else{
	    	INFO( classname(), "No Reporter created, jobPostfix == " << _jobPostfix );
			reporter = nullptr;
	    }
	}


	void TreeAnalyzer::initDataSource( string _fileList ){
		INFO( classname(), "( fileList=" << _fileList << ")" );
		
		chain = new TChain( this->config.getString( nodePath + ".input.dst:treeName" ).c_str() );

		if ( config.exists( config.join( nodePath, "input", "dst" ) ) ) {
	    	
	    	

	    	// single job
		    if ( "" == _fileList ){
		    	INFO( classname(), " Loading data from " << config.getString( nodePath + ".input.dst:url" ) );

		    	ChainLoader::load( 	chain, 
		    						this->config.getString( nodePath + ".input.dst:url" ), 
		    						this->config.getInt( nodePath + ".input.dst:maxFiles", -1 ) );
		    } else { // or parallel
			    	// star-submit style condor files
		    	INFO( classname(), " Parallel Job From " << _fileList );

				ChainLoader::loadList( 	chain, 
										_fileList, 
										this->config.getInt( nodePath + ".input.dst:maxFiles", -1 ) );	
		    }


	    	
	    } else {
	    	chain = nullptr;
	    	ERROR( classname(), "No Chain was created" );
	    	return;
	    }


		// DataSource if requested
		if ( config.exists( nodePath + ".DataSource" ) && chain && chain->GetNtrees() >= 1 ){

			// TODO: Data source shouldn't need config pointer
			ds = new DataSource( config, config.join(nodePath, ".DataSource") , this->config.getString( nodePath + ".input.dst:treeName" ), chain );
			// chain = ds->getChain();

			DEBUG( classname(), "DataSrouce for chain : " << chain );

		}
	}

	void TreeAnalyzer::initDataSource( int _jobIndex ){
	    INFO( classname(), "( jobIndex=" << _jobIndex << ")" );


	    treeName = this->config.getString( nodePath + ".input.dst:treeName" );
	    chain = new TChain( this->config.getString( nodePath + ".input.dst:treeName" ).c_str() );

	    if ( config.exists( config.join( nodePath, "input", "dst" ) ) ) {
	    	
	    	// create the Chain object
	    	
		    string url = this->config.getString( nodePath + ".input.dst:url" );

		    // load from a file list!
		    if ( url.find( ".lis" ) != std::string::npos ){
		    	
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
	    	return;
	    }

		// make a DataSource if you want it
		if ( config.exists( nodePath + ".DataSource" ) && chain  ){
			ds = new DataSource( config, config.join(nodePath, ".DataSource"), treeName, chain );
			DEBUG( classname(), "DataSource for chain : " << chain );
		}


		INFO( classname(), "Data source initialized" );
	}


	void TreeAnalyzer::initLogger(){
		Logger::setGlobalLogLevel( config.getString( nodePath + ".Logger:globalLogLevel" ) );
	}


	void TreeAnalyzer::make(){
		DEBUG( classname(), "" );
		TaskTimer t;
		t.start();

		if ( !chain ){
			ERROR( classname(), "Invalid chain object" );
			return;
		}
		if ( chain->GetNtrees() <= 0 ){
			INFO( classname(), "Empty Chain" );
			return;
		}

		

		/**
		 * Run the pre event loop
		 */
		if ( !skipMake ){
			book->cd( );
			
			

			
			int totalNumberOfEvents = -1;
			showProgress = config.getBool( nodePath + ".EventLoop:progress", false );

			if ( showProgress ){
				totalNumberOfEvents = chain->GetEntries();
			}


			nEventsToProcess = config.getInt( nodePath + ".input.dst:nEvents", totalNumberOfEvents );

			// if neg then process all
			if ( nEventsToProcess < 0 || nEventsToProcess > totalNumberOfEvents )
				nEventsToProcess = totalNumberOfEvents;

			// check for datastore
			if ( ds )
				nEventsToProcess = config.getInt( nodePath + ".DataSource:maxEvents", nEventsToProcess );
			
			if ( showProgress ){
				INFO( classname(), "Loaded: " << nEventsToProcess << " events " );
			}
			
			INFO( classname(), "Starting Event Loops" );
			for ( iEventLoop = 0; iEventLoop < nEventLoops; iEventLoop++ ){
				INFO( classname(), "Starting Event Loop " << iEventLoop+1 << " of " << nEventLoops );
				/**
				 * Run the pre event-loop hook
				 */
				preEventLoop();

				/**
				 * Run the event-loop
				 */
				eventLoop();

				/**
				 * Run the post event-loop hook
				 */
				postEventLoop();
			} // end loop on iEventLoop
		} // skipMake
		else {
			postEventLoop();
		}
		INFO( classname(), "Make completed in " << t.elapsed() );
	} // eventLoop


	void TreeAnalyzer::eventLoop( ){
		DEBUG( classname(), "EventLoop" );

		TaskTimer t;
		t.start();

		TaskProgress tp( "Event Loop", nEventsToProcess );
		
		// loop over all events 
		Long64_t iEvent = 0;
		
		while (true){ // what could go wrong 
			Long64_t read = chain->GetEntry(iEvent);
			
			if ( read <= 0 || (nEventsToProcess >= 0 && iEvent >= nEventsToProcess) ){ // break if we read past end or hit limit
				break;
			}

			if ( showProgress )
				tp.showProgress( iEvent );

			analyzeEventBeforeCuts();

			if ( !keepEvent() ){
				analyzeRejectedEvent();
				iEvent++;
				continue;
			}

			analyzeEvent();
			iEvent++;
		} // Event Loop
		
		if ( false == showProgress ){
			INFO( classname(), "Event Loop Completed in " << t.elapsed() );
		}
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