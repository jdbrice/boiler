#include "XmlConfig.h"
#include <sys/stat.h>

#ifdef __CINT__
ClassImp( jdb::XmlConfig );
#endif

namespace jdb{

	XmlConfig::XmlConfig( string filename){
		DEBUG( classname(), "Loading Xml Configuration from : " << filename );

		setDefaults();
		loadFile( filename );
		
		// typedef map<string, string>::iterator map_it_type;
	}

	XmlConfig::XmlConfig(){
		DEBUG( classname(), "Empty XmlConfig");

		setDefaults();
	}

	XmlConfig::~XmlConfig(){
	}

	void XmlConfig::loadFile( string filename ){
		// check that the config file exists
		this->filename = filename;
		struct stat buffer;   
		bool exists = (stat (filename.c_str(), &buffer) == 0);
	
		if ( exists ){
#ifndef __CINT__
			RapidXmlWrapper rxw( filename );
#endif
			rxw.getMaps( &data, &isAttribute, &nodeExists );

			parseIncludes();
		} else {
			ERROR( classname(), "Config File \"" << filename << "\" DNE " ); 
		}
	}


	string XmlConfig::getString( string nodePath, string def ) const {

		string snp = sanitize( currentNode + nodePath );
		if ( nodeExists.count( snp ) >= 1 ){
			try{
				return data.at( snp );
			} catch (const std::out_of_range &oor ){
				return def;
			}
		}
		return def;
	}

	vector<string> XmlConfig::getStringVector( string nodePath, string defaultVal, int defaultLength ) const {
		
		if ( !exists( nodePath ) ){
			vector<string> d;
			for ( int i = 0; i < defaultLength; i++ )
				d.push_back( defaultVal );
			return d;
		}
		string value = getString( nodePath );
		return vectorFromString( value );
	}

	map<string, string> XmlConfig::getStringMap( string nodePath ) const{

		// first get a vector of comma delimeted pairs
		string value = getString( nodePath );
		vector<string> pairVec =  vectorFromString( value );
		
		map<string, string> rmap;
		// now we need to split each pair
		for ( unsigned int i = 0; i < pairVec.size(); i++ ){
			pair<string, string> parts = stringToPair( pairVec[ i ], mapDelim );
			rmap[ parts.first ] = parts.second;
		}
		return rmap;
	}

	map<int, int> XmlConfig::getIntMap( string nodePath ) const{

		// first get a vector of comma delimeted pairs
		string value = getString( nodePath );
		vector<string> pairVec =  vectorFromString( value );
		
		map<int, int> rmap;
		// now we need to split each pair
		for ( unsigned int i = 0; i < pairVec.size(); i++ ){
			pair<string, string> parts = stringToPair( pairVec[ i ], mapDelim );
			rmap[ atoi( parts.first.c_str() ) ] = atoi( parts.second.c_str() );
		}
		return rmap;
	}

	int XmlConfig::getInt( string nodePath, int def  ) const{
		string str = getString( nodePath, "" );
		if ( "" != str && str.length() >= 1 )
			return atoi( str.c_str() );
		return def;
	}
	vector<int> XmlConfig::getIntVector( string nodePath, int defaultVal, int defaultLength ) const{
		
		vector<int> d;
		// default if node does not exist
		if ( !exists( nodePath ) ){
			for ( int i = 0; i < defaultLength; i++ )
				d.push_back( defaultVal);
			return d;
		}
		

		vector<string> vec = getStringVector( nodePath );
		
		for ( unsigned int i = 0; i < vec.size(); i++  ){
			d.push_back( atoi( vec[ i ].c_str() ) );
		}
		return d;
	}

	double XmlConfig::getDouble( string nodePath, double def  ) const {
		string str = getString( nodePath, "" );
		if ( "" != str && str.length() >= 1 )
			return atof( str.c_str() );
		return def;
	}
	vector<double> XmlConfig::getDoubleVector( string nodePath, double defaultVal, int defaultLength ) const{
		
		vector<double> d;
		// default if node does not exist
		if ( !exists( nodePath ) ){
			for ( int i = 0; i < defaultLength; i++ )
				d.push_back( defaultVal );
			return d;
		}

		vector<string> vec = getStringVector( nodePath );
		for ( unsigned int i = 0; i < vec.size(); i++  ){
			d.push_back( atof( vec[ i ].c_str() ) );
		}
		return d;
	}

	float XmlConfig::getFloat( string nodePath, float def  ) const{
		return (float) getDouble( nodePath, (double)def );
	}


	bool XmlConfig::getBool( string nodePath, bool def  ) const{

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

	bool XmlConfig::exists( string nodePath ) const{
		string snp = sanitize( currentNode + nodePath );
		try{
			if( true == nodeExists.at( snp ) )
				return true;
		} catch ( std::out_of_range &oor ){
			return false;
		}
		return false;
	}

	string XmlConfig::sanitize( string nodePath ) const{

		// TODO
		// make the cn option applied here so everything uses it.
		// currently childrenOf does not work with it
		/**
		 * Remove internal whitespaces
		 */
		string ret = "";
		for ( unsigned int i = 0; i < nodePath.length(); i++ ){
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

	string XmlConfig::operator[]( string nodePath ) const {
		return getString( nodePath);
	}

	vector<string> & XmlConfig::split(const string &s, char delim, vector<string> &elems) const {
		stringstream ss(s);
		string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	vector<string> XmlConfig::vectorFromString( string data ) const {
				
		vector<string> d = split( data, ',' );
		
		for ( unsigned int i = 0; i < d.size(); i++ ){
			d[ i ] =  trim( d[ i ] );
		}
		return d;

	}

	std::string XmlConfig::trim(const std::string& str, const std::string& whitespace ) const {
		std::size_t strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return ""; // no content

		std::size_t strEnd = str.find_last_not_of(whitespace);
		std::size_t strRange = strEnd - strBegin + 1;

		return str.substr(strBegin, strRange);
	}

	vector<string> XmlConfig::split(const string &s, char delim) const {
		vector<string> elems;
		split(s, delim, elems);
		return elems;
	}

	string XmlConfig::manualToLower( string str ) const {
		string str2 = str;
		for ( unsigned int i = 0; i < str.length(); i++ ){
			str2[ i ] = std::tolower( str[ i ] );
		}
		return str2;
	}
	string XmlConfig::pathToParent( string nodePath ) const {
		vector<string> ntf = split( nodePath, pathDelim );
		vector<string> attr = split( nodePath, attrDelim );
		if ( attr.size() >= 2 ){
			ntf[ ntf.size() - 1 ] = ntf[ ntf.size() - 1 ].substr( 0, ntf[ ntf.size() - 1 ].length() - (attr[ 1].length() + 1) );
		}
		if ( ntf.size() >= 2 ){
			string fullPath ="";
			for ( unsigned int i = 0; i < ntf.size() - 1; i++ ){
				fullPath += (ntf[ i ] + pathDelim );
			}

			// remove the final pathDelim
			fullPath = fullPath.substr( 0, fullPath.length() - 1 );
			return fullPath;
		}
		return "";
	}
	string XmlConfig::tagName( string nodePath ) const{
		vector<string> ntf = split( nodePath, pathDelim );
		vector<string> attr = split( nodePath, attrDelim );
		if ( attr.size() >= 2 ){
			ntf[ ntf.size() - 1 ] = ntf[ ntf.size() - 1 ].substr( 0, ntf[ ntf.size() - 1 ].length() - (attr[ 1].length() + 1) );
		}
		if ( ntf.size() >= 1 ){
			vector<string> byIndex = split( ntf[ ntf.size() - 1 ], indexOpenDelim[0] );
			if ( byIndex.size() >= 2 ){
				return byIndex[ 0 ];
			}

			return ntf[ ntf.size() - 1 ];
		}
		return "";
	}
	string XmlConfig::attributeName( string nodePath ) const {
		vector<string> ntf = split( nodePath, pathDelim );
		vector<string> attr = split( nodePath, attrDelim );
		if ( attr.size() >= 2 ){
			return attr[ attr.size() - 1 ];
		}
		
		return "";
	}

	vector<string> XmlConfig::childrenOf( string nodePath, int relDepth, bool attrs ) const {

		
		nodePath = sanitize( nodePath );
		// if ( 	nodePath[ nodePath.length() - 1] != pathDelim && 
		// 		nodePath[ nodePath.length() - 1] != attrDelim  && "" != nodePath )
		// 	nodePath += pathDelim;
	
		int npDepth = depthOf( nodePath );

		vector<string> paths;
		for ( const_map_it_type it = data.begin(); it != data.end(); it++ ){

			size_t found = it->first.find( attrDelim );
			if ( found != string::npos && false == attrs )
				continue;
			
			// reject self
			if ( it->first == nodePath )
				continue;
			
			string parent = (it->first).substr( 0, nodePath.length() );
			if ( nodePath == parent ){
				
				if ( -1 == relDepth ) 
					paths.push_back( it->first );
				else {
					int dp = depthOf( it->first );

					if ( dp - npDepth > 0 && dp - npDepth <= relDepth ) 
						paths.push_back( it->first );
				}
			}
		}
		return paths;

	}

	vector<string> XmlConfig::childrenOf( string nodePath, string tag, int depth) const {

		// TODO: add depth awareness

		nodePath = sanitize( nodePath );
		if ( 	nodePath[ nodePath.length() - 1] != pathDelim && 
				nodePath[ nodePath.length() - 1] != attrDelim && "" != nodePath)
			nodePath += pathDelim;
	
		vector<string> paths;
		for ( const_map_it_type it = data.begin(); it != data.end(); it++ ){

			size_t found = it->first.find( attrDelim );
			if ( found != string::npos )
				continue;
			
			// reject self
			if ( it->first == nodePath )
					continue;
			string parent = (it->first).substr( 0, nodePath.length() );
			
			if ( nodePath == parent && (tag == tagName( it->first )) ){
				paths.push_back( it->first );
			} else if ( nodePath != parent ){
				// DEBUG( "Rejected because parent does not match" )
				// DEBUG( "parent=" << parent << ", shouldBe=" << nodePath )
			} else if ( tag != tagName( it->first ) ){
				// DEBUG( "Rejected because tag does not match" )
				// DEBUG( "tag=" << tagName( it->first ) << ", shouldBe=" << tag )
			}
		}
		return paths;

	}

	vector< string > XmlConfig::attributesOf( string nodePath ) const{
		return childrenOf( nodePath + attrDelim, -1, true );
	}

	map<string, string> XmlConfig::attributesMap( string nodePath ) const{
		DEBUG( classname(), "(" << nodePath << ")" )
		vector<string> pathToAttrs = attributesOf( nodePath );
		
		map<string, string> rmap;
		for ( string p : pathToAttrs ){
			rmap[ attributeName( p ) ] = getString( p );
		}
		return rmap;
	}

	vector<string> XmlConfig::getNodes( string nodePath ) const{

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
			for ( const_map_it_type it = data.begin(); it != data.end(); it++ ){
				
				try {
					if ( isAttribute.at( it->first ) )
						continue;				
				} catch ( std::out_of_range &oor ){
					// TODO: nothing?
				}

				
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
			} else {
				
			}

			vector<string> paths;
			for ( const_map_it_type it = data.begin(); it != data.end(); it++ ){

				try {
					if ( isAttribute.at( it->first ) )
						continue;				
				} catch ( std::out_of_range &oor ){
					// TODO: nothing?
				}

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

	pair<string, string> XmlConfig::stringToPair( string &s, string delim  ) const{

		std::size_t delimPos = s.find( delim );

		if ( std::string::npos != delimPos ){

			// get the string before the delim
			string pA = trim( s.substr( 0, delimPos ) );
			// get the part after
			string pB = trim( s.substr( delimPos + delim.length() ) );
			return make_pair( pA, pB );
		}
		return make_pair( "", "");
	}


	void XmlConfig::parseIncludes() {
		DEBUG( classname(), "" );
		vector<string> allPaths = childrenOf( "", "Include" );

		DEBUG( classname(), "Found " << allPaths.size() << " Include Tag(s)" );

		for ( string path : allPaths ){
			DEBUG( classname(), path )
			DEBUG( classname(), "parent path: " << pathToParent( path ) )

			string ifn = getString( path + ":url" );
			struct stat buffer;
			bool exists = (stat (ifn.c_str(), &buffer) == 0);
			DEBUG( classname(), "file " << ifn << " exists " << exists )

			// if we can't find it from the path directly then try relative to base config path
			if ( !exists ) { // try relative to this config file
				string basePath = pathFromFilename( filename );
				ifn = basePath + ifn;

				exists = (stat (ifn.c_str(), &buffer) == 0);
				DEBUG( classname(), "file " << ifn << " exists " << exists )
			}

			if ( exists ){
#ifndef __CINT__
				RapidXmlWrapper rxw(  ifn  );
#endif
				rxw.includeMaps( pathToParent( path ), &data,  &isAttribute, &nodeExists );
			}

		}

	   //DEBUG( report() );
	}

	void XmlConfig::applyOverrides( map< string, string > over ) {

		for ( auto k : over ){
			// require that the node/attribute already exist
			if ( data.count( k.first ) ){
				data[ k.first ] = k.second;
			}
		}
	}

	string XmlConfig::report( string nodePath ) const {

		vector<string> allPaths = childrenOf( nodePath, -1, true );

		stringstream sstr;
		for ( string path : allPaths ){
			string val = getString( path, "" );
			if ( "" != val )
				sstr << path << " === " << val << endl;
		}
		return sstr.str();

	}







}