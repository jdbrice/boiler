#ifndef HISTO_ANALYZER_H
#define HISTO_ANALYZER_H


/**
 * JDB 
 */
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Reporter.h"
using namespace jdb;

/**
 * ROOT
 */
#include "TFile.h"
#include "TError.h"

/*jdoc{
	"class" : "HistoAnalyzer",
	"desc" : "Base class for creating histogram analysis projects"
}*/
class HistoAnalyzer
{
// protected properties
protected:
	/*jdoc{ "name" : "Logger *logger", "desc" : "The logging object for the job"}*/
	Logger 		*logger;
	/*jdoc{"name" : "XmlConfig *cfg", "desc" : ""}*/
	XmlConfig 	*cfg;
	/*jdoc{"name" : "string nodePath", "desc" : ""}*/
	string 		nodePath;
	/*jdoc{"name" : "HistoBook *book", "desc" : ""}*/
	HistoBook 	*book;
	/*jdoc{"name" : "Reporter *reporter", "desc" : ""}*/
	Reporter 	*reporter;
	/*jdoc{"name" : "TFile *inFile", "desc" : "Input file with histogram data"}*/
	TFile 		*inFile;

// public methods
public:

	/*jdoc{
		"name" : "HistoAnalyzer( XmlConfig * config, string nodePath )",
		"params" : [ "config", "nodePath" ],
		"paramDesc" : [ "Project config object", 
			"The node path containing configuration parameters"
		],
		"returns" : [  ],
		"desc" : ""
	}*/
	HistoAnalyzer( XmlConfig * config, string nodePath );
	~HistoAnalyzer();

	/*jdoc{
		"name" : "virtual void make()",
		"params" : [ ],
		"paramDesc" : [ ],
		"returns" : [  ],
		"desc" : "The maker function for publicly starting the job"
	}*/
	virtual void make() {}
	
};


#endif