#define UNITTEST_NO_CXX
#include "../UnitTest.h"

void ut(){

	using namespace jdb;
	Logger::setGlobalLogLevel( "trace" );
	Logger::setGlobalColor( true );

	UT_START( "TreeAnalyzer" );

	XmlConfig config( "config.xml" );

	TreeAnalyzer ta;
	ta.init( config, "TreeAnalyzer", 2 );

	INFO( "ut", UT_SUMMARY );


}