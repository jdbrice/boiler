

TreeAnalyzerUnitTest( string fname = "cTreeAnalyzerUnitTest.xml" ){
	
	gSystem->Load( "libJDB.so" );
	using namespace jdb;
	XmlConfig cfg( fname );
	TreeAnalyzer ta( &cfg, "TreeAnalyzer." );

	ta.make();

}