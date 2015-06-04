#include <string>

#include "UnitTest/TreeAnalyzerUnitTest.h"
#include "UnitTest/HistoBinsUnitTest.h"

#include "XmlConfig.h"
using namespace jdb;

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