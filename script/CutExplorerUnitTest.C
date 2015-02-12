

void CutExplorerUnitTest( string fname = "cCutExplorerUnitTest.xml" ){

	cout << "Unit Test : CutExplorer" << endl;

	XmlConfig cfg( fname );
	

	CutExplorer ce( &cfg, "CutExplorer." );

	ce.make();
	//Logger::setGlobalLogLevel( Logger::llAll );
	//CutSequence cs( &cfg, "CutExplorer.CutSequence[ 0 ]" );

}