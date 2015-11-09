

#include "HistoBook.h"
#include "Logger.h"

// ROOT
#include "TKey.h"
#include "TObject.h"
#include <TApplication.h>

ClassImp( jdb::HistoBook );

namespace jdb{

	const string HistoBook::tag = "HistoBook";
	/**
	 * Creates a histobook and allows the root filename to be set. optionally read from an existing root
	 * file and include everything into the working space
	 * @param name  root filename
	 * @param input input filename
	 * @param inDir input starting directory
	 */
	HistoBook::HistoBook( string name, string input, string inDir ){

		
		if (name.find(  ".root") != std::string::npos){
			filename = name;
		} else
			filename = name + ".root";

		currentDir = "/";

		file = new TFile( filename.c_str(), "recreate" );
		file->cd();

		INFO( tag, "Output File : " << filename << " opened" );

		globalStyle();


		// if an input was given merge it into the live record
		if ( input.length() >= 5 ){
			INFO( tag, "Loading : " << inDir << " from " << input );
			TFile * fin = new TFile( input.c_str() );
			cd ( inDir );
			fin->cd( inDir.c_str() );
			loadRootDir( gDirectory, inDir );
		}

		saveOnExit( true );
	}	// Constructor

	/**
	 * Constructor that allows th input of a config file
	 * @param name name of file to use for saving root data
	 * @param con  The config file to use for all config relates calls
	 */
	HistoBook::HistoBook( string name, XmlConfig * con, string input, string inDir){

		// set the configuration 
		config = con;

		INFO( tag, "" );

		if (name.find(  ".root") != std::string::npos){
			filename = name;
		} else
			filename = name + ".root";

		currentDir = "/";

		file = new TFile( filename.c_str(), "recreate" );
		file->cd();

		INFO( tag, "Output File : " << filename << " opened" );


		globalStyle();


		INFO( tag, " Set Configuration " );

		// if an input was given merge it into the live record
		if ( input.length() >= 5 ){
			INFO( tag, "Loading : " << inDir << " from " << input );
			TFile * fin = new TFile( input.c_str() );
			cd ( inDir );
			fin->cd( inDir.c_str() );
			loadRootDir( gDirectory, inDir );
		}

		// default to auto save on exit
		saveOnExit( true );
	}	// Constructor

	/**
	 * Destructor
	 * Frees the legend and ensures that the data is written to file
	 * and the file is closed properly
	 */
	HistoBook::~HistoBook(){
		INFO( tag, "" );

		if ( saveAllOnExit  ){
			save();
		} else {
			WARN( tag, "Not Saving Book" );
		}
		file->Close();
		INFO( tag, " Memory freed, data written, file closed " );
		
	}	// Destructor

	void HistoBook::save(bool saveAllInDirectory ) {
		INFO( tag, " Save to File " << filename );
		
		if ( true )
			file->Write();
		else {
			for ( auto k : book ){
				if ( k.second ){
					DEBUG(tag,  "Writing " << k.first << " to file" );
					k.second->Write();
				} else {
					ERROR( tag, "Cannot Write " << k.first << " to file : null" );
				}
			}	
		}
	}	// save

	/**
	 * Loads a directory into the histobook
	 */
	void HistoBook::loadRootDir( TDirectory* tDir, string path ){
		INFO( tag, " Loading from directory " << path );

		TList* list;

		if ( tDir ){
			list = tDir->GetListOfKeys();
		} else {
			INFO( tag, " Bad Directory " << path );
			return;
		}

		TIter next(list);
		TKey* key;
		TObject* obj;

		while ( (key = (TKey*)next()) ) {

			obj = key->ReadObj() ;

			if ( 0 == strcmp(obj->IsA()->GetName(),"TDirectoryFile") ){
				TDirectoryFile* dir = (TDirectoryFile*)obj;

				string nPath = path + dir->GetName();
				if ( path == (string) "" )
					nPath = path + dir->GetName();
				else
					nPath = path + "/" + dir->GetName();

				cd( nPath );
				loadRootDir( dir, nPath );
			} else if ( obj ){
				if (    (strcmp(obj->IsA()->GetName(),"TProfile")!=0) && (!obj->InheritsFrom("TH2") && (!obj->InheritsFrom("TH1"))) ) {
					// not a 1d or 2d histogram
				} else {
					// add it to the book

					add( obj->GetName(), (TH1*)obj->Clone( obj->GetName() ) );
				}

			}
		}

	} // loadRootDir

	void HistoBook::add( string name, TH1* h ){
		

		string oName = name;
		if ( name.length() <= 1 || !h ){
			WARN( tag, " Cannot add " << name << " to dir " << currentDir << " : Invalid name" );
			return;
		}

		name = currentDir + name;

		// dont allow duplicated name overites
		if ( book[ name ] ){
			WARN( tag, " Cannot add " << name << " to dir " << currentDir << " : Duplicate exists" );
			return;
		}

		// save the histo to the map
		book[ name ] = h;
		DEBUG( " Adding " << name );

		// this is kept for legacy
		//add( name, (TObject*)h );
	} 	// add

	void HistoBook::add( string name, TObject* h ){

		DEBUG(tag,  " Adding " << name );

		string oName = name;
		if ( name.length() <= 1 || !h ){
			WARN( tag, " Cannot add " << name << " to dir " << currentDir << " : Invalid name" );
			return;
		}

		name = currentDir + name;

		// dont allow duplicated name overites
		if ( book[ name ] ){
			WARN( tag, " Cannot add " << name << " to dir " << currentDir << " : Duplicate exists" );
			return;
		}

		// save the histo to the map
		book[ name ] = h;

		cd();
		h->Write();

	} 	// add

	string HistoBook::cd( string sdir  ){

		string old = currentDir;
		if ( old != sdir )
			TRACE( tag, " cd to directory " << sdir );
		

		char* csdir = (char*)sdir.c_str();
		file->cd();

		if ( file->GetDirectory( csdir ) ){
			file->cd( csdir );
		} else {
			TRACE( tag, " Creating Directory " << sdir );
			file->mkdir( csdir );
			file->cd( csdir );
		}

		currentDir = sdir;

		return old;
	}	// cd


	void HistoBook::make( string nodeName ){
		if ( config )
			make( config, nodeName );
	}	//make
	void HistoBook::make( XmlConfig * config, string nodeName ){

		if ( config && config->exists( nodeName ) ){


			string hName = config->tagName( nodeName );
			// backward compatible
			if ( config->exists( nodeName + ":name" ) )
				hName = config->getString( nodeName + ":name" );
			if ( "" == hName )
				hName = nodeName;

			// store the path in the config file
			configPath[ hName ] = nodeName;

			string type = config->getString( nodeName + ":type" );
			string hTitle = config->getString( nodeName + ":title", hName );


			HistoBins* bx;
			HistoBins* by;
			HistoBins* bz;
			if ( config->exists( nodeName + ":xBins" ) )
				bx = new HistoBins( config, config->getString( nodeName + ":xBins" ) );
			else 
				bx = new HistoBins( config, nodeName, "X" );
			if ( config->exists( nodeName + ":yBins" ) )
				by = new HistoBins( config, config->getString( nodeName + ":yBins" ) );
			else 
				by = new HistoBins( config, nodeName, "Y" );
			if ( config->exists( nodeName + ":zBins" ) )
				bz = new HistoBins( config, config->getString( nodeName + ":zBins" ) );
			else 
				bz = new HistoBins( config, nodeName, "Z" );

			if ( "1D" == type || ( (bx->nBins() > 0) && (by->nBins() <= 0) && (bz->nBins() <= 0) )){
				TRACE( tag, bx->toString() );
				if ( bx->nBins() >= 1 )
					make1D( hName, hTitle, bx->nBins(), bx->bins.data() );
				else 
					WARN( tag, "Invalid Bins given for " << hName );

			} else if ( "2D" == type || ( (bx->nBins() > 0) && (by->nBins() > 0) && (bz->nBins() <= 0) )){

				if ( bx->nBins() >= 1 && by->nBins() >= 1 )
					make2D( hName, hTitle,
						bx->nBins(), bx->bins.data(), by->nBins(), by->bins.data() );
				else 
					WARN( tag, "Invalid Bins given for " << hName );

			} else if ( "3D" == type || ( (bx->nBins() > 0) && (by->nBins() > 0) && (bz->nBins() > 0) )){

				if ( bx->nBins() >= 1 && by->nBins() >= 1 && bz->nBins() >= 1 )
					make3D( hName, hTitle,
						bx->nBins(), bx->bins.data(), by->nBins(), by->bins.data(), bz->nBins(), bz->bins.data() );
				else 
					WARN( tag, "Invalid Bins given for " << hName );

			} else {
				WARN( tag, "Histogram " << hName << " was not made ");
			}



		}

	}	// make
	void HistoBook::makeAll( XmlConfig * con, string nodeName ){

		if ( !con )
			return;
		vector<string> paths = con->childrenOf( nodeName );
		INFO( tag, " Found " << paths.size() << " histogram paths " );
		for ( int i=0; i < paths.size(); i++ ){

			make( con, paths[ i ] );
		}
	}	//makeAll
	void HistoBook::makeAll( string nodeName ){
		if ( !config ){
			WARN( tag, "No config" );
			return;
		}

		makeAll( config, nodeName );
	}	//makeAll

	void HistoBook::clone( string existing, string create ){

		INFO( tag, " Cloning " << existing << " into " << create );
		if ( get( existing ) ){
			TH1* nHist = (TH1*)get( existing )->Clone( create.c_str() );
			// add the new one
			add( create, nHist );
		} else {
			WARN( tag, existing << " Does Not Exist " );
		}
	}	//clone
	void HistoBook::clone( string ePath, string existing, string cPath, string create ){

		INFO( tag, " Cloning " << existing << " into " << create );
		if ( get( existing, ePath ) ){
			string oDir = cd( cPath ); 
			
			TH1* nHist = (TH1*)get( existing, ePath )->Clone( create.c_str() );
			
			
			// add the new one
			add( create, nHist );

			cd( oDir );
		} else {
			WARN( tag, existing << " Does Not Exist " );
		}
	}	//clone

	void HistoBook::make1F( string name, string title, int nBins, double low, double hi  ){
		INFO( tag, "TH1F( " << name << ", " << title << ", " << nBins << ", " << low << ", " << hi << " )" );
		TH1F* h;
		h = new TH1F( name.c_str(), title.c_str(), nBins, low, hi );

		this->add( name, h );
	}	//make1F

	void HistoBook::make1D( string name, string title, int nBins, double low, double hi  ){

		INFO( tag, "TH1D( " << name << ", " << title << ", " << nBins << ", " << low << ", " << hi << " )" );
		TH1D* h;
		h = new TH1D( name.c_str(), title.c_str(), nBins, low, hi );

		this->add( name, h );
	}	//make1D

	void HistoBook::make1D( string name, string title, int nBins, const Double_t* bins  ){
		INFO( tag, "TH1D( " << name << ", " << title << ", " << nBins << ", " << "[]" <<  " )" );
		TH1D* h;
		h = new TH1D( name.c_str(), title.c_str(), nBins, bins );

		this->add( name, h );
	}	//make1D

	void HistoBook::make2D( string name, string title, int nBinsX, double lowX, double hiX, int nBinsY, double lowY, double hiY ){

		INFO( tag, "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY << ", " << lowY << ", " << hiY << " )" );
		TH2D* h;

		h = new TH2D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, lowY, hiY );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, int nBinsX, const Double_t* xBins, int nBinsY, double lowY, double hiY ){
		INFO( tag, "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", " << lowY << ", " << hiY << " )" );
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, lowY, hiY );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, int nBinsX, double lowX, double hiX, int nBinsY, const Double_t* yBins  ){
		INFO( tag, "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY <<  "[] )" );
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, yBins );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, int nBinsX, const Double_t* xBins, int nBinsY, const Double_t * yBins ){
		INFO( tag, "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", []" << " )" );
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, yBins );

		this->add( name, h );
	}	//make2D

	void HistoBook::make3D( 	string name, string title, 
						int nBinsX, double lowX, double hiX, int nBinsY, double lowY, double hiY, int nBinsZ, double lowZ, double hiZ ){
		INFO( tag, "TH3D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY << ", " << lowY << ", " << hiY << ", " << nBinsZ << ", " << lowZ << ", " << hiZ << " )" );
		TH3D* h;

		h = new TH3D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, lowY, hiY, nBinsZ, lowZ, hiZ );

		this->add( name, h );
	}
	/*void make3D( 	string name, string title, 
					int nBinsX, const Double_t* xBins, int nBinsY, double lowY, double hiY, int nBinsZ, double lowZ, double hiZ );
	void make3D( 	string name, string title, 
					int nBinsX, double lowX, double hiX, int nBinsY, const Double_t* yBins, int nBinsZ, double lowZ, double hiZ );
	void make3D( 	string name, string title, 
					int nBinsX, double lowX, double hiX, int nBinsY, double lowY, double hiY, int nBinsZ, const Double_t* zBins );
	*/
	// TODO: implement these and add remaining definitions
	// TODO: test the 3D histograms
	// TODO: add a fill method for 3d histograms

	void HistoBook::make3D( 	string name, string title, 
	 				int nBinsX, const Double_t* xBins, int nBinsY, const Double_t*yBins, int nBinsZ, const Double_t*zBins ){
		INFO( tag, "TH3D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", []" << ", " << nBinsZ << ", []" <<  " )" );
		TH3D* h;
		h = new TH3D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, yBins, nBinsZ, zBins );

		this->add( name, h );
	}


	TH1* HistoBook::get( string name, string sdir  ){
		if ( sdir.compare("") == 0)
			sdir = currentDir;

		if ( NULL == book[ ( sdir + name  ) ] )
			WARN( tag, sdir + name  << " Does Not Exist " );

		return (TH1*)book[ ( sdir  + name  ) ];
	}	//get

	bool HistoBook::exists( string name, string sdir ){
		if ( sdir.compare("") == 0)
			sdir = currentDir;

		if ( NULL == book[ ( sdir + name  ) ] ){
			DEBUG( sdir + name  << " Does Not Exist " ) 
			return false;
		}

		DEBUG( sdir + name  << " Does Exist " ) 
		return true;
	}

	TH1* HistoBook::operator[]( string name ) {
		return get( name );
	}

	TH2* HistoBook::get2D( string name, string sdir  ){
		return ((TH2*)get( name, sdir ));
	}	//get2D
	TH3* HistoBook::get3D( string name, string sdir ){
		return (( TH3* ) get( name, sdir ));
	}	//get3D

	void HistoBook::fill( string name, double bin, double weight ){
		if ( get( name ) != 0)
			get( name )->Fill( bin, weight );
		else
			WARN( tag, name << " Does Not Exist, cannot fill " );
	}	//fill

	void HistoBook::fill( string name, string binLabel, double weight ){
		if ( get( name ) != 0)
			get( name )->Fill( binLabel.c_str(), weight );
		else
			WARN( tag, name << " Does Not Exist, cannot fill " );
	}	//fill

	bool HistoBook::setBinContent( string name, int bin, double content ){
		if ( !exists( name ) )
			return false;
		get( name )->SetBinContent( bin, content );
		return true;
	}
	bool HistoBook::setBinError( string name, int bin, double error ){
		if ( !exists( name ) )
			return false;
		get( name )->SetBinError( bin, error );
		return true;
	}
	bool HistoBook::setBin( string name, int bin, double content, double error ){
		if ( !exists( name ) )
			return false;
		get( name )->SetBinContent( bin, content );
		get( name )->SetBinError( bin, error );
		return true;
	}

	void HistoBook::globalStyle(){

		gStyle->SetCanvasColor(kWhite);     // background is no longer mouse-dropping white
	  	gStyle->SetPalette(1,0);            // blue to red false color palette. Use 9 for b/w
	  	gStyle->SetCanvasBorderMode(0);     // turn off canvas borders
	  	gStyle->SetPadBorderMode(0);
	  	gStyle->SetPaintTextFormat("5.2f");  // What precision to put numbers if plotted with "TEXT"

	  	// For publishing:
	  	gStyle->SetLineWidth(2.);
	  	gStyle->SetTextSize(0.7);
	  	gStyle->SetLabelSize(0.05,"xy");
	  	gStyle->SetTitleSize(0.05,"xy");
	  	gStyle->SetTitleOffset(1.0,"x");
	  	gStyle->SetTitleOffset(1.0,"y");
	  	gStyle->SetPadTopMargin(0.1);
	  	gStyle->SetPadRightMargin(0.1);
	  	gStyle->SetPadBottomMargin(0.16);
	  	gStyle->SetPadLeftMargin(0.21);

	  	gStyle->SetFillColor(-1);
		gStyle->SetFillStyle(4000);

	}	//globalStyle

} // jdb namespace
