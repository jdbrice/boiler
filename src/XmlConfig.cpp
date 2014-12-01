#include "XmlConfig.h"

using namespace std;

namespace jdb{

	XmlConfig::XmlConfig( string fname ){
		logger.setClassSpace( "XmlConfig" );
		logger.info( __FUNCTION__ ) << "Loading Xml Configuration from : " << fname << endl;

		// currently set may change
		pathDelim = '.';
		attrDelim = ':';
		indexOpenDelim = "[";
		indexCloseDelim = "]";
		equalDelim = '=';

		try {
			TDOMParser * dp = new TDOMParser();
			dp->SetValidate( false );

			int error = dp->ParseFile( fname.c_str() );
			if ( 0 != error )
				logger.info( __FUNCTION__ ) << "XML parsed with error code : " << error << endl;
			if ( dp && 0 == error ){
				doc = dp->GetXMLDocument();
				if ( doc && 0 == error ){
					rootNode = doc->GetRootNode();
					makeMap( rootNode, "" );
				} else {
					logger.error( __FUNCTION__ ) << "Unable to parse config file. All nodes must be within a root node, like <config> ... </config>" << endl;
				}
			} else {
				logger.error( __FUNCTION__ ) << "Unable to parse config file, please check for formatting errors" << endl;
			}
			
		} catch( exception &e ) {
			logger.error( __FUNCTION__ ) << e.what() << endl;
			logger.error( __FUNCTION__ ) << "Could not parse XML" << endl;
			cout << "ERROR : " << e.what() << endl;
		}
	}

	XmlConfig::~XmlConfig(){

	}

	void XmlConfig::makeMap( TXMLNode * parent, string cp ){

		TXMLNode * c = parent->GetChildren();
		
		std::map< string, int > index;

		while ( c && c->HasNextNode() ){
			string name = c->GetNodeName();
			string nodeName = c->GetNodeName();
			
			if ( name == "text" || name == "comment" ){
				c = c->GetNextNode();	
				continue;
			}

			if ( !index[ nodeName ] )
				index[ nodeName ] = 0;

			stringstream sstr;
			sstr << index[ nodeName ];
			string path =  cp ;
			if ( "" != cp )
				path += pathDelim + nodeName;
			else 
				path += nodeName;
			
			if ( index[ nodeName ] >= 1 )
				path += indexOpenDelim + sstr.str() + indexCloseDelim;

			index[ nodeName ]++;

			/**
			 * Get attributes
			 */
			isAttribute[ path ] = false;
			nodeExists[ path ] = true;
			 if ( c->GetText()){
			 	data[path] = trim( std::string(c->GetText()) );
			 } else 
			 	data[path] = "";
				
			
			map<string, string> attrs = attributes( c );
			// copy the map into this one
			for ( map_it_type it = attrs.begin(); it != attrs.end(); it++ ){
				string aPath = path + attrDelim + it->first;
				data[ aPath ] = it->second;
				nodeExists[ aPath ] = true;
				isAttribute[ aPath ] = true;
			}

			makeMap( c, path );

			c = c->GetNextNode();
		}

	}

	map<string, string> XmlConfig::attributes( TXMLNode * node ){

		map<string, string> ret;
		if ( node && node->HasAttributes() ){
			TIter i( (TCollection*)node->GetAttributes() );
			
			TXMLAttr * cAttr;

			while ( cAttr = (TXMLAttr*)i.Next() ){
				ret[ cAttr->GetName() ] = cAttr->GetValue();	
			}
		}

		return ret;
	}

	string XmlConfig::getString( string nodePath, string def ){
		string snp = sanitize( nodePath );
		if ( nodeExists[ snp ] && NULL != data[ snp ]){
			return data[ snp ];
		}
		return def;
	}

	vector<string> XmlConfig::getStringVector( string nodePath ){
		string value = getString( nodePath );
		return vectorFromString( value );
	}

	int XmlConfig::getInt( string nodePath, int def  ){
		string str = getString( nodePath, "" );
		if ( "" != str && str.length() >= 1 )
			return atoi( str.c_str() );
		return def;
	}
	vector<int> XmlConfig::getIntVector( string nodePath ){
		vector<string> vec = getStringVector( nodePath );
		vector<int> d;
		for ( int i = 0; i < vec.size(); i++  ){
			d.push_back( atoi( vec[ i ].c_str() ) );
		}
		return d;
	}

	double XmlConfig::getDouble( string nodePath, double def  ){
		string str = getString( nodePath, "" );
		if ( "" != str && str.length() >= 1 )
			return atof( str.c_str() );
		return def;
	}
	vector<double> XmlConfig::getDoubleVector( string nodePath ){
		vector<string> vec = getStringVector( nodePath );
		vector<double> d;
		for ( int i = 0; i < vec.size(); i++  ){
			d.push_back( atof( vec[ i ].c_str() ) );
		}
		return d;
	}

	float XmlConfig::getFloat( string nodePath, float def  ){
		return (float) getDouble( nodePath, (double)def );
	}


	bool XmlConfig::getBool( string nodePath, bool def  ) {

		string str = getString( nodePath );

		// first check for string literal "true" or "false"
		// push to lower case
		//std::transform( str.begin(), str.end(), str.begin(), std::tolower );
		str = manualToLower( str );
		if ( str == "false" )
			return false;
		if ( str == "true" )
			return true;

		// if not look for an integer
		// 0 or negative = false
		// any positive = true
		if ( str.length() >= 1 ){
			return ( atoi( str.c_str() ) >= 1 );
		}
		return def;
	}

	bool XmlConfig::exists( string nodePath ){
		string snp = sanitize( nodePath );
		if( nodeExists[ snp ] )
			return true;
		return false;
	}

	string XmlConfig::sanitize( string nodePath ){

		/**
		 * Remove internal whitespaces
		 */
		string ret = "";
		for ( int i = 0; i < nodePath.length(); i++ ){
			if ( nodePath[i] != ' ' )
				ret += (char)nodePath[ i ];
		}

		/**
		 * Remove [0] since they are found by leaving it off
		 */
		string rString = indexOpenDelim+"0"+indexCloseDelim;
		size_t found = ret.find(rString);
		while( found != string::npos ){
			//cout << "Period found at: " << found << '\n';
			ret.erase( found, rString.length() );
			found = ret.find(rString);
		}

		return ret;
	}

	string XmlConfig::operator[]( string nodePath ){
		if ( nodeExists[ nodePath ] )
			return data[ nodePath ];

		string other = sanitize( nodePath );
		if ( nodeExists[ other ] )
			return data[ other ];

		return "";
	}

	vector<string> & XmlConfig::split(const string &s, char delim, vector<string> &elems) {
	    stringstream ss(s);
	    string item;
	    while (std::getline(ss, item, delim)) {
	        elems.push_back(item);
	    }
	    return elems;
	}

	vector<string> XmlConfig::vectorFromString( string data ){
				
		vector<string> d = split( data, ',' );
		
		for ( int i = 0; i < d.size(); i++ ){
			d[ i ] =  trim( d[ i ] );
		}
		return d;

	}

	std::string XmlConfig::trim(const std::string& str, const std::string& whitespace ) {
	    std::size_t strBegin = str.find_first_not_of(whitespace);
	    if (strBegin == std::string::npos)
	        return ""; // no content

	    std::size_t strEnd = str.find_last_not_of(whitespace);
	    std::size_t strRange = strEnd - strBegin + 1;

	    return str.substr(strBegin, strRange);
	}

	vector<string> XmlConfig::split(const string &s, char delim) {
	    vector<string> elems;
	    split(s, delim, elems);
	    return elems;
	}

	string XmlConfig::manualToLower( string str ){
		string str2 = str;
		for ( int i = 0; i < str.length(); i++ ){
			str2[ i ] = std::tolower( str[ i ] );
		}
		return str2;
	}
	string XmlConfig::tagName( string nodePath ){
		vector<string> ntf = split( nodePath, pathDelim );
		vector<string> attr = split( nodePath, attrDelim );
		if ( attr.size() >= 2 ){
			ntf[ ntf.size() - 1 ] = ntf[ ntf.size() - 1 ].substr( 0, ntf[ ntf.size() - 1 ].length() - (attr[ 1].length() + 1) );
		}
		if ( ntf.size() >= 1 )
			return ntf[ ntf.size() - 1 ];
		return "";
	}
	string XmlConfig::attributeName( string nodePath ){
		vector<string> ntf = split( nodePath, pathDelim );
		vector<string> attr = split( nodePath, attrDelim );
		if ( attr.size() >= 2 ){
			return attr[ attr.size() - 1 ];
		}
		
		return "";
	}

	vector<string> XmlConfig::childrenOf( string nodePath, int depth, bool attrs ){

		if ( 	nodePath[ nodePath.length() - 1] != pathDelim && 
				nodePath[ nodePath.length() - 1] != attrDelim)
			nodePath += pathDelim;
	
		vector<string> paths;
		for ( map_it_type it = data.begin(); it != data.end(); it++ ){

			size_t found = it->first.find( attrDelim );
			if ( found != string::npos && false == attrs )
				continue;
			
			// reject self
			if ( it->first == nodePath )
					continue;
			string parent = (it->first).substr( 0, nodePath.length() );
			if ( nodePath == parent ){
				paths.push_back( it->first );
			}
		}
		return paths;

	}

	vector< string > XmlConfig::attributesOf( string nodePath ){
		return childrenOf( nodePath + attrDelim, -1, true );
	}

	vector<string> XmlConfig::getNodes( string nodePath ){

		nodePath = sanitize( nodePath );
		// for instance
		// case 1) test.sub should return test.sub[0...N]
		// case 2) test.sub:name should return all test.sub[0...N] with a name attribute
		// case 3) test.sub:name=dan should return test.sub[]:name == dan -> true

		vector<string> nodes = split( nodePath, pathDelim );
		vector<string> attrs = split( nodePath, attrDelim );
	
		// case 1) no attr given, just find siblings like given
		if ( attrs.size() <= 1  ){
			
			vector<string> paths;
			for ( map_it_type it = data.begin(); it != data.end(); it++ ){

				if ( isAttribute[ it->first ]  )
					continue;
				
				string parent = (it->first).substr( 0, nodePath.length() );
				if ( nodePath == parent ){
					paths.push_back( it->first );
				}
			}
			return paths;

		} else if ( attrs.size() == 2 ) {
			
			// get everything up to attr delim
			string baseNodePath = attrs[ 0 ];

			// split off the conditional
			vector<string> conds = split( attrs[ 1 ], equalDelim );
			
			// case 2) if no conditional is given just check for existance of attr
			string attrName = attrs[ 1 ];
			bool attrEquals = false;
			string attrIs = "";

			// case 3) if an equals is given
			if ( conds.size() == 2 ){
				attrIs = conds[ 1 ];
				attrName = conds[ 0 ];
				attrEquals = true;
			}

			vector<string> paths;
			for ( map_it_type it = data.begin(); it != data.end(); it++ ){

				if ( isAttribute[ it->first ] )
					continue;

				string wA = it->first + attrDelim + attrName;
				
				string parent = (it->first).substr( 0, baseNodePath.length() );

				if ( baseNodePath == parent ){
					bool aExists = exists( wA );
					
					// test for attribute existing
					if ( !aExists ){
						continue;
					}
					// if given, test for equality to value
					if ( aExists && attrEquals && attrIs != getString( wA ) ){
						continue;
					}
					
					paths.push_back( it->first );
				}
			}
			return paths;
		}

		vector<string> ret;
		return ret;
	}

	//vector<string> nodesWhere( string nodePath, string attr, string equals )

}