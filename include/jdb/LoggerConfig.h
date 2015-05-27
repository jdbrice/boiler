#ifndef LOGGER_CONFIG_H
#define LOGGER_CONFIG_H


#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"

#include <string>
using namespace std;

namespace jdb{

	/* A class for creating Loggers from Xml Configs
	 * An example node:
	 * ``` xml
	 *  <Logger>
	 *  	<logLevel>info</logLevel>
	 *  </Logger>
	 * ```
	 */
	class LoggerConfig
	{
	public:
		/* The Class is meant to be used statically - dont call constructor directly
		 * 
		 */
		LoggerConfig(){  }
		// Destructor
		~LoggerConfig(){ }
		
		/* Creates a Logger from the given Xml Config and node
		 * @config   Xml Config
		 * @nodePath path to node
		 * @return   A logger instance with the given proerties or a default Logger if the node is not found or usable
		 */
		static Logger * makeLogger( XmlConfig * config, string nodePath ) {

			cout << "Making logger( " << config << ", " << nodePath << " ) " << endl;
			if ( config && config->getBool( nodePath+":color" ) )
				Logger::setGlobalColor( true );

			if ( config && config->exists( nodePath+":globalLogLevel" ) ){
				cout << "Setting globalLogLevel = " << config->getString( nodePath+":globalLogLevel" ) << endl;
				Logger::setGlobalLogLevel( Logger::logLevelFromString( config->getString( nodePath+":globalLogLevel" ) ) );
			}
			cout << "Looking in " << (nodePath + ":logLevel") << endl;
			string ll = config->getString( nodePath + ":logLevel", 
							config->getString( nodePath + ".logLevel", 
								"info" ) );
		
			cout << "LogLevel = " << ll << endl;
			return (new Logger( Logger::logLevelFromString( ll ) ) );
			
		
			//cout << "Making Default Logger Instance" << endl;
			//return (new Logger() );
		}

	};

}




#endif