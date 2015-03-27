#include "Logger.h"


namespace jdb {
	int Logger::llGlobal = Logger::llDefault;	
	bool Logger::showColors = false;
	Logger Logger::log;

	void Logger::setGlobalLogLevel( int ll ){
		Logger::llGlobal = ll;
	}

	int Logger::getGlobalLogLevel() { return llGlobal; }

	void Logger::setGlobalColor( bool state  ){
		Logger::showColors = state;
	}
	bool Logger::getGlobalColor( ) { return Logger::showColors; }
}
