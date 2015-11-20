

#include "HistoBook.h"
#include "Logger.h"

// ROOT
#include "TKey.h"
#include "TObject.h"
#include <TApplication.h>

#ifdef __CINT__
ClassImp( jdb::HistoBook );
#endif

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
		this->filename = name;
		this->config = nullptr;
		this->inputFilename = input;
		this->inputDir = inDir;

		initialize();
	}	// Constructor

	/**
	 * Constructor that allows th input of a config file
	 * @param name name of file to use for saving root data
	 * @param con  The config file to use for all config relates calls
	 */
	HistoBook::HistoBook( string name, XmlConfig * con, string input, string inDir){
		this->filename = name;
		this->config = con;
		this->inputFilename = input;
		this->inputDir = inDir;

		initialize();
	}	// Constructor

	void HistoBook::initialize(){
		INFO( tag, "" );

		// append .root to end if needed
		if (this->filename.find(  ".root") == std::string::npos){
			this->filename = this->filename + ".root";
		}

		// open the output file
		this->file = new TFile( this->filename.c_str(), "recreate" );
		this->file->cd();

		if ( this->file->IsOpen() ){
			INFO( tag, "Output File : " << this->filename << " opened" );
		} else {
			ERROR( tag, "Output File : " << this->filename << " not opened" );
		}

		// set our default global style
		globalStyle();
		
		// if an input was given merge it into the live record
		mergeIn( this->inputFilename, this->inputDir );

		this->currentDir = "";

		// default to auto save on exit
		saveOnExit( true );
	}


	/**
	 * Merges in a root file
	 * 
	 * @param _filename Filename of root file to load
	 * @param _dir      Optional: sub directory to load
	 */
	void HistoBook::mergeIn( string _filename, string _dir ){
		INFO( tag, "(" << _filename << ", " << _dir << " )"  );

		if (_filename.find(  ".root") == std::string::npos){
			_filename = _filename + ".root";
		}

		if ( _filename.length() >= 6 ){
			INFO( tag, "Loading : " << _dir << " from " << _filename );
			
			TFile * fin = new TFile( _filename.c_str() );
			
			this->cd ( _dir );
			
			fin->cd( _dir.c_str() );
			loadRootDir( gDirectory, _dir );
		}
	}

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
		INFO( tag, "Shutting Down" );
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


	TH1 * HistoBook::make( string _type, string _name, string _title, HistoBins &_bx, HistoBins &_by, HistoBins &_bz ){
		DEBUG( tag, "type=" << _type << " name=" << _name << " title=" << _title << "bx, by, bz" );

		int nD = 0;
		if ( (_bx.nBins() > 0) && (_by.nBins() <= 0) && (_bz.nBins() <= 0) )
			nD = 1;
		if ( (_bx.nBins() > 0) && (_by.nBins() > 0) && (_bz.nBins() <= 0) )
			nD = 2;
		if ( (_bx.nBins() > 0) && (_by.nBins() > 0) && (_bz.nBins() > 0) )
			nD = 3;


		if ( 1 == nD ){
			if ( "C" == _type )
				return new TH1C( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data() );
			if ( "S" == _type )
				return new TH1S( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data() );
			if ( "I" == _type )
				return new TH1I( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data() );
			if ( "F" == _type )
				return new TH1F( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data() );
			if ( "D" == _type )
				return new TH1D( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data() );
		} // make 1Ds
		else if ( 2 == nD ){
			if ( "C" == _type )
				return new TH2C( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data() );
			if ( "S" == _type )
				return new TH2S( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data() );
			if ( "I" == _type )
				return new TH2I( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data() );
			if ( "F" == _type )
				return new TH2F( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data() );
			if ( "D" == _type )
				return new TH2D( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data() );
		} // make 2Ds
		else if ( 3 == nD ){
			if ( "C" == _type )
				return new TH3C( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data(), _bz.nBins(), _bz.bins.data() );
			if ( "S" == _type )
				return new TH3S( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data(), _bz.nBins(), _bz.bins.data() );
			if ( "I" == _type )
				return new TH3I( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data(), _bz.nBins(), _bz.bins.data() );
			if ( "F" == _type )
				return new TH3F( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data(), _bz.nBins(), _bz.bins.data() );
			if ( "D" == _type )
				return new TH3D( _name.c_str(), _title.c_str(), _bx.nBins(), _bx.bins.data(), _by.nBins(), _by.bins.data(), _bz.nBins(), _bz.bins.data() );
		} // make 2Ds

		return nullptr;
	}

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

			string type 	= config->getString( nodeName + ":type", "D" );
			string hTitle 	= config->getString( nodeName + ":title", hName );


			HistoBins* bx;
			HistoBins* by;
			HistoBins* bz;


			// make the HistBins object from the config path
			if ( config->exists( nodeName + ":xBins" ) )
				bx = new HistoBins( config, config->getString( nodeName + ":xBins" ) );
			else 
				bx = new HistoBins( config, nodeName, "x" );
			if ( config->exists( nodeName + ":yBins" ) )
				by = new HistoBins( config, config->getString( nodeName + ":yBins" ) );
			else 
				by = new HistoBins( config, nodeName, "y" );
			if ( config->exists( nodeName + ":zBins" ) )
				bz = new HistoBins( config, config->getString( nodeName + ":zBins" ) );
			else 
				bz = new HistoBins( config, nodeName, "z" );

			TH1 * tmp = make( type, hName, hTitle, (*bx), (*by), (*bz) );
			if ( nullptr != tmp ){
				add( hName, tmp );
			} else  {
				ERROR( tag, "could not make histogram : " << hName );
				ERROR( tag, "x bins : " << bx->toString() );
				ERROR( tag, "y bins : " << by->toString() );
				ERROR( tag, "z bins : " << bz->toString() );
				ERROR( tag, "histo : " << tmp );
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

	TH1* HistoBook::get( string name, string sdir  ){
		if ( sdir.compare("") == 0)
			sdir = currentDir;

		if ( NULL == book[ ( sdir + name  ) ] )
			WARN( tag, sdir + name  << " Does Not Exist " );

		return (TH1*)book[ ( sdir  + name  ) ];
	}	//get

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
