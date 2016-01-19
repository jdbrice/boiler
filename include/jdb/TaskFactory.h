#ifndef TASK_FACTORY_H
#define TASK_FACTORY_H

// STL
#include <string>

// ROOBARB

namespace jdb {

	class TaskRunner;

	class TaskFactory {

	public:
		typedef std::map<std::string, TaskRunner*(*)()> map_type;

		TaskFactory(){}
		~TaskFactory(){}

		static TaskRunner * createTaskRunner( string const& typeName ) {
			map_type::iterator it = getMap()->find( typeName );
			if(it == getMap()->end())
				return nullptr;
			return it->second();
		}


		template<typename T>
		static TaskRunner * createInstance() { return new T; }

		template<typename T>
		static void registerTaskRunner( string const &typeName ){
			getMap()->insert( make_pair( typeName, &TaskFactory::createInstance<T>));
		}
		

	protected:
		static map_type * getMap() {
			// never delete'ed. (exist until program termination)
			// because we can't guarantee correct destruction order 
			if(!trMap) { trMap = new map_type; } 
			return trMap; 
		}

	private:
		static map_type * trMap;

	};


}





#endif