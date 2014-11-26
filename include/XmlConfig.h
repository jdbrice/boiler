#ifndef XML_CONFIG_H
#define XML_CONFIG_H


/*
	STD
 */
#include <string>
#include <iostream>
#include <vector>
#include <exception>
#include <sstream>
#include <map>
#include <algorithm>
#include <iomanip>
/*
	ROOT
 */
#include "TXMLAttr.h"
#include "TXMLNode.h"
#include "TDOMParser.h"
#include "TXMLDocument.h"
#include "TCollection.h"

/**
 * JDB
 */

#include "Logger.h"

using namespace std;

namespace jdb {
	
	/*jdoc{
		"class" : "XmlConfig",
		"desc" : "A lot goes here"
	}*/
	class XmlConfig
	{
	protected:


		Logger logger;
		
		map<string, bool> nodeExists;
		
		map<string, string> data;
		

		typedef map<string, string>::iterator map_it_type;

		//jdoc{ "name" : "string filename", "desc" : "Filename of the config file"}
		string filename;

		//jdoc{ "name" : "TXMLDocument *doc", "desc" : "TXMLDocument returned by the DOM parser"}
		TXMLDocument * doc;

		//jdoc{ "name" : "TXMLNode *rootNode", "desc" : "The XML DOM's root node"}
		TXMLNode * rootNode;

		//jdoc{ "name" : "char attrDelim", "desc" : "The delimiter used for attributes - Default is \":\""}
		char attrDelim;

		//jdoc{ "name" : "char pathDelim", "desc" : "The delimiter used for paths - Default is \".\""}
		char pathDelim;

		//jdoc{ "name" : "indexOpenDelim", "desc" : "The delimeter for index open - Default is \"[\""}
		string indexOpenDelim;
		
		//jdoc{ "name" : "indexCloseDelim", "desc" : "The delimeter for index open - Default is \"]\""}
		string indexCloseDelim;



	public:

		/*jdoc{
			"name" : "XmlConfig( string filename )",
			"params" : [
				"filename"
			],
			"paramDesc" : [
				"The filename of the xml file to load"
			],
			"returns" : [
				
			],
			"desc" : "Loads the XML file in and parses it using the TDOMParser. Successfully parsed xml will produce a document and a root node."
		}*/
		XmlConfig( string fname );
		~XmlConfig();

		void makeMap( TXMLNode * parent, string cp = "" );

		string getString( string nodePath, string def = "" );
		vector<string> getStringVector( string nodePath );
		int getInt( string nodePath, int def = 0 );
		vector<int> getIntVector( string nodePath );

		double getDouble( string nodePath, double def = 0 );
		vector<double> getDoubleVector( string nodePath );

		float getFloat( string nodePath, float def = 0 );


		bool getBool( string nodePath, bool def = false );

		string manualToLower( string str );

		bool exists( string nodePath );

		vector<string> childrenOf( string nodePath, int depth = -1, bool attrs = false );
		vector<string> attributesOf( string nodePath );
		vector<string> getNodes( string nodePath );

		/**
		 * Helper Methods
		 */
		vector<string> split(const string &s, char delim);

		std::string trim(const std::string& str, const std::string& whitespace = " \t\n");

		string sanitize( string nodePath );
		string operator[]( string nodePath );

		string tagName( string nName );
		string attributeName( string nName );

	protected:

		vector<string> vectorFromString( string data );
		vector<string> &split(const string &s, char delim, vector<string> &elems);
		map<string, string> attributes( TXMLNode * node );
		
	};
}





#endif