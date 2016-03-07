#define UNITTEST_NO_CXX
#include "../UnitTest.h"

void ut(){

	using namespace jdb;
	Logger::setGlobalLogLevel( "trace" );
	Logger::setGlobalColor( true );

	UT_START( "XmlConfig" );
	jdb::XmlConfig config( "cXmlConfigUnitTest.xml" );

	map<string, string> m = config.attributesMap( "Analyzer1" );

	INFO( "ut", UT_EQ( m[ "name" ], "MakerOfThings" ) << " : \"name\" attribute" );
	INFO( "ut", UT_TRUE( config.exists( "Analyzer1.Items" ) ) << " : Includes" );
	INFO( "ut", UT_EQ( config.getIntVector( "Analyzer1.Items" ).size(), 16 ) << " : Included vector size" );
	if ( UT_PASSED ){
		INFO( "ut", UT_EQ( config.getIntVector( "Analyzer1.Items" )[4], 45 ) << " : Included vector data integrity" );
	} 

	for ( auto k : m ){
		cout << k.first << " = " << k.second << endl;
	}

	INFO( "ut", UT_SUMMARY );



}