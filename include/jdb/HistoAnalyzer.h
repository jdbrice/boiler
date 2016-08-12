#ifndef HISTO_ANALYZER_H
#define HISTO_ANALYZER_H


// ROOBARB
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
	#include "IHistoBookMaker.h"
#include "XmlRange.h"
#include "XmlPoint.h"
#include "Reporter.h"
#include "TaskRunner.h"

using namespace jdb;

// ROOT
#include "TFile.h"
#include "TError.h"

// STL
#include <memory>
using namespace std;

namespace jdb{

	/* Base class for creating histogram analysis projects
	 *
	 * Sublcassing HistoAnalyzer provides a good starting point for
	 * modules that take a root file containing histograms as input.
	 *
	 */
	class HistoAnalyzer : public TaskRunner, public IHistoBookMaker
	{
	// protected properties
	protected:
		
		// Reporter for generating pdf reports
		shared_ptr<Reporter> 	reporter;
		// File containing input
		map<string, TFile*> rootFiles;
		TFile 		*inFile;
		string 		jobModifier;

		int jobIndex = -1;
	// public methods
	public:

		virtual const char* classname() const { return "HistoAnalyzer"; }

		/* Creates a HistoAnalyzer from an Xml Config
		 * @nodePath The path to the node containing the HistoAnalyzer data
		 * 
		 */
		HistoAnalyzer();
		/* Destructor
		 * Saves output and closes input
		 */
		~HistoAnalyzer();

		void setCurrentFile( string name ){
			if ( rootFiles.count( name ) >= 0 ){
				inFile = rootFiles[ name ];
			} else {
				ERROR( classname(), "No file named " << name << " available" );
			}
		}

		void addRootFile( string filename, string name = "" ){
			TFile * f = new TFile( filename.c_str(), "READ" );
			
			if ( "" == name )
				rootFiles[ filename ] = f;
			else 
				rootFiles[ name ] = f;
		}

		

		void initHistoBook( string _jobPostfix );
		void initReporter( string _jobPostfix );
		/* Initialize the input file(s)
		 *
		 * initRootFiles sets up the input files for the HistoAnalyzer
		 * It searches the config paths :
		 *		nodePath.input.TFile:url ( applies string format interpolation )
		 *		nodePath.input.data:url
		 *
		 * @return void
		 */
		void initRootFiles();

		virtual void init( XmlConfig &_config, string _nodePath="", int _jobIndex = -1);
		virtual void init( XmlConfig &_config, string _nodePath="", string _fileList = "", string _jobPostfix = "" );
		virtual void initialize() { DEBUG( classname(), "HistoAnalyzer" ); };

		/* The Maker function
		 *
		 * Use this public function to start the job.
		 * Subclasses should override.
		 */
		virtual void make() {}
		
	};
}

#endif