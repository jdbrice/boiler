

#include "HistoBook.h"
#include "Logger.h"
#include "LoggerConfig.h"

// ROOT
#include "TKey.h"
#include "TObject.h"
#include <TApplication.h>


namespace jdb{
	/**
	 * Creates a histobook and allows the root filename to be set. optionally read from an existing root
	 * file and include everything into the working space
	 * @param name  root filename
	 * @param input input filename
	 * @param inDir input starting directory
	 */
	HistoBook::HistoBook( string name, string input, string inDir ){

		// setup the logger
		logger = new Logger( Logger::getGlobalLogLevel(), "HistoBook" );
		

		if (name.find(  ".root") != std::string::npos){
			filename = name;
		} else
			filename = name + ".root";

		currentDir = "/";

		file = new TFile( filename.c_str(), "recreate" );
		file->cd();

		logger->info(__FUNCTION__) << "Output File : " << filename << " opened" << endl;


		// make the legend and draw it once to apply styles etc.
		// for some reason needed to make styling work on the first draw
		legend = new TLegend( 0.65, 0.65, 0.9, 0.9);
		legend->SetFillColor( kWhite );
		legend->Draw();
		legend->Clear();

		globalStyle();


		// if an input was given merge it into the live record
		if ( input.length() >= 5 ){
			logger->info(__FUNCTION__) << "Loading : " << inDir << " from " << input << endl;
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

		// make the logger
		logger = new Logger( Logger::getGlobalLogLevel(), "HistoBook" );
		

		logger->info(__FUNCTION__) << "" << endl;

		if (name.find(  ".root") != std::string::npos){
			filename = name;
		} else
			filename = name + ".root";

		currentDir = "/";

		file = new TFile( filename.c_str(), "recreate" );
		file->cd();

		logger->info(__FUNCTION__) << "Output File : " << filename << " opened" << endl;


		// make the legend and draw it once to apply styles etc.
		// for some reason needed to make styling work on the first draw
		legend = new TLegend( 0.65, 0.65, 0.9, 0.9);
		legend->SetFillColor( kWhite );
		legend->Draw();
		legend->Clear();

		globalStyle();


		logger->info(__FUNCTION__) << " Set Configuration " << endl;

		// if an input was given merge it into the live record
		if ( input.length() >= 5 ){
			logger->info(__FUNCTION__) << "Loading : " << inDir << " from " << input << endl;
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
		logger->info(__FUNCTION__) << "" << endl;
		delete legend;

		if ( saveAllOnExit  ){
			save();
		} else {
			logger->warn() << "Not Saving Book" << endl;
		}
		file->Close();
		logger->info(__FUNCTION__) << " Memory freed, data written, file closed " << endl;
		delete logger;
	}	// Destructor

	void HistoBook::save(bool saveAllInDirectory ) {
		logger->info(__FUNCTION__) << " Save to File " << filename << endl;
		
		if ( true )
			file->Write();
		else {
			for ( auto k : book ){
				if ( k.second ){
					logger->debug(__FUNCTION__) << "Writing " << k.first << " to file" << endl;
					k.second->Write();
				} else {
					logger->error(__FUNCTION__) << "Cannot Write " << k.first << " to file : null" << endl;
				}
			}	
		}
	}	// save

	/**
	 * Loads a directory into the histobook
	 */
	void HistoBook::loadRootDir( TDirectory* tDir, string path ){
		logger->info(__FUNCTION__) << " Loading from directory " << path << endl;

		TList* list;

		if ( tDir ){
			list = tDir->GetListOfKeys();
		} else {
			logger->info(__FUNCTION__) << " Bad Directory " << path << endl;
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
		logger->debug(__FUNCTION__) << " Adding " << name << endl;

		string oName = name;
		if ( name.length() <= 1 || !h ){
			logger->warn(__FUNCTION__) << " Cannot add " << name << " to dir " << currentDir << " : Invalid name" << endl;
			return;
		}

		name = currentDir + name;

		// dont allow duplicated name overites
		if ( book[ name ] ){
			logger->warn(__FUNCTION__) << " Cannot add " << name << " to dir " << currentDir << " : Duplicate exists" << endl;
			return;
		}

		// save the histo to the map
		book[ name ] = h;

		// this is kept for legacy
		//add( name, (TObject*)h );
	} 	// add

	void HistoBook::add( string name, TObject* h ){

		logger->debug(__FUNCTION__) << " Adding " << name << endl;

		string oName = name;
		if ( name.length() <= 1 || !h ){
			logger->warn(__FUNCTION__) << " Cannot add " << name << " to dir " << currentDir << " : Invalid name" << endl;
			return;
		}

		name = currentDir + name;

		// dont allow duplicated name overites
		if ( book[ name ] ){
			logger->warn(__FUNCTION__) << " Cannot add " << name << " to dir " << currentDir << " : Duplicate exists" << endl;
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
			logger->trace(__FUNCTION__) << " cd to directory " << sdir << endl;
		

		char* csdir = (char*)sdir.c_str();
		file->cd();

		if ( file->GetDirectory( csdir ) ){
			file->cd( csdir );
		} else {
			logger->trace(__FUNCTION__) << " Creating Directory " << sdir << endl;
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
				logger->trace( __FUNCTION__ )<< bx->toString() << endl;
				if ( bx->nBins() >= 1 )
					make1D( hName, hTitle, bx->nBins(), bx->bins.data() );
				else 
					logger->warn(__FUNCTION__) << "Invalid Bins given for " << hName << endl;

			} else if ( "2D" == type || ( (bx->nBins() > 0) && (by->nBins() > 0) && (bz->nBins() <= 0) )){

				if ( bx->nBins() >= 1 && by->nBins() >= 1 )
					make2D( hName, hTitle,
						bx->nBins(), bx->bins.data(), by->nBins(), by->bins.data() );
				else 
					logger->warn(__FUNCTION__) << "Invalid Bins given for " << hName << endl;

			} else if ( "3D" == type || ( (bx->nBins() > 0) && (by->nBins() > 0) && (bz->nBins() > 0) )){

				if ( bx->nBins() >= 1 && by->nBins() >= 1 && bz->nBins() >= 1 )
					make3D( hName, hTitle,
						bx->nBins(), bx->bins.data(), by->nBins(), by->bins.data(), bz->nBins(), bz->bins.data() );
				else 
					logger->warn(__FUNCTION__) << "Invalid Bins given for " << hName << endl;

			} else {
				logger->warn(__FUNCTION__) << "Histogram " << hName << " was not made "<< endl;
			}



		}

	}	// make
	void HistoBook::makeAll( XmlConfig * con, string nodeName ){

		if ( !con )
			return;
		vector<string> paths = con->childrenOf( nodeName );
		logger->info(__FUNCTION__) << " Found " << paths.size() << " histogram paths " << endl;
		for ( int i=0; i < paths.size(); i++ ){

			make( con, paths[ i ] );
		}
	}	//makeAll
	void HistoBook::makeAll( string nodeName ){
		if ( !config ){
			logger->info( __FUNCTION__ ) << "No config" << endl;
			return;
		}

		makeAll( config, nodeName );
	}	//makeAll

	void HistoBook::clone( string existing, string create ){

		logger->info(__FUNCTION__) << " Cloning " << existing << " into " << create << endl;
		if ( get( existing ) ){
			TH1* nHist = (TH1*)get( existing )->Clone( create.c_str() );
			// add the new one
			add( create, nHist );
		} else {
			logger->warn(__FUNCTION__) << existing << " Does Not Exist " << endl;
		}
	}	//clone
	void HistoBook::clone( string ePath, string existing, string cPath, string create ){

		logger->info(__FUNCTION__) << " Cloning " << existing << " into " << create << endl;
		if ( get( existing, ePath ) ){
			string oDir = cd( cPath ); 
			
			TH1* nHist = (TH1*)get( existing, ePath )->Clone( create.c_str() );
			
			
			// add the new one
			add( create, nHist );

			cd( oDir );
		} else {
			logger->warn(__FUNCTION__) << existing << " Does Not Exist " << endl;
		}
	}	//clone

	void HistoBook::make1F( string name, string title, int nBins, double low, double hi  ){
		logger->info(__FUNCTION__) << "TH1F( " << name << ", " << title << ", " << nBins << ", " << low << ", " << hi << " )" << endl;
		TH1F* h;
		h = new TH1F( name.c_str(), title.c_str(), nBins, low, hi );

		this->add( name, h );
	}	//make1F

	void HistoBook::make1D( string name, string title, int nBins, double low, double hi  ){

		logger->info(__FUNCTION__) << "TH1D( " << name << ", " << title << ", " << nBins << ", " << low << ", " << hi << " )" << endl;
		TH1D* h;
		h = new TH1D( name.c_str(), title.c_str(), nBins, low, hi );

		this->add( name, h );
	}	//make1D

	void HistoBook::make1D( string name, string title, int nBins, const Double_t* bins  ){
		logger->info(__FUNCTION__) << "TH1D( " << name << ", " << title << ", " << nBins << ", " << "[]" <<  " )" << endl;
		TH1D* h;
		h = new TH1D( name.c_str(), title.c_str(), nBins, bins );

		this->add( name, h );
	}	//make1D

	void HistoBook::make2D( string name, string title, int nBinsX, double lowX, double hiX, int nBinsY, double lowY, double hiY ){

		logger->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY << ", " << lowY << ", " << hiY << " )" << endl;
		TH2D* h;

		h = new TH2D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, lowY, hiY );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, int nBinsX, const Double_t* xBins, int nBinsY, double lowY, double hiY ){
		logger->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", " << lowY << ", " << hiY << " )" << endl;
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, lowY, hiY );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, int nBinsX, double lowX, double hiX, int nBinsY, const Double_t* yBins  ){
		logger->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY <<  "[] )" << endl;
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, lowX, hiX, nBinsY, yBins );

		this->add( name, h );
	}	//make2D

	void HistoBook::make2D( string name, string title, int nBinsX, const Double_t* xBins, int nBinsY, const Double_t * yBins ){
		logger->info(__FUNCTION__) << "TH2D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", []" << " )" << endl;
		TH2D* h;
		h = new TH2D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, yBins );

		this->add( name, h );
	}	//make2D

	void HistoBook::make3D( 	string name, string title, 
						int nBinsX, double lowX, double hiX, int nBinsY, double lowY, double hiY, int nBinsZ, double lowZ, double hiZ ){
		logger->info(__FUNCTION__) << "TH3D( " << name << ", " << title << ", " << nBinsX << ", " << lowX << ", " << hiX << ", " << nBinsY << ", " << lowY << ", " << hiY << ", " << nBinsZ << ", " << lowZ << ", " << hiZ << " )" << endl;
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
		logger->info(__FUNCTION__) << "TH3D( " << name << ", " << title << ", " << nBinsX << ", " << "[]" << ", " << nBinsY << ", []" << ", " << nBinsZ << ", []" <<  " )" << endl;
		TH3D* h;
		h = new TH3D( name.c_str(), title.c_str(), nBinsX, xBins, nBinsY, yBins, nBinsZ, zBins );

		this->add( name, h );
	}


	TH1* HistoBook::get( string name, string sdir  ){
		if ( sdir.compare("") == 0)
			sdir = currentDir;

		if ( NULL == book[ ( sdir + name  ) ] )
			logger->warn(__FUNCTION__) << sdir + "/" + name  << " Does Not Exist " << endl; 

		return (TH1*)book[ ( sdir  + name  ) ];
	}	//get

	bool HistoBook::exists( string name, string sdir ){
		if ( sdir.compare("") == 0)
			sdir = currentDir;

		if ( NULL == book[ ( sdir + name  ) ] ){
			logger->info(__FUNCTION__) << sdir + name  << " Does Not Exist " << endl; 
			return false;
		}

		logger->info(__FUNCTION__) << sdir + name  << " Does Exist " << endl; 
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
			logger->warn(__FUNCTION__) << name << " Does Not Exist, cannot fill " << endl;
	}	//fill

	void HistoBook::fill( string name, string binLabel, double weight ){
		if ( get( name ) != 0)
			get( name )->Fill( binLabel.c_str(), weight );
		else
			logger->warn(__FUNCTION__) << name << " Does Not Exist, cannot fill " << endl;
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


	HistoBook* HistoBook::exportAs( string filename ) {

		string outName = styling + ".png";
		if ( "" != filename )
			outName = filename;

		logger->info(__FUNCTION__) << "Exporting Pad to " << outName << endl;

		gPad->SaveAs( outName.c_str() );
		return this;

	}	//exportAs

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
	  	gStyle->SetTitleOffset(1.5,"y");
	  	gStyle->SetPadTopMargin(0.1);
	  	gStyle->SetPadRightMargin(0.1);
	  	gStyle->SetPadBottomMargin(0.16);
	  	gStyle->SetPadLeftMargin(0.2);

	  	gStyle->SetFillColor(-1);
		gStyle->SetFillStyle(4000);

	}	//globalStyle

	HistoBook* HistoBook::style( string histName ){
		styling = histName;

		logger->info(__FUNCTION__) << " Styling " << histName << endl;

		// set the default style if it is there
		if ( config && config->exists( configPath[ histName ] + ".style" ) ){
			logger->info(__FUNCTION__) << " Setting style from " << configPath[ histName ] << ".style" << endl;
			set( configPath[histName] + ".style" );
		} else if ( config && config->exists( configPath[ histName ] + ":style" ) && config->exists( config->getString( configPath[ histName ] + ":style" ) ) ){
			logger->info(__FUNCTION__) << " Setting Style from " << config->getString( configPath[ histName ] + ":style" ) << endl;
			set( config->getString( configPath[ histName ] + ":style" ) );
		}

		return this;
	}

	HistoBook* HistoBook::set( string param, string p1, string p2, string p3, string p4 ){

		vector<string> l;
		l.push_back( p1 );
		l.push_back( p2 );
		l.push_back( p3 );
		l.push_back( p4 );
		set( param, l );

		return this;
	}

	HistoBook* HistoBook::set( string param, double p1, double p2, double p3, double p4  ){

		vector<string> list;
		stringstream sstr;
		sstr.str("");
		sstr << p1;
		list.push_back( sstr.str() );

		sstr.str("");
		sstr << p2;
		list.push_back( sstr.str() );

		sstr.str("");
		sstr << p3;
		list.push_back( sstr.str() );

		sstr.str("");
		sstr << p4;
		list.push_back( sstr.str() );

		set( param, list );

		return this;
	}

	HistoBook* HistoBook::set( XmlConfig* config, string nodePath ){

		// get the list of attributes and set the style from that
		vector< string > list = config->attributesOf( nodePath );
		for ( int i = 0; i < list.size(); i++ ){
			vector<string> params = config->getStringVector( list[ i ] );
			set( config->attributeName( list[ i ] ), params );
		}

		return this;
	}
	HistoBook* HistoBook::set( string opt, vector<string> params ){

		//TODO : need to fix the logger + cout 
		logger->info( __FUNCTION__ ) << "( " << opt << ", ";
		for ( int i = 0; i < params.size(); i++ ){
			logger->info( "", false ) << params[ i ] ;
			if ( i != params.size() - 1 )
				logger->info( "", false ) << ", ";
		}
		logger->info( "", false ) << " ) " << endl;

		// force the param name to lowercase
		transform(opt.begin(), opt.end(), opt.begin(), ::tolower);

		logger->debug( __FUNCTION__ ) << "lowercase option : " << opt << endl;

	    TH1* h = get( styling );
	    if ( h ){
	    	logger->debug(__FUNCTION__ ) << "Histogram " << styling << " is good " << endl;
		    if ( "title" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting title " << endl;
		    	h->SetTitle( cParam(params, 0) );
		    } else if ( "x" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting x " << endl;
		    	h->GetXaxis()->SetTitle( cParam(params, 0) );
		    } else if ( "y" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting y " << endl;
		    	h->GetYaxis()->SetTitle( cParam(params, 0) );
		    } else if ( "xaxisfontsize" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting x axis font size " << endl;
		    	h->GetXaxis()->SetLabelSize( dParam(params, 0) );
		    } else if ( "legend" == opt ){ 
		    	logger->debug(__FUNCTION__) << "Setting legend 1 " << endl;
		    	legend->AddEntry( h, cParam(params, 0), cParam(params, 1, "lpf") );
				legend->Draw();
		    } else if ( "draw" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting draw opt " << endl;
		    	drawOption = cParam(params, 0);
		    } else if ( "linecolor" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting linecolor " << endl;
		    	int c = color( cParam( params, 0) );
		    	if ( c  < 0 )
		    		c = (int) dParam( params, 0);
		    	h->SetLineColor( c );
		    } else if ( "fillcolor" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting fillcolor " << endl;
		    	int c = color( cParam( params, 0) );
		    	if ( c  < 0 )
		    		c = (int) dParam( params, 0);
		    	h->SetFillColor( c );
		    } else if ( "linewidth" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting linewidth " << endl;
		    	h->SetLineWidth( dParam( params, 0) );
		    } else if ( "domain" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting domain " << endl;
		    	double min = dParam( params, 0);
		    	double max = dParam( params, 1);
			    h->GetXaxis()->SetRangeUser( min, max );
		    } else if ( "dynamicdomain" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting dynamicdomain " << endl;
		    	double thresh = dParam( params, 0);
		    	int min = (int)dParam( params, 1);
		    	int max = (int)dParam( params, 2);
		    	int axis = (int)dParam( params, 3);		// 1 = x, 2 = y

		    	if ( 1 != axis && 2 != axis )
		    		axis = 1;

		    	if ( thresh >= 0) {
		    		if ( -1 >= min )
		    			min = h->FindFirstBinAbove( thresh, axis );
		    		if ( -1 >= max )
		    			max = h->FindLastBinAbove( thresh, axis );
		    	}

		    	if ( 1 == axis )
			  	  h->GetXaxis()->SetRange( min, max );
			  	else if ( 2 == axis )
			  		h->GetYaxis()->SetRange( min, max );

		    }  else if ( "range" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting range " << endl;
		    	double min = dParam( params, 0);
		    	double max = dParam( params, 1);

		    	h->GetYaxis()->SetRangeUser( min, max );
		    } else if ( "markercolor" == opt ) {
		    	logger->debug(__FUNCTION__) << "Setting markercolor " << endl;
		    	int c = color( cParam( params, 0) );
		    	if ( c  < 0 )
		    		c = (int) dParam( params, 0);
		    	h->SetMarkerColor( c );
		    } else if ( "markerstyle" == opt ) {
		    	logger->debug(__FUNCTION__) << "Setting markerstyle " << endl;
		    	h->SetMarkerStyle( (int)dParam( params, 0) );
		    } else if ( "markersize" == opt){
		    	logger->debug(__FUNCTION__) << "Setting markersize " << endl;
		    	h->SetMarkerSize( dParam( params, 0) );
		    } else if ( "legendalignment" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting legend " << endl;
		    	// p1 - alignmentX
		    	// p2 - alignmentY
		    	// p3 - width
		    	// p4 - height

		    	// make sure option is valid
		    	double p1 = dParam( params, 0);
		    	double p2 = dParam( params, 1);
		    	if ( !(legendAlignment::center == p1 || legendAlignment::left == p1 || legendAlignment::right == p1) )
		    		p1 = legendAlignment::best;
		    	if ( !(legendAlignment::center == p2 || legendAlignment::top == p2 || legendAlignment::bottom == p2) )
		    		p2 = legendAlignment::best;
		    	placeLegend( p1, p2, dParam( params, 3), dParam( params, 3) );
		    } else if ( "legendfontsize" == opt ) {
		    	legend->SetTextSize( dParam( params, 0 ) ); 
			}else if ( "numberofticks" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting numberofticks " << endl;
		    	// p1 - # of primary divisions
		    	// p2 - # of secondary divisions
		    	// p3 - axis : 0 or 1 = x, 2 = y
		    	double p1 = dParam( params, 0);
		    	double p2 = dParam( params, 1);
		    	double p3 = dParam( params, 2);

		    	if ( p2 == -1 )
		    		p2 = 0;

			    if ( 2 == (int)p3 )
			    	h->GetYaxis()->SetNdivisions( (int) p1, (int) p2, 0, true );
			    else
			    	h->GetXaxis()->SetNdivisions( (int) p1, (int) p2, 0, true );
		    } else if ( "logy" == opt ){
		    	logger->debug(__FUNCTION__) << "Setting log y " << endl;
		    	gPad->SetLogy( (int)dParam( params, 0 ) );
		    	
		    } else if ( "logx" == opt ){
		    	gPad->SetLogx( (int)dParam( params, 0 ) );
		    } else if ( "logz" == opt ){
		    	gPad->SetLogz( (int)dParam( params, 0 ) );
		    } else if ( "xoffset" == opt ){
		    	h->GetXaxis()->SetTitleOffset( dParam( params, 0 ) );
		    } else if ( "yoffset" == opt ){
		    	h->GetYaxis()->SetTitleOffset( dParam( params, 0 ) );
		    }



		} else {
			logger->warn(__FUNCTION__ ) << "Histogram " << styling << " is no good. " << endl;
		}

		return this;

	}
	HistoBook * HistoBook::set( string nodeName ){
		if ( config )
			set( config, nodeName );
		return this;
	}

	HistoBook* HistoBook::draw(string name, Option_t* opt ){


		// no parameters
		if ( name == ""){
			TH1* h = get( styling );
			if ( h ){
				// use the draw option set in its styling
				h->Draw( drawOption.c_str() );
				logger->info(__FUNCTION__) << styling << "->Draw( " << drawOption << " ) " << endl;
				drawOption = "";
			} else
				logger->warn(__FUNCTION__) << styling << " does not exist " << endl;
		} else {
			TH1* h = get( name );
			if ( h ){
				h->Draw( opt );
				logger->info(__FUNCTION__) << "Drawing " << name << endl;
			} else
				logger->warn(__FUNCTION__) << name << " does not exist " << endl;
		}

		return this;
	}


	HistoBook* HistoBook::placeLegend( int alignmentX, int alignmentY, double width, double height ){


		logger->info(__FUNCTION__) << "Placing Legend" << endl;
		double mR = 1 - gPad->GetRightMargin();
		double mL = gPad->GetLeftMargin();
		double mT = 1- gPad->GetTopMargin();
		double mB = gPad->GetBottomMargin();

		double x1, x2, y1, y2;

		if ( width <= 0 || width > 1 )
			width = .2;
		if ( height <= 0 || height > 1 )
			height = .2;

		// alignment best needs a current histo
		if ( !(get( styling )) ){
			if ( legendAlignment::best == alignmentX )
				alignmentX = legendAlignment::right;
			if ( legendAlignment::best == alignmentY )
				alignmentY = legendAlignment::top;
		} else {

			//TODO

		}


		if ( 	legendAlignment::left ==  alignmentX ){
			x1 =  mL ;
			x2 =  mL + width;
		}
		if ( 	legendAlignment::right ==  alignmentX ){
			x1 =  mR - width;
			x2 =  mR ;
		}
		if ( 	legendAlignment::center ==  alignmentX ){
			x1 =  0.55 - width/2.0;
			x2 =  0.55 + width/2.0;
		}
		if ( 	legendAlignment::top ==  alignmentY ){
			y1 =  mT - height;
			y2 = mT ;
		}
		if ( 	legendAlignment::bottom ==  alignmentY ){
			y1 =  mB ;
			y2 =  mB + height;
		}
		if ( 	legendAlignment::center ==  alignmentY ){
			y1 =  0.55 - height/2.0;
			y2 =  0.55 + height/2.0;
		}
		legend->SetX1NDC( x1 );
		legend->SetX2NDC( x2 );
		legend->SetY1NDC( y1 );
		legend->SetY2NDC( y2 );

		return this;
	}

} // jdb namespace
