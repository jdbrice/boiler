#ifndef I_OUTPUT_MAKER_H
#define I_OUTPUT_MAKER_H

#include "IObject.h"
#include "Logger.h"
#include "XmlConfig.h"
#include "HistoBook.h"
#include "XmlString.h"

#include "TFile.h"

#include <memory>

class IHistoBookMaker : virtual public IObject
{
public:
	virtual const char* classname() const { return "IHistoBookMaker"; }
	IHistoBookMaker() {}
	~IHistoBookMaker() {}

	//Store project histograms and data
	shared_ptr<HistoBook> 	book = nullptr;

	virtual void initializeHistoBook( XmlConfig &config, string nodePath, int jobIndex, string _jobPostfix ){
		INFO( classname(), "hmm" );
		// do something with tfileOutput
		string outputPath ="";
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
			ofn = "histoBook.root";
		}

		// create the book
		DEBUG( classname(), " Creating book : " << ofn );
		book = shared_ptr<HistoBook>(new HistoBook( ofn, config ) );
	}
	
};

#endif