#include "HistoAnalyzer.h"

namespace jdb{

	HistoAnalyzer::HistoAnalyzer( XmlConfig * config, string np, bool _setup ){
		

		//Set the Root Output Level
		gErrorIgnoreLevel = kSysError;

		// Save Class Members for config
		cfg = config;
		nodePath = np;
		
		if ( _setup )
			setup();
	}

	void HistoAnalyzer::setup(){

		
	    // create the book
	    INFO( tag, " Creating book " << cfg->getString( nodePath + "output.data" ) )
	    book = shared_ptr<HistoBook>(new HistoBook( cfg->getString( nodePath + "output:path", "" ) + cfg->getString( nodePath + "output.data" ), cfg, "", "" ));


	    INFO( tag, " Creating report " << cfg->getString( nodePath + "output.report" ) );
	    if ( cfg->exists( nodePath + "Reporter.output:url" ) )
		    reporter = shared_ptr<Reporter>(new Reporter( cfg, nodePath + "Reporter." ));
		else 
			reporter = nullptr;


        INFO( "Looking for input @ input.data:url" )
	    INFO( " Loading data from " << cfg->getString( nodePath + "input.data:url" ) )
		inFile = new TFile( cfg->getString( nodePath + "input.data:url" ).c_str(), "READ" );
		INFO( "Input file : " << inFile )

		if ( !inFile->IsOpen()  )
			ERROR( "Data File Could not be opened" )
	}

	HistoAnalyzer::~HistoAnalyzer(){
		
	}

}
