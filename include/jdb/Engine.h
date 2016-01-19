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


namespace jdb{

	class Engine : public IObject, public IConfig
	{
	public:
		virtual const char * classname() const { return "Engine"; }
		Engine( int argc, char *argv[] ){

			try {

				// log everything for the moment
				Logger::setGlobalLogLevel( "all" );


				if ( argc >= 2 ){


					fileList = "";
					jobPrefix = "";

					if ( argc >= 4 ){
						fileList = argv[ 2 ];
						jobPrefix = argv[ 3 ];
						INFO( classname(), "Filelist = " << fileList );
						INFO( classname(), "Job Prefix = " << jobPrefix );
					}
					// create the config file for understanding jobs
					INFO( classname(), "Loading engine config : " << argv[1] );
					if ( !fileExists( argv[1] ) ){
						ERROR( classname(), "Cannot load " << argv[1] << ": File DNE" );
					} else {
						config.loadFile( argv[1] );

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

				taskRunner->init( _taskConfig, _nodePath, fileList, jobPrefix );
				taskRunner->run();

				delete taskRunner;

			}// loop on task paths
		}// runTasks

	protected:
		string fileList;
		string jobPrefix;


#ifdef __CINT__
		ClassDef( jdb::Engine, 1)
#endif
		
	};


}


#endif