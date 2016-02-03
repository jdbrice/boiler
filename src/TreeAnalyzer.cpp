#include "TreeAnalyzer.h"
#include "ChainLoader.h"

namespace jdb{

	// TreeAnalyzer::TreeAnalyzer( XmlConfig _config, string _nodePath, int _jobIndex )
	// 	: TaskRunner( _config, _nodePath, _jobIndex ){
		
	// 	//Set the Root Output Level
	// 	//gErrorIgnoreLevel = kSysError;

	// 	// Save inputs
	// 	INFO( classname(), "Got config with nodePath = " << nodePath );
	// 	init( _config, _nodePath, _jobIndex );
	// }

	// TreeAnalyzer::TreeAnalyzer( XmlConfig _config, string _nodePath, string _fileList, string _jobPostfix )
	// 	: TaskRunner( _config, _nodePath, _fileList, _jobPostfix ){
		
	// 	//Set the Root Output Level
	// 	//gErrorIgnoreLevel = kSysError;

	// 	// Save inputs
	// 	INFO( classname(), "Got config with nodePath = " << nodePath << " fileList = " << _fileList );
	// 	init( _config, _nodePath, _fileList, _jobPostfix );
	// }

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


	void TreeAnalyzer::init( XmlConfig _config, string _nodePath, int _jobIndex ){
		DEBUG( classname(), "( " << _config.getFilename() << ", " << nodePath << ", " << _jobIndex << " )" );

		TaskRunner::init( _config, _nodePath, _jobIndex );

		string jobPostfix = "_" + ts( _jobIndex ) + ".root";
 		if ( -1 == _jobIndex )
 			jobPostfix = ".root";

 		this->jobPostfix = jobPostfix;

		initDataSource( _jobIndex );
		if ( chain && chain->GetListOfFiles()->GetEntries() >= 1 ){
			initHistoBook( jobPostfix );
			initReporter( jobPostfix );
			initLogger();	
		}
		DEBUG( classname(), "Common Initialization" );
		initialize();
	}

	void TreeAnalyzer::init( XmlConfig _config, string _nodePath, string _fileList, string _jobPostfix ){
		DEBUG( classname(), "( " << _config.getFilename() << ", " << nodePath << ", \"" << _fileList << "\", \"" << _jobPostfix << "\" )" );

		TaskRunner::init( _config, _nodePath, _fileList, _jobPostfix );

		this->jobPostfix = _jobPostfix;

		initDataSource( _fileList );
		if (  chain && chain->GetListOfFiles()->GetEntries() >= 1 ){
			initHistoBook( _jobPostfix );
			initReporter( _jobPostfix );
			initLogger();
		}
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
		DEBUG( classname(), "nodePath = " << nodePath );
		// DataSource auto-tree mapper mehh
	    if ( config.exists( nodePath + ".DataSource" ) ){
	    
	    	// TODO: Data source shouldn't need config pointer
	    	ds = new DataSource( &config, config.join(nodePath, ".DataSource") , _fileList );
	    	chain = ds->getChain();

	    	DEBUG( classname(), "DataSrouce for chain : " << chain );
	    
	    } else if ( config.exists( config.join( nodePath, "input", "dst" ) ) ) {
	    	
	    	chain = new TChain( this->config.getString( nodePath + ".input.dst:treeName" ).c_str() );

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
	    }

	}

	void TreeAnalyzer::initDataSource( int _jobIndex ){
	    /**
	     * Sets up the input, should switch seemlessly between chain only 
	     * and a DataSource 
	     */
	    INFO( classname(), "Creating Data Adapter" );
	    
	    if ( config.exists( config.join( nodePath, "input", "dst" ) ) ) {
	    	
	    	// create the Chain object
	    	chain = new TChain( this->config.getString( nodePath + ".input.dst:treeName" ).c_str() );
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
	    } else if ( config.exists( nodePath + ".DataSource" ) ){

	    	// TODO: Data source shouldn't need config pointer
	    	ds = new DataSource( &config, config.join(nodePath, ".DataSource") );
	    	chain = ds->getChain();

	    	DEBUG( classname(), "DataSource for chain : " << chain );
	    
	    } else {
	    	chain = nullptr;
	    	ERROR( classname(), "No Chain was created" );
	    }
	}


	void TreeAnalyzer::initLogger(){
		Logger::setGlobalLogLevel( config.getString( nodePath + ".Logger:globalLogLevel" ) );
	}


	void TreeAnalyzer::make(){
		DEBUG( classname(), "" );
		if ( !chain ){
			ERROR( classname(), "Invalid chain object" );
			return;
		}
		if ( chain->GetListOfFiles()->GetEntries() <= 0 ){
			INFO( classname(), "Empty Chain" );
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
			nEventsToProcess = config.getInt( nodePath + ".input.dst:nEvents", nEvents );
			
			// if neg then process all
			if ( nEventsToProcess < 0 )
				nEventsToProcess = nEvents;

			// check for datastore
			if ( ds )
				nEventsToProcess = config.getInt( nodePath + ".DataSource:maxEvents", nEvents );
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