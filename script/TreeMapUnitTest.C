

void TreeMapUnitTest( string fname = "cTreeMapUnitTest.xml" ){

	gSystem->Load( "libJDB.so" );
	using namespace jdb;
	
	XmlConfig cfg( fname );

	Logger::setGlobalLogLevel( Logger::llAll );

	TChain * chain = new TChain( cfg.getString( "input.dst:treeName" ).c_str() );
	ChainLoader::load( chain, cfg.getString( "input.dst:url" ), cfg.getInt( "input.dst:maxFiles", -1 ) );

	TreeMap tm( chain );

	int nEvents = tm.getEntries();

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
			cout << " pt " << pt << endl;
			//cout << "charge = " <<  << endl;
		}

	}

	cout << "Finished in " << tt.elapsedTime() << endl;



}