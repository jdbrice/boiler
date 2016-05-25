#ifndef ENGINE_H
#define ENGINE_H

// RooBarb
#include "Logger.h"
#include "XmlConfig.h"
#include "Utils.h"

#include "TaskFactory.h"
#include "TaskRunner.h"
	// Interfaces
	#include "IConfig.h"
	#include "IObject.h"

// STL
#include <exception>
#include <sys/stat.h>

// format
#include "format.h"

namespace jdb{

	class Engine : public IObject, public IConfig
	{
	public:
		virtual const char * classname() const { return "Engine"; }
		Engine( int argc, char *argv[] ){

			try {

				getCmdLineConfigOverrides( argc, argv );
				// log everything for the moment
				Logger::setGlobalLogLevel( "all" );


				if ( argc >= 2 ){
					
					// sets the jobIndex either from positional arg
					// or if --jobIndex=N is given use that (dont the positional arg)
					setJobIndex( argc, argv );
					INFO( classname(), "Job Index = " << jobIndex );


					// create the config file for understanding jobs
					INFO( classname(), "Loading engine config : " << argv[1] );
					if ( !fileExists( argv[1] ) ){
						ERROR( classname(), "Cannot load " << argv[1] << ": File DNE" );
					} else {
						config.loadFile( argv[1] );
						config.applyOverrides( cmdLineConfig );

						runTasks();
					}
				} // if argv >= 2
			} catch ( exception &e ) {

			}
		
		}
		~Engine() {}

		inline bool fileExists (const std::string& name) {
			struct stat buffer;   
			return (stat (name.c_str(), &buffer) == 0); 
		}


		void runTasks(){

			vector<string> paths = config.childrenOf( "", "Task" );
			INFO( classname(), "Found " << paths.size() << plural( paths.size(), " task", " tasks" ) );

			for ( string p : paths ){

				string _name = config[ p + ":name" ];
				string _type = config[ p + ":type" ];
				string _configFile = config[ p + ":config" ];
				string _nodePath = config[ p + ":nodePath" ];

				XmlConfig _taskConfig;

				INFO( classname(), "Running " << _type << " " << _name );
				if ( "" == _configFile ){
					_taskConfig = config;
				} else if ( fileExists( _configFile ) ){
					// INFO( classname(), "Using " << _configFile << " config for task" );
					_taskConfig.loadFile( _configFile );
					_taskConfig.applyOverrides( cmdLineConfig );
				} else {
					ERROR( classname(), "Cannot load config " << _configFile );
					continue;
				}

				// we have a config file now;
				INFO( classname(), "Using " << _taskConfig.getFilename() << " config for task" );
				
				TaskRunner * taskRunner = TaskFactory::createTaskRunner( _type );
				if ( nullptr == taskRunner ){
					ERROR( classname(), "Cannot create " << _type << ". Maybe you need to register it?" );
					ERROR( classname(), "TaskFactory::registerTaskRunner<" << _type << ">( \""<< _type <<"\" );" );
					continue;
				}

				taskRunner->init( _taskConfig, _nodePath, jobIndex );
				taskRunner->run();

				delete taskRunner;

			}// loop on task paths
		}// runTasks

	protected:
		int jobIndex;

		map<string, string> cmdLineConfig;
		void getCmdLineConfigOverrides( int argc, char * argv[] ){
			
			for ( int i = 0; i < argc; i++ ){
				string v = argv[ i ];
				if ( 0 == v.compare( 0, 2, "--" ) ){
					
					vector<string> parts = split( v, '=' );
					if ( parts.size() <= 1 ) continue;

					string path = parts[0].substr( 2 );
					string value = parts[1];

					cmdLineConfig[ path ] = value;
				} // check that it starts with "--"
			} // loop over argc
		} // getCmdLineConfigOverrides


		// only needed so thatI cna be backwards compatibile with my old way
		// should be called after getCmdLineConfigOverrides
		void setJobIndex(int argc, char * argv[]){

			if ( cmdLineConfig.count( "jobIndex" ) >= 1 ){
				jobIndex = atoi( cmdLineConfig[ "jobIndex" ].c_str() );
			} else if ( argc >= 3 ){
				jobIndex = atoi( argv[2] );
			} else {
				jobIndex = -1; // not parallel job
			}

			// amke sure that the jobIndex is set in the in-memory configs
			cmdLineConfig[ "jobIndex" ] = ts( jobIndex );
		} // setJobIndex


		std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
			return elems;
		}


		std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}
		


#ifdef __CINT__
		ClassDef( jdb::Engine, 1)
#endif
		
	};


}


#endif