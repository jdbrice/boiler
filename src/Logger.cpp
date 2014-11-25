#include "Logger.h"


namespace jdb {
	int Logger::llGlobal = Logger::llDefault;	

	void Logger::setGlobalLogLevel( int ll ){
		Logger::llGlobal = ll;
	}

	int Logger::getGlobalLogLevel() { return llGlobal; }
}
