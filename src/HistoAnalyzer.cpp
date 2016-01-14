#include "HistoAnalyzer.h"

namespace jdb{

	HistoAnalyzer::HistoAnalyzer( XmlConfig * config, string np, bool _setup ){
		

		//Set the Root Output Level
		gErrorIgnoreLevel = kSysError;

		// Save Class Members for config
		this->config = *config;
		this->nodePath = this->config.basePath( np );
		
		if ( _setup )
			setup();
	}

	void HistoAnalyzer::setup(){

		
	    // create the book
	    INFO( classname(), " Creating book " << this->config.getString( nodePath + ".output.data" ) )
	    book = shared_ptr<HistoBook>(new HistoBook( this->config.getString( nodePath + ".output:path", "" ) + this->config.getString( nodePath + "output.data" ), this->config, "", "" ));


	    INFO( classname(), " Creating report " << this->config.getString( nodePath + ".output.report" ) );
	    if ( this->config.exists( nodePath + ".Reporter.output:url" ) )
		    reporter = shared_ptr<Reporter>(new Reporter( this->config, nodePath + ".Reporter." ));
		else 
			reporter = nullptr;


        INFO( classname(), "Looking for input @ input.data:url" )
	    INFO( classname(), " Loading data from " << this->config.getString( nodePath + ".input.data:url" ) )
		inFile = new TFile( this->config.getString( nodePath + ".input.data:url" ).c_str(), "READ" );
		INFO( classname(), "Input file : " << inFile )

		if ( !inFile->IsOpen()  )
			ERROR( classname(), "Data File Could not be opened" )
	}

	HistoAnalyzer::~HistoAnalyzer(){
		
	}

}
