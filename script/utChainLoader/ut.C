
#define UNITTEST_NO_CXX
#include "../UnitTest.h"

void ut(){

	using namespace jdb;
	Logger::setGlobalLogLevel( "info" );
	Logger::setGlobalColor( true );


	TChain * c = new TChain( "V0PicoDst" );

	ChainLoader::loadListRange( c, "list.lis", 0, 10 );

	UT_START( "ChainLoader" );

	INFO( "ut", UT_EQ( c->GetNtrees(), 10 ) << " Load List Range" );

	INFO( "ut", UT_SUMMARY );
	// TODO: Add unit tests for the other chain loading functions
	
	// INFO( "ut", UT_EQ( c->GetNtrees(), 10 ) << " Load List Range" );

}