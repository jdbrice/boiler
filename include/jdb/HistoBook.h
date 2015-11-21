
#ifndef HISTOBOOK_H
#define HISTOBOOK_H

#include <map>
#include <string>
#include <sstream>
#include <stdarg.h>

// JDB
#include "XmlConfig.h"
#include "Logger.h"
#include "HistoBins.h"

// ROOT 
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLegend.h"
#include "TFile.h"
#include "TStyle.h"
#include "TPad.h"
#include "TClass.h"
#include "TObject.h"


using namespace std;

namespace jdb{


	/* A book keeper and helper for storing and using ROOT histograms
	 *
	 */
	class HistoBook {

	protected:

		// Current directory in output file
		string currentDir;
		// Map of hitogram names to paths in the Xml Config File for automaticlly created histos
		std::map<string, string> configPath;
		// Map of histogram names to objects
		std::map<string, TObject*> book;
		// Filename of output file
		string filename;
		// filename for input 
		string inputFilename;
		// directory to read in input file
		string inputDir;
		// Output file
		TFile *file;

		// Optional config to use for all config related calls
		XmlConfig * config;
		// Save on Exit or not
		bool saveAllOnExit;


	public:

		static const string tag;
		
		HistoBook( string name, string input = "", string inDir = "" );		
		HistoBook( string name, XmlConfig* config, string input = "", string inDir = "");
		~HistoBook();

		void initialize();
		void mergeIn( string _filename, string _dir );


		/*
		 *Changes into the given directory. 
		 *If the dir DNE it is created, or it is simply set as the current. 
		 *Subdirectories can be used if the entire path is given, paths are never relative
		 *
		 */
		string cd( string dir = "" );

		void ls( bool print = true );
		
		void add( string name, TH1 * );
		void add( string name, TObject* );
		TH1* get( string name, string sdir = "" );
		TH1 * operator[]( string name );
		TH2* get2D( string name, string sdir = "" );
		TH3* get3D( string name, string sdir = "" );



		/*
		 * This method checks for existance unlike using get(...)->Fill(...). 
		 * If the histo DNE then an error is reported through the Logger and execution continues.
		*/
		void fill( string name, double bin, double weight = 1.0 );
		void fill( string name, double binx, double biny, double weight);
		void fill( string name, double binx, double biny, double binz, double weight);


		/* Fill a histogram by name
		 * Checks for existance
		 */
		void fill( string name, string binLabel, double weight = 1 );

		bool setBinContent( string name, int bin, double content );
		bool setBin( string name, int bin, double content, double error );
		bool setBinError( string name, int bin, double error );
		

		TH1 * make( string type, string name, string title, HistoBins &bx, HistoBins &by, HistoBins &bz );

		/*
		 * Makes a histogram from a node in a config file 
		 * 
		 */
		void make( XmlConfig * config, string nodeName );
		
		/*
		 *Makes a single histogram from the class config file given during construction
		 *
		 * 
		 */
		void make( string nodeName );

		/*
		 *Makes all histograms that are children of the given parent 
		 *node in the class config file given during construction"
		 */
		void makeAll( string nodeName );
		void makeAll( XmlConfig * config, string nodeName );

		void clone( string existing, string create );
		void clone( string ePath, string existing, string cPath, string create );
		

		TDirectory* getDirectory( ) { return gDirectory; }

		/*
		 * Saves all histograms and other Root objects attached to the current file to 
		 * the permanent file given during construction
		 */
		void save( bool saveAllInDirectory = false );

		void saveOnExit( bool doIt = true ){
			INFO( tag, "Auto Save on exit set to " << doIt );
			saveAllOnExit = doIt;
		}

		bool exists( string name, string sdir = "" );

		bool is1D(string name, string sdir = "");
		bool is2D(string name, string sdir = "");
		bool is3D(string name, string sdir = "");

		void removeFromDir( string name, string sdir = "" );

	private:
		void globalStyle();
		void loadRootDir( TDirectory*, string path = "" );

#ifdef __CINT__
		ClassDef( jdb::HistoBook, 1 )
#endif
	};	
}






#endif