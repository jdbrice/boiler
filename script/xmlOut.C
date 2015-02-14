

void xmlOut(){

	// create the XML config object
	XmlConfig cfg( "cXmlConfigUnitTest.xml" );

	// setup a logger for some output to the screen
	Logger::setGlobalLogLevel( Logger::llAll );
	Logger l;	


	cfg.saveXml("dan.xml");
}
