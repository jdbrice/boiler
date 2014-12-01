
/*

Problem drawing to screen!
 */
void HistoBookUnitTest( string fname = "cHistoBookUnitTest.xml" ){

	gSystem->Load( "libJDB.so" );
	using namespace jdb;
	
	Logger::setGlobalLogLevel( Logger::llAll );
	Logger l;
	XmlConfig cfg( fname );

	Reporter rp( "rp.pdf" );
	HistoBook * book = new HistoBook( "outHistoBook.root", &cfg );

	book->makeAll( "histograms" );

	book->get( "events" )->FillRandom( "gaus" );

	book->style( "events" )->set( "styles.Style" )->draw();

	rp.savePage();

	delete book;

}