#ifndef TASK_RUNNER_H
#define TASK_RUNNER_H

#include "ITaskRunner.h"
#include "IConfig.h"
#include "IObject.h"

#include "XmlConfig.h"
#include "Logger.h"

namespace jdb {

	class TaskRunner : public ITaskRunner, public IConfig, public IObject
	{
	public:

		virtual const char* classname() const { return "TaskRunner"; }
		TaskRunner(){}
		TaskRunner( XmlConfig _config, string _nodePath="", int _jobIndex = -1 ) {
			init( _config, _nodePath, _jobIndex );
		}
		TaskRunner( XmlConfig _config, string _nodePath="", string _fileList = "", string _jobPostfix = "" ) {
			init( _config, _nodePath, _fileList, _jobPostfix );
		}
		~TaskRunner(){}

		virtual void init(XmlConfig _config, string _nodePath="", int _jobIndex = -1) {
			DEBUG( classname(), "( " << _config.getFilename() << ", " << nodePath << ", " << _jobIndex << " )" );

			this->config 	= _config;
			// makes sure it is in the right form
			// not ending in '.' or ':attribute' etc.
			this->nodePath = this->config.basePath( _nodePath );
		}

		virtual void init(XmlConfig _config, string _nodePath="", string _fileList = "", string _jobPostfix = "") {
			DEBUG( classname(), "( " << _config.getFilename() << ", " << nodePath << ", \"" << _fileList << "\", \"" << _jobPostfix << "\" )" );

			this->config 	= _config;
			// makes sure it is in the right form
			// not ending in '.' or ':attribute' etc.
			this->nodePath = this->config.basePath( _nodePath );
		}

		virtual void run(){
			preMake();
			make();
			postMake();
		}
			

	protected:
		
		virtual void preMake(){
			DEBUG( classname(), "" );
		}

		virtual void make() {
			DEBUG( classname(), "" );

		}

		virtual void postMake() {
			DEBUG( classname(), "" );

		}

#ifdef __CINT__
		ClassDef( jdb::TaskRunner, 1)
#endif

	};
}

#endif