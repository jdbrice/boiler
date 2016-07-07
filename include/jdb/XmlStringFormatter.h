#ifndef XML_STRING_FORMATTER_H
#define XML_STRING_FORMATTER_H


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
	class XmlStringFormatter
	{
	public:
		XmlStringFormatter() {}
		~XmlStringFormatter() {}

		map<string, string> kv;

		vector<string> find_keys( string s ){
			vector<string> keys;
			std::smatch m;
	  		std::regex e ("\\{(.*?)\\}");

	  		regex_search( s, m, e );
			while (std::regex_search (s,m,e)) {
				if ( m.size() >= 2 )
					keys.push_back( m[1] );
				cout << m.size() << endl;

				cout << m[0] << " " <<m[1] << endl;
				// for (auto x:m) std::cout << x << " ";
				// 	std::cout << std::endl;
					s = m.suffix().str();
			}

			return keys;
		}

		string format( string _s, map<string, string> _kv ){
			string rs = _s;
			vector<string> keys = find_keys( _s );

			for (string key : keys ){
				cout << "Looking for " << key << endl;
				regex e( "\\{" + key + "\\}" );
				if ( _kv.count( key ) >= 1 ){
					rs = regex_replace( rs, e, _kv[ key ] );
				} else {
					cout << "replacing with empty string" << endl;
					
					rs = regex_replace( rs, e, "" );
				}
			}
			return rs;
		}


		
	};
}


#endif