

void unitTest_XmlConfig( ){

	gSystem->Load( "../lib/libJDB.so" );

	using namespace jdb;
	
	Logger::setGlobalLogLevel( Logger::llAll );
	XmlConfig cfg( "config.xml" );


	vector<string> cs = cfg.getNodes( "unitTest_XmlConfig.manySame.Histo:name");
	
	for ( int i = 0; i <cs.size(); i++ ){
		cout << cs[ i ] << endl;
	}

	cout << "\nSearching for nodes where :name=h1" << endl;
	cs = cfg.getNodes( "unitTest_XmlConfig.manySame.Histo:name = h1");
	
	for ( int i = 0; i <cs.size(); i++ ){
		cout << cs[ i ] << endl;
	}
}