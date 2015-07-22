#include "HistoAnalyzer.h"

namespace jdb{

	HistoAnalyzer::HistoAnalyzer( XmlConfig * config, string np ){
		

		//Set the Root Output Level
		gErrorIgnoreLevel = kSysError;

		// Save Class Members for config
		cfg = config;
		nodePath = np;
		
		// make the Logger
		logger = LoggerConfig::makeLogger( cfg, np + "Logger" );
		
		
		logger->setClassSpace( "HistoAnalyzer" );
		logger->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;
		
	    // create the book
	    logger->info(__FUNCTION__) << " Creating book " << config->getString( np + "output.data" ) << endl;
	    book = new HistoBook( config->getString( np + "output:path", "" ) + config->getString( np + "output.data" ), config, "", "" );
	    logger->info(__FUNCTION__) << " Creating report " << config->getString( np + "output.report" ) << endl;
	    if ( cfg->exists( np + "Reporter" ) )
		    reporter = new Reporter( cfg, np+"Reporter." );
		else 
			reporter = nullptr;

        INFO( "Looking for input @ input.data:url" )
	    INFO( " Loading data from " << config->getString( np + "input.data:url" ) )
		inFile = new TFile( cfg->getString( np+"input.data:url" ).c_str(), "READ" );
		INFO( "Input file : " << inFile )

	}

	HistoAnalyzer::~HistoAnalyzer(){
		delete book;
		if ( reporter )
			delete reporter;
		delete logger;
	}

}
