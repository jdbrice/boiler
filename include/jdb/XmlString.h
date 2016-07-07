#ifndef XML_STRING_H
#define XML_STRING_H

#include "XmlConfig.h"
#include "Utils.h"

#include <iostream>
#include <string>
#include <regex>
#include <iterator>
#include <string>
#include <map>

using namespace std;

// Formats a string like
// "this is a {key1} and {path.to.node}"
namespace jdb{
	class XmlString
	{
	public:
		virtual const char* classname() const { return "XmlString"; }
		XmlString() { }
		XmlString( XmlConfig &_cfg ) { cfg = _cfg; }
		~XmlString() {}

		void add( string k, string v ){
			DEBUG( classname(), "Adding [" << k << "] = " << v );
			kv[ k ] = v;
		}
		void add( string k, int v ){
			add( k, ts(v) );
		}
		void add( string k, double v ){
			add( k, dts(v) );
		}
		void add( string k, float v ){
			add( k, dts(v) );
		}

		// static const XmlConfig emptyConfig;
		XmlConfig cfg;

		vector<string> find_keys( string s ){
			DEBUG( classname(), "Finding interpolation keys in : " << s );
			vector<string> keys;
			smatch m;
	  		regex e ("\\{(.*?)\\}");
	  		regex_search( s, m, e );
			while ( regex_search (s,m,e) ) {
				if ( m.size() >= 2 )
					keys.push_back( m[1] );
					s = m.suffix().str();
			}
			return keys;
		}

		string format( string _s ){
			DEBUG( classname(), "Formatting : " << _s );
			string rs = _s;
			vector<string> keys = find_keys( _s );

			for (string key : keys ){
				// cout << "Looking for " << key << endl;
				regex e( "\\{" + key + "\\}" );
				if ( kv.count( key ) >= 1 ){

					string rv = kv[ key ];
					rs = regex_replace( rs, e, kv[ key ] );
				} else if ( cfg.exists( key ) ) {
					string rv = cfg.getString( key );
					rs = regex_replace( rs, e, rv );
				} else {
					// cout << "replacing with empty string" << endl;
					
					rs = regex_replace( rs, e, "" );
				}
			}
			return rs;
		}

	protected:
		map<string, string> kv;

		
	};
}


#endif