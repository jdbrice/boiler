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

			INFO( classname(), "start = " << index );
			INFO( classname(), "stop = " << stop );
			
			if ( -1 == index || -1 == stop || len < 0 ) return rs;
			INFO( classname(), "substringing" );
			rs = s.substr( index+1, len-2 ); // just the part inside of the {}
			return rs;
		}

		void replace_token( string &_s, string &_key, int index, int len ){

			if ( cfg.exists( _key ) ) {
				string rv = cfg.getString( _key );
				_s.replace( index, len, rv );
			} else if ( kv.count( _key ) >= 1 ){
				string rv = kv[ _key ];
				_s.replace( index, len, rv );
			} else if ( getenv( _key.c_str() ) ) {
				string env = getenv( _key.c_str() );
				_s.replace( index, len, env );
			} else {
				_s.replace( index, len, "" );
			}
		}

		// vector<string> find_keys( string s, string _re ){
		// 	DEBUG( classname(), "Finding interpolation keys in : " << s );
		// 	vector<string> keys;

		// 	TRegexp e( _re.c_str() );
		// 	TString str(s.c_str() );

		// 	int pos = 0;
		// 	int index = -1;
		// 	int len = 0;
		// 	while( (index = str.Index( e, &len, pos )) >= pos ){

		// 		INFO( classname(), "index = " << index << " -> " << index + len);
		// 		pos = index + 1;
		// 	}

		// 	// smatch m;
	 //  // 		regex e ( "\\{(.*?)\\}");
	 //  // 		regex_search( s, m, e );
		// 	// while ( regex_search (s,m,e) ) {
		// 	// 	if ( m.size() >= 2 )
		// 	// 		keys.push_back( m[1] );
		// 	// 		s = m.suffix().str();
		// 	// 		DEBUG( classname(), m[1] );
		// 	// }
		// 	return keys;
		// }

		string format( string _s, string _re ="" ){ 

			int index = -1;
			int len = -1;
			int pos = 0;
			string key = first_token_at( _s, index, len, pos );
			while ( index >= 0 ){

				replace_token( _s, key, index, len );
				INFO( classname(), "new : " << _s );

				pos = index+1;
				cout << "pos = " << pos << endl;
				cout << key << endl;
				key = first_token_at( _s, index, len, pos );
			}
			return _s;
		}
		// 	DEBUG( classname(), "Formatting : " << _s );
		// 	string rs = _s;
		// 	vector<string> keys = find_keys( _s, _re );
		// 	int nkvs = kv.size();
		// 	string nullString = "";

		// 	for (string key : keys ){
		// 		// cout << "Looking for " << key << endl;
		// 		regex e( "\\{" + key + "\\}" );
		// 		if ( nkvs >= 1 && kv.count( key ) >= 1 ){
		// 			string rv = kv[ key ];
		// 			rs = regex_replace( rs, e, kv[ key ] );
		// 		} else if ( cfg.exists( key ) ) {
		// 			string rv = cfg.getString( key );
		// 			rs = regex_replace( rs, e, rv );
		// 		} else  if ( getenv( key.c_str() ) ) {
		// 			string env = getenv( key.c_str() );
		// 			rs = regex_replace( rs, e, env );
		// 		} else {
		// 			rs = regex_replace( rs, e, nullString );
		// 		}
		// 	}
		// 	DEBUG( classname(), "Result : " << rs );
		// 	return rs;
		// }

	protected:
		map<string, string> kv;

		
	};
}


#endif