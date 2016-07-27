#define UNITTEST_NO_CXX
#include "../UnitTest.h"
#include "HistoBins.h"


void ut(){

	using namespace jdb;
	Logger::setGlobalLogLevel( "all" );
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
    INFO( "", "filename " << config.getFilename() );

    // XmlConfig cfg2 = config;
    HistoBins hbc1( config, "pt" );
    INFO( hbc1.toString() );
    INFO( UT_EQ( hbc1.nBins(), 10) << " : Created bins from config.pt" );

    HistoBins hbc2( config, "p" );
    INFO( hbc2.toString() );
    INFO( UT_EQ( hbc2.findBinWidth( 0.5 ), 0.32323) << " : Created bins from config.p" );

    HistoBins hbc3( config, "eta" );
    INFO( hbc3.toString() );
    INFO( UT_EQ( hbc3.nBins(), 500) << " : Created bins from config.eta" );

    HistoBins hbc4( config, "var" );
    INFO( hbc4.toString() );
    INFO( UT_EQ( hbc4.nBins(), 7) << " : Created bins from config.var" );
    val = 12.0;
    INFO( UT_EQ( hbc4.findBin( val ), 4) << " : findBin( " << val <<" ) == 4"  );
    val = 7.0;
    INFO( UT_EQ( hbc4.findBin( val, BinEdge::upper ), 0) << " : findBin( " << val <<" ) == 0 (BinEdge::upper)"  );
    val = 7.0;
    INFO( UT_EQ( hbc4.findBin( val, BinEdge::lower ), 1) << " : findBin( " << val <<" ) == 1 (BinEdge::lower)"  );

    INFO( UT_EQ( hbc4.findBinWidth( val, BinEdge::lower ), 1.0) << " : findBinWidth( " << val <<" ) == 1 (BinEdge::lower)"  );
    INFO( UT_EQ( hbc4.findBinWidth( val, BinEdge::upper ), 6.0) << " : findBinWidth( " << val <<" ) == 6 (BinEdge::upper)"  );

    // test quantile binning
    vector<double> fake_data = { 12.8184,14.3276,14.7087,14.9422,15.1128,15.2452,15.3533,15.4437,15.5195,15.5914,15.6597,15.7212,15.7772,15.8314,15.8848,15.9375,15.9884,16.0325,16.0764,16.1228,16.1683,16.2153,16.2633,16.3123,16.3648,16.4192,16.4827,16.556,16.6373,16.7267,16.8191,16.9147,17.0166,17.1345,17.2864,17.5018,17.9791,21.2528,26.8747,33.149,12.9883,14.4463,14.8357,15.0682,15.2352,15.3667,15.4724,15.561,15.6381,15.7081,15.7751,15.8412,15.9059,15.9622,16.0138,16.065,16.1145,16.1657,16.2112,16.2564,16.3044,16.3498,16.3953,16.4422,16.4942,16.5495,16.612,16.682,16.7563,16.8333,16.9167,17.0034,17.0943,17.1911,17.314,17.5039,17.7909,19.4254,24.3619,32.0101,12.7136,14.365,14.7583,14.9931,15.1571,15.2859,15.3876,15.4742,15.5439,15.6108,15.67,15.7288,15.783,15.8355,15.8853,15.939,15.9893,16.0384,16.0922,16.144,16.1965,16.2523,16.3136,16.384,16.4721,16.5717,16.6813,16.7942,16.9055,17.0203,17.1389,17.2706,17.405,17.5544,17.7179,17.9325,18.4676,21.3381,27.0015,33.3364 };
    INFO( "ut", vts(HistoBins::makeQuantileBins( fake_data, 10 )) );


    INFO( UT_SUMMARY );
   
}

