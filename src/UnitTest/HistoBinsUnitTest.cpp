//
// Created by James Brandenburg on 6/3/15.
//

#include <vector>
#include "UnitTest/HistoBinsUnitTest.h"

#include "UnitTest/UnitTest.h"
#include "HistoBins.h"
#include "Logger.h"
using namespace jdb;

HistoBinsUnitTest::HistoBinsUnitTest(){


}

HistoBinsUnitTest::~HistoBinsUnitTest() {


}


bool HistoBinsUnitTest::test(string testName, int argc, char * argv[]) {

    Logger l( Logger::logLevelFromString( "all" ) );
    Logger::setGlobalLogLevel( Logger::llAll );

    l.info() << "Creating HistoBins" << endl;

    vector<double> bins = { 0, 1.2, 5.0, 6.0, 9.0 };

    HistoBins hb( bins );
    l.info() << "Created : " << hb.toString() << endl;
    double val = 5.0;

    string test = UT_EQ( hb.findBin( val ), 2);
    l.info() << "findBin( " << val <<" ) ==> " << test << endl;

    test = UT_EQ( hb.findBin( val, BinEdge::upper ), 1);
    l.info() << "findBin( " << val <<" ) ==> " << test << endl;

    val = 0.0;
    test = UT_EQ( hb.findBin( val ), 0);
    l.info() << "findBin( " << val <<" ) ==> " << test << endl;


    LINFO << "Testing Overflow : " << endl;
    val = 1000;
    test = UT_EQ( hb.findBin( val ), -2);
    l.info() << "findBin( " << val <<" ) ==> " << test << endl;

    l.info() << "Testing Underflow : " << endl;
    val = -100;
    test = UT_EQ( hb.findBin( val ), -1);
    l.info() << "findBin( " << val <<" ) ==> " << test << endl;

    l.info() << endl << "Testing Creating From Config : " << endl;
    DEBUG( UT_EQ( -2, -2) );

    return true;
}