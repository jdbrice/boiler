#define UNITTEST_NO_CXX
// #include "Utils.h"
#include "../UnitTest.h"

void ut(){
	gSystem->Load( "libRooBarb.dylib" );

	using namespace jdb;
	Logger::setGlobalLogLevel( "trace" );
	Logger::setGlobalColor( true );

	UT_START( "XmlString" );
	jdb::XmlConfig config( "config.xml" );

	// // map<string, string> m = config.attributesMap( "Analyzer1" );

	// // INFO( "ut", UT_EQ( m[ "name" ], "MakerOfThings" ) << " : \"name\" attribute" );
	// // INFO( "ut", UT_TRUE( config.exists( "Analyzer1.Items" ) ) << " : Includes" );
	// // INFO( "ut", UT_EQ( config.getIntVector( "Analyzer1.Items" ).size(), 16 ) << " : Included vector size" );
	// // if ( UT_PASSED ){
	// // 	INFO( "ut", UT_EQ( config.getIntVector( "Analyzer1.Items" )[4], 45 ) << " : Included vector data integrity" );
	// // } 

	// // for ( auto k : m ){
	// // 	cout << k.first << " = " << k.second << endl;
	// // }

	XmlString xstr;
	string fstr = xstr.format( config, config[ "XmlString:url" ] );
	INFO( "UT", UT_EQ( fstr, "DestroyerOfThings_hasComeAgain" ) );

	INFO( "ut", UT_SUMMARY );



}