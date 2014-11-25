
#ifndef HISTOBOOK_H
#define HISTOBOOK_H

#include <map>
#include <string>
#include <sstream>
#include <stdarg.h>

/**
 * JDB
 */
#include "XmlConfig.h"
#include "Logger.h"
#include "HistoBins.h"

/**
 * ROOT
 */
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLegend.h"
#include "TFile.h"
#include "TStyle.h"
#include "TPad.h"
#include "TClass.h"


using namespace std;

namespace jdb{

	/**
	 * For legend Alignment
	 * may remove!!!
	 */
	class legendAlignment {
	public:
		static const int topLeft = 1;
		static const int topRight = 2;
		static const int topCenter = 3;
		static const int bottomLeft = 4;
		static const int bottomRight = 5;
		static const int bottomCenter = 6;

		static const int bottom = 0;
		static const int center = 1; // for x and y dirs
		static const int right = 2;
		static const int left = 3;
		static const int top = 4;

		// TODO: make a smart legend that trys to avoid data
		static const int best = 5;

		

	};





	/*jdoc{
		"class" : "HistoBook",
		"desc" : "A book keeper and feature rich environemnt for using Root Hsitograms"
	}*/
	class HistoBook {

	protected:

		Logger * logger;

		string currentDir;

		std::map<string, string> configPath;
		std::map<string, TH1*> book;
		
		string filename;
		
		TFile *file;

		/* Style and display */
		string styling;
		string drawOption;
		TLegend * legend;

		// optional config to use for all config related calls
		XmlConfig * config;



	public:

		/**
		 * Static Usage
		 */
		static vector<double> makeNBins( int nBins, double low, double high ){
			return HistoBins::makeNBins( nBins, low, high );
		}
		static vector<double> makeFixedWidthBins( double binWidth, double low, double high ){
			return HistoBins::makeFixedWidthBins( binWidth, low, high );
		}
		static int findBin( vector<double> &bins, double value ){
			return HistoBins::findBin( bins, value );
		}
		

		/*jdoc{
			"name" : "HistoBook( string name, string input = \"\", string inDir = \"\" )",
			"params" : [ "name", "input", "inDir" ],
			"paramDesc" : [ "Name of root file output", "Optional: input file to merge in", "Optional: directory in input to merge - others ignored" ],
			"returns" : [  ],
			"desc" : "Constructor: Creates a HistoBook with no config support (legacy )"
			}
		 */
		HistoBook( string name, string input = "", string inDir = "" );
		/*jdoc{
			"name" : "HistoBook( string name, XmlConfig* config, string input = \"\", string inDir = \"\" )",
			"params" : [ "name", "config", "input", "inDir" ],
			"paramDesc" : [ "Name of root file output", "Project config file", "Optional: input file to merge in", "Optional: directory in input" ],
			"returns" : [  ],
			"desc" : "Constructor: Creates a HistoBook with config support"
			}
		 */
		HistoBook( string name, XmlConfig* config, string input = "", string inDir = "");
		~HistoBook();

		/*jdoc{
			"name" : "string cd( string dir = \"/\" )",
			"params" : [
				"dir"
			],
			"paramDesc" : [
				"Directory to move into"
			],
			"returns" : [
				"Previous directory"
			],
			"desc" : "Changes into the given directory. If the dir DNE it is created, else it is simply set as the current. Subdirectories can be used if the entire path is given, paths are never relative/"
		}*/
		string cd( string dir = "/" );

		/*jdoc{
			"name" : "void add( string name, TH1 * h)",
			"params" : [
				"name", "h"
			],
			"paramDesc" : [
				"Key name for histogram in book", "Root Histogam"
			],
			"returns" : [
				
			],
			"desc" : "Adds a root histogram to the book for safe-keeping"
		}*/
		void add( string name, TH1 * );

		/*jdoc{
			"name" : "TH1* get( string name, string sdir = \"\" )",
			"params" : [
				"name", "sdir"
			],
			"paramDesc" : [
				"Name (key) of histogram", "Optional: look for the histogram in the directory sdir"
			],
			"returns" : [
				"TH1* to histogram - may be NULL if DNE"
			],
			"desc" : "Gets a ROOT histo out of the book by key"
		}*/
		TH1* get( string name, string sdir = "" );

		/*jdoc{
			"name" : "TH2* get2D( string name, string sdir = \"\" )",
			"params" : [
				"name", "sdir"
			],
			"paramDesc" : [
				"Name (key) of histogram", "Optional: look for the histogram in the directory sdir"
			],
			"returns" : [
				"TH2* to histogram - may be NULL if DNE"
			],
			"desc" : "Same as get but auto casts to a TH2"
		}*/
		TH2* get2D( string name, string sdir = "" );

		/*jdoc{
			"name" : "TH3* get3D( string name, string sdir = \"\" )",
			"params" : [
				"name", "sdir"
			],
			"paramDesc" : [
				"Name (key) of histogram", "Optional: look for the histogram in the directory sdir"
			],
			"returns" : [
				"TH3* to histogram - may be NULL if DNE"
			],
			"desc" : "Same as get but auto casts to a TH3"
		}*/
		TH3* get3D( string name, string sdir = "" );

		/*jdoc{
			"name" : "void fill( string name, double bin, double weight = 1)",
			"params" : [
				"name", "bin", "weight"
			],
			"paramDesc" : [
				"Key in book", "Bin index", "Weight for 1D, or y Bin index for TH2D"
			],
			"returns" : [
				
			],
			"desc" : "This method checks for existance unlike using get(...)->Fill(...). If the histo DNE then an error is reported through the Logger and execution continues."
		}*/
		void fill( string name, double bin, double weight = 1);
		void make1F( string name, string title, uint nBins, double low, double hi );
		void make1D( string name, string title, uint nBins, double low, double hi );
		void make1D( string name, string title, uint nBins, const Double_t* bins );
		void make2D( 	string name, string title, 
						uint nBinsX, double lowX, double hiX, uint nBinsY, double lowY, double hiY );
		void make2D( 	string name, string title, 
						uint nBinsX, const Double_t* xBins, uint nBinsY, double lowY, double hiY );
		void make2D( 	string name, string title, 
						uint nBinsX, double x1, double x2, uint nBinsY, const Double_t* yBins );
		void make2D( 	string name, string title, 
						uint nBinsX, const Double_t* xBins, uint nBinsY, const Double_t*yBins );
		
		/*jdoc{
			"name" : "void make( XmlConfig * config, string nodeName )",
			"params" : [
				"config", "nodeName"
			],
			"paramDesc" : [
				"Config containing histogram schema", "Path to node containing schema"
			],
			"returns" : [
				
			],
			"desc" : "Makes a histogram from a node in a config file <br/><code>&lthName key=\"\" nBins=\"42\" minX=\"0\" maxX=\"10\" \\&gt</code>"
		}*/
		void make( XmlConfig * config, string nodeName );
		
		/*jdoc{
			"name" : "void make( string nodeName )",
			"params" : [
				"nodeName"
			],
			"paramDesc" : [
				"Path to a histogram schema node"
			],
			"returns" : [
				
			],
			"desc" : "Makes a single histogram from the class config file given during construction"
		}*/
		void make( string nodeName );

		/*jdoc{
			"name" : "void makeAll( string nodeName )",
			"params" : [
				"nodeName"
			],
			"paramDesc" : [
				"Path to the parent node containing 1 or more histogram schema nodes"
			],
			"returns" : [
				
			],
			"desc" : "Makes all histograms that are children of the given parent node in the class config file given during construction"
		}*/
		void makeAll( string nodeName );
		void makeAll( XmlConfig * config, string nodeName );
		void clone( string existing, string create );

		TLegend* getLegend() { return legend; }

		HistoBook* draw(string name = "", Option_t* opt= "" );
		

		HistoBook* clearLegend() { legend->Clear(); return this; };
		
		// add a legend by setting the legendName
		

		TDirectory* getDirectory( ) { return gDirectory; }

		/*jdoc{
			"name" : "void save()",
			"desc" : "Saves all histograms and other Root objects attached to the file to the permanent file given during construction"
		}*/
		void save();

		/*jdoc{
			"name" : "HistoBook* style( string hName )",
			"params" : [
				"hName"
			],
			"paramDesc" : [
				"Name of histogram to style"
			],
			"returns" : [
				"Histobook * of this for function chaining"
			],
			"desc" : "Example: book->style( \"h1\" )->set( \"title\", \"science\" )->draw(); "
		}*/
		HistoBook* style( string hName );

		//set( string param, ... )
		// for function chaining and rapid styling
		HistoBook* set( string param, string p1, string p2 = "", string p3 = "", string p4 = "" );
		HistoBook* set( string param, double p1, double p2 = -1, double p3 = -1, double p4 = -1 );
		HistoBook* set( XmlConfig* config, string nodePath );
		HistoBook* set( string nodePath );
		HistoBook* set( string opt, vector<string> nodePath );

		HistoBook* exportAs( string filename = "" );

		bool exists( string name, string sdir = "" ){
			if ( NULL != get( name, sdir ) )
				return true;
			return false;
		}

		int color( string color ) {
			if ( "red" == color )
				return kRed;
			if ( "green" == color )
				return kGreen;
			if ( "blue" == color )
				return kBlue;
			if ( "black" == color )
				return kBlack;
			return -1;
		}

	private:
		void globalStyle();
		HistoBook* placeLegend( int alignmentX, int alignmentY, double width = -1, double height = -1 );
		void loadRootDir( TDirectory*, string path = "" );

		string sParam( vector<string> params, int p, string def="" ) {
			if ( p < params.size() )
				return params[ p ];
			else 
				return def;
		}
		char* cParam( vector<string> params, int p, string def="" ) {
			if ( p < params.size() )
				return (char*)(params[ p ].c_str());
			else 
				return (char*)(def.c_str());
		}
		double dParam( vector<string> params, int p, double def=0 ) {
			if ( p < params.size() )
				return atof( params[ p ].c_str() );
			else 
				return def;
		}

		



	};	
}






#endif