

void test( ){

	gSystem->Load( "lib/libJDB.so" );

	using namespace jdb;
	
	Logger::setGlobalLogLevel( Logger::llAll );
	XmlConfig cfg( "config.xml" );


	HistoBook * book = new HistoBook( "out.root", &cfg );

	book->makeAll( "kong.histograms" );

	delete book;

}