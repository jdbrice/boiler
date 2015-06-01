#include <string>


#include "HistoBook.h"
#include "Reporter.h"
using namespace jdb;

#include "TGraph.h"

#include "UnitTest/TreeAnalyzerUnitTest.h"

int main( int argc, char* argv[] ){

	TreeAnalyzerUnitTest taut;
	taut.test("", argc, argv);

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