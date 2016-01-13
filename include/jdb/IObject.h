#ifndef IOBJECT_H
#define IOBJECT_H

namespace jdb {
	class IObject
	{
	public:
		virtual ~IObject() {};
		
		virtual const char* classname() = 0;
	};
}


#endif