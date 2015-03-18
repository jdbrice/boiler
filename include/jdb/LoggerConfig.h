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

			if ( config && config->getBool( nodePath+":color" ) )
				Logger::setGlobalColor( true );

			if ( 	config && config->exists( nodePath ) &&
					config->exists( nodePath + ".logLevel" ) ){

				string ll = config->getString( nodePath + ".logLevel" );
				return (new Logger( Logger::logLevelFromString( ll ) ) );
				
			}
			return (new Logger() );
		}

	};

}




#endif