#define UNITTEST_NO_CXX
#include "../UnitTest.h"

void ut(){

	using namespace jdb;
	Logger::setGlobalLogLevel( "trace" );
	Logger::setGlobalColor( true );

	UT_START( "HistoBins" );
	
    

    vector<double> bins = { 0, 1.2, 5.0, 6.0, 9.0 };
    HistoBins hb( bins );
    INFO( "Created : " << hb.toString()  );
    double val = 5.0;
    INFO( UT_EQ( hb.findBin( val ), 2) << " : findBin( " << val <<" )" );
    INFO( UT_EQ( hb.findBin( val, BinEdge::upper ), 1) << " : findBin( " << val <<" ) using BinEdge::upper"  );

    val = 0.0;
    INFO( UT_EQ( hb.findBin( val ), 0) << " : findBin( " << val <<" )"  );


    INFO( "Testing Overflow : " );
    val = 1000;
    INFO( UT_EQ( hb.findBin( val ), -2) << " : findBin( " << val <<" )" );

    INFO( "Testing Underflow : "  );
    val = -100;
    INFO( UT_EQ( hb.findBin( val ), -1) << " : findBin( " << val <<" )" );

    INFO( endl << "Testing Creating From Config : " );
    jdb::XmlConfig config( "cHistoBins.xml" );

    HistoBins hbc1( config, "pt" );
    INFO( hbc1.toString() );
    INFO( UT_EQ( hbc1.nBins(), 10) << " : Created bins from config.pt" );

    HistoBins hbc2( config, "p" );
    INFO( hbc2.toString() );
    INFO( UT_EQ( hbc2.nBins(), 100) << " : Created bins from config.p" );

    HistoBins hbc3( config, "eta" );
    INFO( hbc3.toString() );
    INFO( UT_EQ( hbc3.nBins(), 500) << " : Created bins from config.eta" );

    HistoBins hbc4( config, "var" );
    INFO( hbc4.toString() );
    INFO( UT_EQ( hbc4.nBins(), 7) << " : Created bins from config.var" );
    val = 12.0;
    INFO( UT_EQ( hbc4.findBin( val ), 4) << " : findBin( " << val <<" )"  );
    val = 7.0;
    INFO( UT_EQ( hbc4.findBin( val, BinEdge::upper ), 0) << " : findBin( " << val <<" ) == 0 (BinEdge::upper)"  );
    val = 7.0;
    INFO( UT_EQ( hbc4.findBin( val, BinEdge::lower ), 1) << " : findBin( " << val <<" ) == 1 (BinEdge::lower)"  );


    INFO( UT_SUMMARY );
   
}

