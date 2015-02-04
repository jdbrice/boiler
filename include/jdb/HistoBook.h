
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
		
		HistoBook( string name, string input = "", string inDir = "" );		
		HistoBook( string name, XmlConfig* config, string input = "", string inDir = "");
		~HistoBook();

		/*
		 *Changes into the given directory. 
		 *If the dir DNE it is created, or it is simply set as the current. 
		 *Subdirectories can be used if the entire path is given, paths are never relative
		 *
		*/
		string cd( string dir = "/" );

		
		void add( string name, TH1 * );
		TH1* get( string name, string sdir = "" );

		TH1 * operator[]( string name );

		TH2* get2D( string name, string sdir = "" );

		TH3* get3D( string name, string sdir = "" );

		/*
		 * This method checks for existance unlike using get(...)->Fill(...). 
		 * If the histo DNE then an error is reported through the Logger and execution continues.
		*/
		void fill( string name, double bin, double weight = 1);

		void make1F( string name, string title, int nBins, double low, double hi );
		void make1D( string name, string title, int nBins, double low, double hi );
		void make1D( string name, string title, int nBins, const Double_t* bins );
		void make2D( 	string name, string title, 
						int nBinsX, double lowX, double hiX, int nBinsY, double lowY, double hiY );
		void make2D( 	string name, string title, 
						int nBinsX, const Double_t* xBins, int nBinsY, double lowY, double hiY );
		void make2D( 	string name, string title, 
						int nBinsX, double x1, double x2, int nBinsY, const Double_t* yBins );
		void make2D( 	string name, string title, 
						int nBinsX, const Double_t* xBins, int nBinsY, const Double_t*yBins );
		
		/*
		 * Makes a histogram from a node in a config file 
		 * 
		 */
		void make( XmlConfig * config, string nodeName );
		
		/*
		 *Makes a single histogram from the class config file given during construction
		 */
		void make( string nodeName );

		/*
		 *Makes all histograms that are children of the given parent 
		 *node in the class config file given during construction"
		 */
		void makeAll( string nodeName );
		void makeAll( XmlConfig * config, string nodeName );
		void clone( string existing, string create );

		TLegend* getLegend() { return legend; }

		HistoBook* draw(string name = "", Option_t* opt= "" );
		

		HistoBook* clearLegend() { legend->Clear(); return this; };
		
		// add a legend by setting the legendName
		

		TDirectory* getDirectory( ) { return gDirectory; }

		/*
		 * Saves all histograms and other Root objects attached to the current file to 
		 * the permanent file given during construction
		 */
		void save();

		/* Styles a histogram 
		 *Example: book->style( "h1" )->set( "title", "science" )->draw(); "
		 */
		HistoBook* style( string hName );

		//set( string param, ... )
		// for function chaining and rapid styling
		HistoBook* set( string param, string p1, string p2 = "", string p3 = "", string p4 = "" );
		HistoBook* set( string param, double p1, double p2 = -1, double p3 = -1, double p4 = -1 );
		HistoBook* set( XmlConfig* config, string nodePath );
		HistoBook* set( string nodePath );
		HistoBook* set( string opt, vector<string> nodePath );

		HistoBook* exportAs( string filename = "" );

		bool exists( string name, string sdir = "" );

		bool is1D(string name, string sdir = ""){
			if ( exists( name, sdir) ){
				TH1* h = get( name, sdir);
				if ( h->GetNbinsY() <= 1 && h->GetNbinsZ() <= 1 && h->GetNbinsX() >= 1 ){
					return true;
				}
			}
			return false;
		} 
		bool is2D(string name, string sdir = ""){
			if ( exists( name, sdir) ){
				TH1* h = get( name, sdir);
				if ( h->GetNbinsY() >= 2 && h->GetNbinsZ() <= 1 && h->GetNbinsX() >= 1 ){
					return true;
				}
			}
			return false;
		} 
		bool is3D(string name, string sdir = ""){
			if ( exists( name, sdir) ){
				TH1* h = get( name, sdir);
				if ( h->GetNbinsY() >= 2 && h->GetNbinsZ() >= 2 && h->GetNbinsX() >= 1 ){
					return true;
				}
			}
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

		void removeFromDir( string name, string sdir = "" ){
			TH1 * h = get( name, sdir );
			if ( h )
				h->SetDirectory( 0 );
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