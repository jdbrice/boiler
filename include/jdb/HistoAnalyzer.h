#ifndef HISTO_ANALYZER_H
#define HISTO_ANALYZER_H


// ROOBARB
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
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
	class HistoAnalyzer : public TaskRunner
	{
	// protected properties
	protected:
		
		// HistoBook used to organize ROOT objects
		shared_ptr<HistoBook> 	book;
		// Reporter for generating pdf reports
		shared_ptr<Reporter> 	reporter;
		// File containing input
		vector<TFile*> rootFiles;
		vector<string> rootFileNames;
		TFile 		*inFile;
		string 		jobModifier;
		string 		outputPath;

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

		void setCurrentFile( unsigned int index = 0){
			if ( rootFiles.size() > index ){
				inFile = rootFiles[index ];
			} else {
				ERROR( classname(), "Invalid File Index " << index );
			}
			return;
		}
		void setCurrentFile( string filename ){
			int index = (find( rootFileNames.begin(), rootFileNames.end(), filename ) - rootFileNames.begin());
			setCurrentFile( index );
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