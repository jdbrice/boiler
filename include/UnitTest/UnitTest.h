//
// Created by James Brandenburg on 5/30/15.
//

#ifndef ROOBARB_UNITTEST_H
#define ROOBARB_UNITTEST_H

// ROOBARB
#include "Utils.h"
using namespace jdb;

// STL
#include <string>
using namespace std;

// ROOT
#include "TNamed.h"

#define UT_EQ( x, y ) UnitTest::eq( x, y )
#define UT_R_NAMED( x, y ) UnitTest::root_named( x, y )

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
    static string nn( void * val ){
        // 033[1;32m
        // 033[0;m

        if ( NULL == val || nullptr == val )
            return  "NULL/nullptr FAIL";
        else {
            return "Valid Pointer PASS";
            
        }
    }

    static string root_named( TNamed * obj, string name ){
        if ( NULL == obj || nullptr == obj )
            return nn( obj );

        if ( obj->GetName() == name )
            return "Object " + name + " PASS";
        else
            return "Object " + string(obj->GetName()) + " != " + name + " FAIL";
    }


};


#endif //ROOBARB_UNITTEST_H
