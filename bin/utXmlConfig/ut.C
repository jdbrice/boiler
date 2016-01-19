



void ut(){

	jdb::XmlConfig config( "cXmlConfigUnitTest.xml" );


	map<string, string> m = config.attributesMap( "Analyzer1" );

	for ( auto k : m ){
		cout << k.first << " = " << k.second << endl;
	}



}