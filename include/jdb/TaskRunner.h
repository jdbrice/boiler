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
		TaskRunner( XmlConfig _config, string _nodePath="", string _fileList="", string _prefix="" ) {
			init( _config, _nodePath, _fileList, _prefix );
		}
		~TaskRunner(){}

		virtual void init(XmlConfig _config, string _nodePath="", string _fileList="", string _prefix="") {
			DEBUG( classname(), "( config, " << nodePath << ", " << _fileList << ", " << _prefix << " )" );
		}

		virtual void run(){
			preMake();
			make();
			postMake();
		}
			

	protected:

		// Prefix for this job ( optional, used for parallel)
		string jobPrefix;
		
		// fileList for running data
		string fileList;
		
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