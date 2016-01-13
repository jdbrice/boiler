#ifndef CUT_COLLECTION_H
#define CUT_COLLECTION_H

#include "XmlConfig.h"
#include "Utils.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Logger.h"



#include <memory>
using namespace jdb;

namespace jdb{

	class CutCollection{
	public:
		CutCollection( XmlConfig * cfg, string np ){ init( cfg, np ); }
		~CutCollection(){}

		map< string, shared_ptr<ConfigRange> > ranges;
		shared_ptr<ConfigRange> operator[]( string name ) {
			if ( ranges.count( name ) > 0 )
				return ranges[ name ];

			return nullptr;
		}

		static constexpr auto tag = "CutCollection";


		void report() {
			for ( auto k : ranges ){
				INFO( tag, k.first << " : " << k.second->toString() );
			}
		}

	protected:

		void init( XmlConfig * cfg, string np ){

			vector<string> children = cfg->childrenOf( np, "Range" );

			for ( string p : children ){
				

				if ( cfg->exists( p + ":name" ) ){
					string name = cfg->getString( p + ":name" );	
					shared_ptr<ConfigRange> cr = shared_ptr<ConfigRange>( new ConfigRange( cfg, p ) );	

					ranges[ name ] = cr;

					DEBUG( tag, "Added cut range '" << name << "' " );
					DEBUG( tag, cr->toString() );
				}	
			}

		}



	};
}

#endif