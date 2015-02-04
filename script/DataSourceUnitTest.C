

void DataSourceUnitTest( string fname = "cDataSourceUnitTest.xml" ){

	gSystem->Load( "libJDB.so" );
	using namespace jdb;
	
	XmlConfig cfg( fname );

	Logger::setGlobalLogLevel( Logger::llInfo );

	DataSource ds( &cfg, "DataSource" );

	TChain * chain = ds.getChain();
	

	HistoBook book( "lbnl.root" );
	book.make1D( "vertexR", "vertexR", 200, 0, 2 );

	cout << "Getting Entries" << endl;
	int nEvents = ds.getEntries();

	for ( int i = 0; i < nEvents; i++ ){
		//cout << " Getting Entry " << i << endl; 
		ds.getEntry( i );
		book.fill( "vertexR", ds( "vR" ) );

	}

}