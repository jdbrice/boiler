#define UNITTEST_NO_CXX
#include "../UnitTest.h"

void ut(){

	using namespace jdb;
	Logger::setGlobalLogLevel( "trace" );
	Logger::setGlobalColor( true );

	UT_START( "HistoAnalyzer" );

	XmlConfig config( "config.xml" );

	HistoAnalyzer ta;
	ta.init( config, "HistoAnalyzer", -1 );

	ta.listFiles();

	INFO( "ut", UT_SUMMARY );


}