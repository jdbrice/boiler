#ifndef CUT_COLLECTION_H
#define CUT_COLLECTION_H

#include "XmlConfig.h"
#include "Utils.h"
#include "XmlRange.h"
#include "XmlPoint.h"
#include "Logger.h"


#include "IConfig.h"
#include "IObject.h"


#include <memory>
using namespace jdb;

namespace jdb{

	class CutCollection : public IConfig, public IObject {
	public:

		virtual const char* classname() const { return "CutCollection"; }
		CutCollection() {};
		CutCollection( XmlConfig _config, string _nodePath ){ init( _config, _nodePath ); }
		~CutCollection(){}

		map< string, shared_ptr<XmlRange> > ranges;
		shared_ptr<XmlRange> operator[]( string name ) {
			if ( ranges.count( name ) > 0 )
				return ranges[ name ];

			return nullptr;
		} // operator[]

		void report() {
			for ( auto k : ranges ){
				INFO( classname(), k.first << " : " << k.second->toString() );
			}
		} // report

	protected:

		void init( XmlConfig _config, string _nodePath ){

			vector<string> children = _config.childrenOf( _nodePath, "Range" );

			for ( string p : children ){
				

				if ( _config.exists( p + ":name" ) ){
					string name = _config.getString( p + ":name" );	
					shared_ptr<XmlRange> cr = shared_ptr<XmlRange>( new XmlRange( &_config, p ) );	

					ranges[ name ] = cr;

					DEBUG( classname(), "Added cut range '" << name << "' " );
					DEBUG( classname(), cr->toString() );
				}	
			}
		} // init
	};
}

#endif