//
// Created by James Brandenburg on 6/4/15.
//

#ifndef ROOBARB_ROOPLOTLIB_H
#define ROOBARB_ROOPLOTLIB_H

// ROOBARB
#include "XmlCanvas.h"
#include "Utils.h"

// STL
#include <string>
#include <initializer_list>
using namespace std;


// ROOT
#include "TH1.h"
#include "TGraph.h"


namespace jdb{
	class RooPlotLib {

	protected:

	    map<string, shared_ptr<XmlCanvas> > canvas;

	    //string type = "";
	    TObject * styling = nullptr;
	    string drawOption = "";

	    bool validObject( TObject * obj );


	public:

	    RooPlotLib ();
	    ~RooPlotLib();


	    RooPlotLib &style( TObject * );

	    
	    RooPlotLib &set( string opt, float p0 = -999, float p1 = -999, float p2 = -999, float p3 = -999 ){
	    	return set( opt, { dts(p0), dts(p1), dts(p2), dts(p3) } );
	    }
	    RooPlotLib &set( string, initializer_list<string> l );
	    RooPlotLib &set( string, vector<string> l );
	    RooPlotLib &set( string opt, string p0 ="", string p1 ="", string p2 ="", string p3 =""){
	    	return set( opt, { p0, p1, p2, p3 } );
	    }
	    RooPlotLib &set( XmlConfig * cfg, string np );

	    /**
	     * Draws the object being styled
	     * @return 		reference to RooPlotLib object for method chaining
	     */
	    RooPlotLib &draw();


	    int color( string color );
	    

	};
}

#endif //ROOBARB_ROOPLOTLIB_H
