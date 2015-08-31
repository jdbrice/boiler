//
// Created by James Brandenburg on 5/30/15.
//

#ifndef ROOBARB_UNITTEST_H
#define ROOBARB_UNITTEST_H

#include "Utils.h"
using namespace jdb;

#include <string>
using namespace std;

#define UT_EQ( x, y ) UnitTest::eq( x, y )

class UnitTest{

public:
    // state of the last run op
    bool pass;

    static string eq( int life, int hope ){
        // 033[1;32m
        // 033[0;m

        if ( life == hope )
            return ts( life ) + " PASS";
        else {
            return  ts(life) + " != " + ts(hope) + " FAIL";
        }
    }
    static string nn( void ** val ){
        // 033[1;32m
        // 033[0;m

        if ( NULL == val || nullptr == val )
            return  "NULL/nullptr FAIL";
        else {
            return "Valid Pointer PASS";
            
        }
    }


};


#endif //ROOBARB_UNITTEST_H
