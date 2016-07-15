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
#include <cstdlib>

#include "TString.h"
#include "TRegexp.h"
#include <regex.h> 


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

		XmlConfig cfg;
		
		

		string first_token_at( string &s, int &index, int &len, int pos = 0 ){
			string rs = s;
				
			string _tkstart = "{";
			string _tkstop = "}";

			index = s.find( _tkstart, pos );

			int stop = s.find( _tkstop, index >= pos ? index : pos);
			len = stop - index + 1;

			DEBUG( classname(), "start = " << index );
			DEBUG( classname(), "stop = " << stop );
			
			if ( -1 == index || -1 == stop || len < 0 ) return rs;
			
			rs = s.substr( index+1, len-2 ); // just the part inside of the {}
			return rs;
		}

		void replace_token( string &_s, string &_key, int index, int len ){

			if ( kv.count( _key ) >= 1 ){
				string rv = kv[ _key ];
				_s.replace( index, len, rv );
			} else if ( getenv( _key.c_str() ) ) {
				string env = getenv( _key.c_str() );
				_s.replace( index, len, env );
			} else if ( cfg.exists( _key ) ) {
				string rv = cfg.getString( _key );
				_s.replace( index, len, rv );
			} else {
				_s.replace( index, len, "" );
			}
		}

		string format( string _s, string _re ="" ){ 

			int index = -1;
			int len = -1;
			int pos = 0;
			string key = first_token_at( _s, index, len, pos );
			while ( index >= 0 ){

				replace_token( _s, key, index, len );
				DEBUG( classname(), "new : " << _s );

				pos = index+1;
				DEBUG( classname(), "pos = " << pos );
				DEBUG( classname(), key );
				key = first_token_at( _s, index, len, pos );
			}
			return _s;
		}


	protected:
		map<string, string> kv;

		
	};
}


#endif