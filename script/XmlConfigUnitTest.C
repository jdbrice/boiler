

void XmlConfigUnitTest( ){

	gSystem->Load( "libJDB.so" );
	using namespace jdb;

	// create the XML config object
	XmlConfig cfg( "cXmlConfigUnitTest.xml" );

	// setup a logger for some output to the screen
	Logger::setGlobalLogLevel( Logger::llAll );
	Logger l;
	

	// Access basic variables in the config
	// First lets get a string
	//
	l.info() << "Any node value can be retrieved as a string, int, double, or bool" << endl;
	l.info() << "First lets get a string:" << endl;
	l.info() << "Getting cfg.getString(\"basic.var1\") : " << cfg.getString("basic.var1" ) << endl;
	l.info() << "Or you can use the [] operator: " << "Getting cfg[ \"basic.var1\" ] : " << cfg[ "basic.var1" ] << endl;

	// Now get an integer
	l.info() << "Now an integer: " << endl;
	l.info() << "Getting cfg.getInt(\"basic.var2\") : " << cfg.getInt("basic.var2" ) << endl;

	// Now get a double
	l.info() << "Now a double: " << endl;
	l.info() << "Getting cfg.getDouble(\"basic.var3\") : " << cfg.getDouble("basic.var3" ) << endl;

	// Now get a bool
	l.info() << "Now a boolean: " << endl;
	l.info() << "Getting cfg.getBool(\"basic.var4\") : " << cfg.getBool("basic.var4" ) << endl;
	l.info() << "Note: " << endl;
	l.info() << "Bools can be stored as either an integer 0 (false) or 1 ( true) or as a string \"true\" or \"false\" (the case doesnt matter)" << endl;


	l.info() << "Getting paths to nodes under manySame.Histo that have a name attribute : " << endl;
	l.info() << "cfg.getNodes( \"manySame.Histo:name\") : " << endl;
	vector<string> cs = cfg.getNodes( "manySame.Histo:name");
	for ( int i = 0; i < cs.size(); i++ ){
		l.info() << cs[ i ] << endl;
	}

	l.info() << "Searching for nodes under manySame.Histo where name=h1 ( Should find 1 )" << endl;
	cs = cfg.getNodes( "manySame.Histo:name = h1");
	
	for ( int i = 0; i <cs.size(); i++ ){
		l.info() << cs[ i ] << endl;
	}

	l.info() << "Finding Attributs" << endl;
	cs = cfg.attributesOf( "withAttr.vars" );
	for ( int i = 0; i <cs.size(); i++ ){
		l.info() << cs[ i ] << " = " << cfg.getString( cs[ i ] ) << endl;
	}

}