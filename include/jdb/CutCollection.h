#ifndef CUT_COLLECTION_H
#define CUT_COLLECTION_H

#include "XmlConfig.h"
#include "Utils.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
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
		CutCollection( XmlConfig _config, string _nodePath ){ init( _config, nodePath ); }
		~CutCollection(){}

		map< string, shared_ptr<ConfigRange> > ranges;
		shared_ptr<ConfigRange> operator[]( string name ) {
			if ( ranges.count( name ) > 0 )
				return ranges[ name ];

			return nullptr;
		} // operator[]

		static constexpr auto tag = "CutCollection";


		void report() {
			for ( auto k : ranges ){
				INFO( tag, k.first << " : " << k.second->toString() );
			}
		} // report

	protected:

		void init( XmlConfig _config, string _nodePath ){

			vector<string> children = _config.childrenOf( _nodePath, "Range" );

			for ( string p : children ){
				

				if ( _config.exists( p + ":name" ) ){
					string name = _config.getString( p + ":name" );	
					shared_ptr<ConfigRange> cr = shared_ptr<ConfigRange>( new ConfigRange( &_config, p ) );	

					ranges[ name ] = cr;

					DEBUG( tag, "Added cut range '" << name << "' " );
					DEBUG( tag, cr->toString() );
				}	
			}
		} // init
	};
}

#endif