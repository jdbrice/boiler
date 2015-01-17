

void CutExplorerUnitTest( string fname = "cCutExplorerUnitTest.xml" ){

	cout << "Unit Test : CutExplorer" << endl;

	gSystem->Load( "libJDB.so" );
	using namespace jdb;
	
	XmlConfig cfg( fname );
	
	Logger::setGlobalLogLevel( Logger::llAll );

	
	TChain * chain = new TChain( cfg.getString( "input.dst:treeName" ).c_str() );
	ChainLoader::load( chain, cfg.getString( "input.dst:url" ), cfg.getInt( "input.dst:maxFiles", -1 ) );

	

	TreeMap tm( chain );
	tm.getEntry( 0 );

	int nEvents = tm.getEntries();

	cout << "Setting up CutVariable" << endl;

	CutVariable cv( &tm, &cfg, "CutSequence[0].CutVariable" );

	cout << "vertexR = " << cv.eval() << endl;

	HistoBook book( "ce.root", &cfg );
	book.make1D( "vR", "vR", 100, 0, 10 );

	TaskTimer tt;
	tt.start();
	for ( int i = 0; i < nEvents; i++ ){

		tm.getEntry( i );
		//cout << "vertexR = " << cv->eval() << endl;
		book.fill( "vR", cv.eval() );


	}

	cout << "Finished in " << tt.elapsedTime() << endl;




}