

void TreeMapUnitTest( string fname = "cTreeMapUnitTest.xml" ){

	gSystem->Load( "libJDB.so" );
	using namespace jdb;
	
	XmlConfig cfg( fname );

	Logger::setGlobalLogLevel( Logger::llAll );

	TChain * chain = new TChain( cfg.getString( "input.dst:treeName" ).c_str() );
	ChainLoader::load( chain, cfg.getString( "input.dst:url" ), cfg.getInt( "input.dst:maxFiles", -1 ) );

	TreeMap tm( chain );

	int nEvents = tm.getEntries();

	HistoBook book( "dTreeMapUnitTest.root", &cfg );
	book.make1D( "pt", "pt [GeV]", 150, 0, 7.5 ); 

	TaskTimer tt;
	tt.start();
	for ( int i = 0; i < nEvents; i++ ){

		tm.getEntry( i );

		int nTof = tm[ "nTofHits" ];
		//cout << "nTofHits = " << tm[ "nTofHits" ] << endl;
		for ( int j = 0; j < nTof; j++ ){
			int c = tm.get( "charge", j );
			int match = tm.get( "matchFlag", j );
			int nHits = tm.get( "nHits", j );
			int nHitsFit = tm.get( "nHitsFit", j );
			float pt = tm.get( "pt", j );
			book.fill( "pt", pt );
			//cout << " pt " << pt << endl;
			//cout << "charge = " <<  << endl;
		}

	}

	cout << "Finished in " << tt.elapsedTime() << endl;




}