#include "HistoAnalyzer.h"

namespace jdb{

	HistoAnalyzer::HistoAnalyzer(){	} 

	void HistoAnalyzer::init( XmlConfig &_config, string _nodePath, int _jobIndex ){
		TaskRunner::init( _config, _nodePath, _jobIndex );

		jobIndex = _jobIndex;
		string jobPostfix = "_" + ts( _jobIndex ) + ".root";
		this->jobModifier = "job_" + ts( _jobIndex ) +"_";

		if ( -1 == _jobIndex ){
 			jobPostfix = ".root";
 			this->jobModifier = "";
 		}
 		
		initHistoBook( jobPostfix );
		initReporter( jobPostfix );
		initRootFiles();

		initialize();
	}

	void HistoAnalyzer::init( XmlConfig &_config, string _nodePath, string _fileList, string _jobPostfix ){
		TaskRunner::init( _config, _nodePath, _fileList, _jobPostfix );

		this->jobModifier = _jobPostfix;
		initHistoBook( _jobPostfix );
		initReporter( _jobPostfix );
		initRootFiles();

		initialize();
	}

	void HistoAnalyzer::initHistoBook( string _jobPostfix ){
		initializeHistoBook( config, nodePath, _jobPostfix );
	}

	void HistoAnalyzer::initReporter( string _jobPostfix ){

		INFO( classname(), "Creating Reporter" );
		string pRepOut = config.join( nodePath, "Reporter", "output:url" );
		string outputURL = config[ pRepOut ];

	   	// Default reporter
	   	// dont make for parallel!
	    if ( ".root" == _jobPostfix && config.exists( pRepOut ) ) {
		    reporter = shared_ptr<Reporter>(
		    	new Reporter( 	config, 
		    					config.join( nodePath, "Reporter" ), 
		    					"" 
		    				) 
		    	); // TODO: is reporter's path handeling broken?

		    INFO( classname(), "Creating report @" << outputURL );
	    } else {
	    	INFO( classname(), "No Reporter created, jobPostfix == " << _jobPostfix );
			reporter = nullptr;
	    }
	}


	void HistoAnalyzer::initRootFiles(){
		vector<string> filenames;
		string inf = "";
		/***************************************/
		// nodePath.input.TFile:url
		if ( config.exists( nodePath + ".input.TFile:url" ) ){
			INFO( classname(), "Getting filename from TFile:url" );
			XmlString xstr;
			// xstr.add( "jobIndex", jobIndex );
			inf = xstr.format( config, config.getString( ( nodePath + ".input.TFile:url" ) ) );
		} else if ( config.exists( nodePath + ".input.data:url" ) ){
			// TODO : Add multiple file support!
			inf = config.getString( nodePath + ".input.data:url" );
		} else {
			WARN( classname(), "No input file given!" );
			WARN( classname(), "Search paths:" );
			WARN( classname(), nodePath  << ".input.TFile:url" );
			WARN( classname(), nodePath  << ".input.data:url" );
		}

		DEBUG( classname(), " Loading data from " << inf )
		inFile = new TFile( inf.c_str(), "READ" );
		if ( !inFile->IsOpen() ){
			ERROR( classname(), "Data File Could not be opened from : " + inf );
		}
	}

	HistoAnalyzer::~HistoAnalyzer(){
		
		// book->save();
	}

}
