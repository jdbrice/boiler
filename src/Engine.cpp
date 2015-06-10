#include <string>

#include "UnitTest/TreeAnalyzerUnitTest.h"
#include "UnitTest/HistoBinsUnitTest.h"

#include "XmlConfig.h"
#include "XmlCanvas.h"
#include "XmlPad.h"
using namespace jdb;

#include "TH1D.h"

int main( int argc, char* argv[] ){


    // try loading a config in from the first argument
	if ( argc >= 2 ){
        XmlConfig cfg( argv[ 1 ] );


        string job = cfg.getString( "job", "default" );
        if ( "TreeAnalyzerUnitTest" == job ){
            TreeAnalyzerUnitTest taut;
            taut.test("", argc, argv);
        } else if ("HistoBinsUnitTest" == job){
            HistoBinsUnitTest hbut;
            hbut.test( "General", argc, argv );
        } else if ( "XmlCanvasUnitTest" == job ){
            Logger::setGlobalLogLevel( Logger::llAll );
            XmlCanvas can( &cfg, "Canvas" );

            can.saveImage("prePad.pdf");

            can.activatePad("pad1");
            TH1D * h2 = new TH1D( "h2", "h2", 100, -10, 10 );
            h2->FillRandom("gaus", 10000);
            h2->Draw();

            can.saveImage("pad1.pdf");

            can.activatePad("pad2");
            TH1D * h = new TH1D( "h", "h", 100, -10, 10 );
            h->FillRandom("gaus", 10000);
            h->Draw();

            can.activatePad("pad3");
            h->Draw();

            can.saveImage("afterPad.pdf");

        }





    }



	/*
	Logger::setGlobalLogLevel( Logger::llAll );
	Logger l;
	l.setLogLevel( Logger::llAll );
	XmlConfig cfg( argv[ 1 ] );

	Reporter rp( "rp.pdf" );
	HistoBook * book = new HistoBook( "outHistoBook.root", &cfg );

	book->makeAll( "histograms" );

	book->get( "events" )->FillRandom( "gaus" );

	book->style( "events" )->set( "styles.Style" )->draw();

	rp.savePage();


	float x[] = { 1, 2, 3, 4};
	float y[] = { 1, 4, 9, 16};

	TGraph * g = new TGraph(4, x, y);
	g->Draw();

	book->add( "graph", g );

	delete book;*/
}