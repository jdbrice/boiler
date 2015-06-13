#include "RooPlotLib.h"


//#include "TROOT.h" 
#include "TStyle.h"

/*
 * Default Ctor
 * 
 */
jdb::RooPlotLib::RooPlotLib()  {
    DEBUG("");
	// create a default canvas for generic plotting
	//canvas[ "default" ] = shared_ptr<XmlCanvas>( new XmlCanvas( nullptr, "" ) );


}

/*
 * Dtor
 */
jdb::RooPlotLib::~RooPlotLib(){
    DEBUG("");
}

bool jdb::RooPlotLib::validObject( TObject * obj ){
    DEBUG("( " << obj << " )");
	if ( nullptr == obj  )
		return false;

	if ( obj->InheritsFrom( "TH1" ) || obj->InheritsFrom( "TGraph" ) )
		return true;
	return false;
}


RooPlotLib & jdb::RooPlotLib::style( TObject * obj) {
    DEBUG("( " << obj << " )");
	//if ( validObject( obj ) )
		styling = obj;
	//else 
	//	styling = nullptr;

	return *this;
}


RooPlotLib &jdb::RooPlotLib::set( string option, initializer_list<string> l ){
    DEBUG( "( " << option <<", " << l << " )" );
	vector<string> params( l.begin(), l.end() );
	return set( option, params );
}

/* sets visual styles 
 * @option 	the option to set
 * @l 		an initializer_list of parameters
 *
 * implements all of the options that expect a number type so that it can handle
 * styles loaded from configs etc.
 */
RooPlotLib &jdb::RooPlotLib::set( string option, vector<string> params ){
	DEBUG("( "<< option << " )")
	if ( nullptr == styling ){
		ERROR( "Invalid object" );
		return *this;
	}

	// force the param name to lowercase
	transform(option.begin(), option.end(), option.begin(), ::tolower);

	// if param gives a valid color from a string then keep it for later
	int colorFromString = color( params[ 0 ] );

	TH1 * h 	= dynamic_cast<TH1*>(styling);
	TGraph * g 	= dynamic_cast<TGraph*>(styling);


	// get the X axis
	TAxis * ax = nullptr;
	if ( nullptr != h ) ax = h->GetXaxis();
	if ( nullptr != g ) ax = g->GetXaxis();
	// get the y axis
	TAxis * ay = nullptr;
	if ( nullptr != h ) ay = h->GetYaxis();
	if ( nullptr != g ) ay = g->GetYaxis();

	// set some general properties
	if ( "title" == option || "t" == option ){
		if ( nullptr != h ) h->SetTitle( params[0].c_str() );
		if ( nullptr != g ) g->SetTitle( params[0].c_str() );
	}
	if ( "draw" == option ){
		drawOption = params[ 0 ];
	}

	// Axis Stuff
	if ( ("x" == option || "xtitle" == option) && nullptr != ax ){
		ax->SetTitle( params[ 0 ].c_str() );
	}
	if ( ("y" == option || "ytitle" == option ) && nullptr != ay ){
		ay->SetTitle( params[ 0 ].c_str() );
	}
	if ( ("xo" == option || "xoffset" == option ) && nullptr != ax ){
		ax->SetTitleOffset( atof(params[ 0 ].c_str()) );
	}
	if ( ("yo" == option || "yoffset" == option ) && nullptr != ay ){
		ay->SetTitleOffset( atof(params[ 0 ].c_str()) );
	}

	// Axis Stuff (number)
	if ( ("xrange" == option || "xr" == option ) && nullptr != ax ){
		ax->SetRangeUser( atof(params[ 0 ].c_str()), atof(params[ 1 ].c_str()) );
	}
	if ( ("xbinrange" == option || "xbr" == option ) && nullptr != ax ){
		ax->SetRange( atof(params[ 0 ].c_str()), atof(params[ 1 ].c_str()) );
	}
	if ( ("yrange" == option || "yr" == option ) && nullptr != ay ){
		ay->SetRangeUser( atof(params[ 0 ].c_str()), atof(params[ 1 ].c_str()) );
	}
	if ( ("ybinrange" == option || "ybr" == option ) && nullptr != ay ){
		ay->SetRange( atof(params[ 0 ].c_str()), atof(params[ 1 ].c_str()) );
	}

	// gPad Options
	if ( "logx" == option && nullptr != gPad )
		gPad->SetLogx( atoi( params[ 0 ].c_str() ) );
	if ( "logy" == option && nullptr != gPad )
		gPad->SetLogy( atoi( params[ 0 ].c_str() ) );
	if ( "logz" == option && nullptr != gPad )
		gPad->SetLogz( atoi( params[ 0 ].c_str() ) );

	if ( "stats" == option || "stat" == option || "optstat" == option){
		gStyle->SetOptStat( atoi( params[ 0 ].c_str() ) );
	}
	if ( "fitbox" == option || "fit" == option || "optfit" == option){
		gStyle->SetOptFit( atoi( params[ 0 ].c_str() ) );
	}


	// Line attributes
	TAttLine * line = dynamic_cast<TAttLine*>( styling );
	if ( line ){
		if ( "linecolor" == option || "lc" == option ){
			int c = atoi( params[ 0 ].c_str() );
			if ( colorFromString >= 0 )
				c = colorFromString;
			line->SetLineColor( c );

		}
		if ( "linewidth" == option || "lw" == option){
			line->SetLineWidth( atof( params[ 0 ].c_str() ) );
		}
	}

	// Fill attributes
	TAttFill * fill = dynamic_cast<TAttFill*>( styling );
	if ( fill ){
		if ( "fillcolor" == option || "fc" == option ){
			int c = atoi( params[ 0 ].c_str() );
			if ( colorFromString >= 0 )
				c = colorFromString;
			fill->SetFillColor( c );
		}
		if ( "fillcoloralpha" == option || "fca" == option ){
			int c = atoi( params[ 0 ].c_str() );
			if ( colorFromString >= 0 )
				c = colorFromString;
			fill->SetFillColorAlpha( c, atoi( params[ 1 ].c_str() ) );
		}
		if ( "fillstyle" == option || "fst" == option){
			fill->SetFillStyle( atoi( params[ 0 ].c_str() ) );
		}
	}

	// Marker attributes
	TAttMarker * marker = dynamic_cast<TAttMarker*>( styling );
	if ( marker ){
		if ( "markercolor" == option || "mc" == option ){
			int c = atoi( params[ 0 ].c_str() );
			if ( colorFromString >= 0 )
				c = colorFromString;
			marker->SetMarkerColor( c );
		}
		if ( "markersize" == option || "ms" == option ){
			marker->SetMarkerSize( atoi( params[ 0 ].c_str() ) );
		}
		if ( "markerstyle" == option || "mst" == option){
			marker->SetMarkerStyle( atoi( params[ 0 ].c_str() ) );
		}
	}


	return *this;

}

RooPlotLib &jdb::RooPlotLib::set( XmlConfig * cfg, string nodePath ){
	DEBUG("");

	// get the list of attributes and set the style from that
	vector< string > list = cfg->attributesOf( nodePath );
	for ( int i = 0; i < list.size(); i++ ){
		vector<string> params = cfg->getStringVector( list[ i ] );
		DEBUG( "attr : " << cfg->attributeName( list[ i ] )  );

		if ( 0 == params.size()  )
			params.push_back( "" );
		set( cfg->attributeName( list[ i ] ), params );
	}

	return *this;
}


RooPlotLib &jdb::RooPlotLib::draw(){
    DEBUG("");
	if ( nullptr == styling ){
		ERROR( "Invalid object" );
		return *this;
	}

	styling->Draw( drawOption.c_str() );
	return *this;
}



int jdb::RooPlotLib::color( string color ) {
    DEBUG("( " + color + " )");
	transform(color.begin(), color.end(), color.begin(), ::tolower);
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














