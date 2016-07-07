#include "HistoAnalyzer.h"
#include "XmlString.h"
#include "XmlTFile.h"

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

		string ofn = "";
		if ( config.exists( nodePath + ".output.TFile:url" ) ){
			XmlString xstr( config );
			xstr.add( "jobIndex", jobIndex );
			ofn = xstr.format( config.getString( nodePath + ".output.TFile:url" ) );
		} else if ( config.exists( nodePath + ".output.data" ) ){
			// old school - no string interp
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
			DEBUG( classname(), "name = \"" << name << "\"");
			if ( extPos != std::string::npos )
				name = name.substr( 0, extPos - (ext.length() - 1) );
			DEBUG( classname(), "name = \"" << name << "\"");
			string full = outputPath + jobPrefix + name + _jobPostfix;
			ofn = full;
		}

		// Ensure that there is a valid output filename
		if ( ofn.size() < 4 ){
			WARN( classname(), "No valid output filename found, default = histoAnalyzer.root" );
			WARN( classname(), "Searching :" );
			WARN( classname(), nodePath + ".output.TFile:url" );
			WARN( classname(), nodePath + ".output.data" );
			ofn = "histoAnalyzer.root";
		}

		// create the book
		DEBUG( classname(), " Creating book : " << ofn );
		book = shared_ptr<HistoBook>(new HistoBook( ofn, config ) );

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
			XmlString xstr( config );
			xstr.add( "jobIndex", jobIndex );
			inf = xstr.format( config.getString( ( nodePath + ".input.TFile:url" ) ) );
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
