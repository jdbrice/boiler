#ifndef LOGGER_H
#define LOGGER_H 



#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
using namespace std;


namespace jdb {

	/*jdoc{
		"class" : "Logger",
		"desc" : "An all purpose logging utility with log level functionality. Meant to be used in modular projects. Multpile Logger instances can be used with different log levels."
	}*/
	class Logger{


	protected:

		/**
		 * For removing output seamlessly
		 * Not the most efficient but I like the way it allows the system to behave
		 */
		class NullBuffer : public std::streambuf
		{
		public:
		  int overflow(int c) { return c; }
		};
		class NullStream : public std::ostream {
		public:
			NullStream() : std::ostream(&sb) {}
			private: 
				NullBuffer sb;
		};


	public:

		//jdoc{ "name" : "static const int llAll = 40", "desc" : "Show all log messages"}
		/*jdoc{ "name" : "static const int llInfo = 30", "desc" : "Show info and below"}*/
		/*jdoc{ "name" : "static const int llWarn = 20", "desc" : "Show warnings and below"}*/
		/*jdoc{ "name" : "static const int llError = 10", "desc" : "Show errors and below"}*/
		/*jdoc{ "name" : "static const int llNone = 1", "desc" : "Show nothing"}*/
		static const int llAll 		= 60;
		static const int llDebug 	= 50;
		static const int llTrace 	= 40;
		static const int llInfo		= 30;
		static const int llWarn 	= 20;
		static const int llError 	= 10;	
		static const int llNone 	= 1;

		/*jdoc{ "name" : "static const int llDefault = llWarn", "desc" : "The default log level"}*/
		static const int llDefault	= llWarn;
		/*jdoc{ "name" : "static int llGlobal", "desc" : "The global log level used if not set otherwise"}*/
		static int llGlobal;

		/*jdoc{
			"name" : "inline static int getGlobalLogLevel()",
			"returns" : [
				"Global log level"
			]
		}*/
		static int getGlobalLogLevel();
		/*jdoc{
			"name" : "inline static void setGlobalLogLevel( int ll )",
			"params" : [
				"ll"
			],
			"paramDesc" : [
				"Log Level"
			],
			"returns" : [
				
			],
			"desc" : "Sets the global log level"
		}*/
		static void setGlobalLogLevel( int ll );

		/*jdoc{
			"name" : "Logger( int ll, string classSpace, ostream* os )",
			"params" : [
				"ll", "classSpace", "os"
			],
			"paramDesc" : [
				"Log Level", "Class name to prepend to messages", "Output stream"
			],
			"returns" : [
				
			],
			"desc" : "Creates a logger instance with the given properties"
		}*/
		Logger( int ll, string cs, ostream* oss ){
			logLevel = ll;
			cSpace = cs;
			this->os = oss;
		} 
		/*jdoc{
			"name" : "Logger( int ll = Logger::getGlobalLogLevel(), string classSpace = \"\" )",
			"params" : [
				"ll", "classSpace"
			],
			"paramDesc" : [
				"Optional: log level set to global log level",
				"Optional: Class name to prepend messages with"
			],
			"returns" : [
				
			],
			"desc" : "Default constructor - uses the global log level that can be set with static method"
		}*/
		Logger( int ll = Logger::getGlobalLogLevel(), string cs = "" ){
			logLevel = ll;
			cSpace = cs;
			os = &cout;
		}

		/*jdoc{
			"name" : "int getLogLevel()",
			"returns" : [
				"Current log level"
			]
		}*/
		int getLogLevel() { return logLevel; }
		/*jdoc{
			"name" : "void setLogLevel( int ll )",
			"params" : [
				"ll"
			],
			"paramDesc" : [
				"new log level to use"
			],
			"returns" : [
			]
		}*/
		void setLogLevel( int ll ) { logLevel = ll; }

		/*jdoc{
			"name" : "string getClassSpace()",
			"returns" : [
				"Current class prefix for messages"
			],
			"desc" : ""
		}*/
		string getClassSpace() { return cSpace; }

		/*jdoc{
			"name" : "void setClassSpace(string cs )",
			"params" : [
				"cs"
			],
			"paramDesc" : [
				"New class name for prepending to messages"
			],
			"desc" : ""
		}*/
		void setClassSpace(string cs ) { cSpace = cs; }

		/*jdoc{
			"name" : "ostream & warn( string functionName = \"\" )",
			"params" : [
				"functionName"
			],
			"paramDesc" : [
				"Calling function name to be prepended to message"
			],
			"returns" : [
				"An output stream for writing messages"
			],
			"desc" : "Shows all messages below llWarn"
		}*/
		ostream & warn( string functionName = "" ){
			if ( logLevel < llWarn )
				return ns;

			preMessage( "Warning", functionName );
			
			return (*os);
		}


		/*jdoc{
			"name" : "ostream & error( string functionName = \"\" )",
			"params" : [
				"functionName"
			],
			"paramDesc" : [
				"Calling function name to be prepended to message"
			],
			"returns" : [
				"An output stream for writing messages"
			],
			"desc" : "Shows all messages below llError"
		}*/
		ostream & error( string functionName = "" ){
			if ( logLevel < llError )
				return ns;

			preMessage( "ERROR", functionName );
			
			return (*os);
		}

		/*jdoc{
			"name" : "ostream & info( string functionName = \"\" )",
			"params" : [
				"functionName"
			],
			"paramDesc" : [
				"Calling function name to be prepended to message"
			],
			"returns" : [
				"An output stream for writing messages"
			],
			"desc" : "Shows all messages below llInfo"
		}*/
		ostream & info( string functionName = "" ){
			if ( logLevel < llInfo )
				return ns;

			preMessage( "Info", functionName );
			
			return (*os);
		}

		/*jdoc{
			"name" : "ostream & trace( string functionName = \"\" )",
			"params" : [
				"functionName"
			],
			"paramDesc" : [
				"Calling function name to be prepended to message"
			],
			"returns" : [
				"An output stream for writing messages"
			],
			"desc" : "Shows all messages below llTrace"
		}*/
		ostream & trace( string functionName = "" ){
			if ( logLevel < llInfo )
				return ns;

			preMessage( "Trace", functionName );
			
			return (*os);
		}

		/*jdoc{
			"name" : "ostream & debug( string functionName = \"\" )",
			"params" : [
				"functionName"
			],
			"paramDesc" : [
				"Calling function name to be prepended to message"
			],
			"returns" : [
				"An output stream for writing messages"
			],
			"desc" : "Shows all messages below llTrace"
		}*/
		ostream & debug( string functionName = "" ){
			if ( logLevel < llInfo )
				return ns;

			preMessage( "Debug", functionName );
			
			return (*os);
		}

		
		/*jdoc{
			"name" : "static int logLevelFromString( string ll )",
			"params" : [
				"ll"
			],
			"paramDesc" : [
				"String representation of log level"
			],
			"returns" : [
				"Integer representation of log level"
			],
			"desc" : "Can be one of [ info, warning, error, all, none ] default is all"
		}*/
		static int logLevelFromString( string ll ){
			if ( "debug" == ll )
				return llDebug;
			else if ( "trace" == ll )
				return llTrace;
			else if ( "info" == ll )
				return llInfo;
			else if ( "warning" == ll  )
				return llWarn;
			else if ( "error" == ll )
				return llError;
			else if ( "all" == ll )
				return llAll;
			else if ( "none" == ll )
				return llNone;
			return llAll;
		}

		void summary() {

			int w1 = 10;
			info() << std::left << std::setw(w1) << "# of ERRORS : " << counts[ "error" ] << endl;
			info() << std::left << std::setw(w1) << "# of Warnings : " << counts[ "warning" ] << endl;
			info() << std::left << std::setw(w1) << "# of Info : " << counts[ "info" ] << endl;
		}

	protected:
		
		int logLevel;
		map< string, int > counts;

		void preMessage( string level = "", string functionName = "" ){

			int w1 = 8;
			if ( cSpace.length() >= 2 && functionName.length() >= 2 )
				(*os) << std::left << std::setw(w1) << level << " : " << "[" << cSpace << "." << functionName << "] ";
			else if (cSpace.length() < 2 && functionName.length() >= 2)
				(*os) << std::left << std::setw(w1) << level << " : " << "[" << functionName << "] ";
			else if ( level.length() >= 1 )
				(*os) << "" << std::left << std::setw(w1) << level << " : ";

			string tag = level;
			// force level to lower case
			transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
			if ( !counts[ tag ] )
				counts[ tag ] = 0;
			counts[ tag ]++;
		}

		std::ostream* os;
		NullStream ns;

		string cSpace;


	};


}



#endif