#include <string>
#include <iostream>
#include "Logger.h"
using namespace jdb;

int main(){
	
	Logger l;
	l.info( ) << "Main" << endl;

	return 0;
}