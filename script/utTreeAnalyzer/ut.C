


{

	using namespace jdb;
	Logger::setGlobalLogLevel( "all" );
	Logger::setGlobalColor( true );


	XmlConfig config( "config.xml" );

	TreeAnalyzer ta( config, "TreeAnalyzer", 2 );


}