#include "XmlString.h"
#include "XmlConfig.h"


namespace jdb{

	XmlString::XmlString() {}
	XmlString::~XmlString() {}


	void XmlString::replace_token( XmlConfig &cfg, string &_s, string &_key, int index, int len ) {

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
}
