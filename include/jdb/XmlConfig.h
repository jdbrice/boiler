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
		"desc" : "The XmlConfig class allows you to use XML for simple project configuration files."
	}*/
	class XmlConfig
	{
	protected:

		Logger logger;
		
		map<string, bool> nodeExists;
		
		map<string, string> data;

		map<string, bool> isAttribute;
		
		typedef map<string, string>::iterator map_it_type;

		//pjdco{ "name" : "string filename", "desc" : "Filename of the config file"}
		string filename;

		//pjdco{ "name" : "TXMLDocument *doc", "desc" : "TXMLDocument returned by the DOM parser"}
		TXMLDocument * doc;

		//pjdco{ "name" : "TXMLNode *rootNode", "desc" : "The XML DOM's root node"}
		TXMLNode * rootNode;

		//pjdco{ "name" : "char attrDelim", "desc" : "The delimiter used for attributes - Default is \":\""}
		char attrDelim;

		//pjdco{ "name" : "char pathDelim", "desc" : "The delimiter used for paths - Default is \".\""}
		char pathDelim;

		//pjdco{ "name" : "char equalDelim", "desc" : ""}
		char equalDelim;

		//pjdco{ "name" : "indexOpenDelim", "desc" : "The delimeter for index open - Default is \"[\""}
		string indexOpenDelim;
		
		//pjdco{ "name" : "indexCloseDelim", "desc" : "The delimeter for index open - Default is \"]\""}
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

		/*jdoc{
			"name" : "string operator[]( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node"
			],
			"returns" : [
				"Value of node as an std::string"
			],
			"desc" : "Same as getString(...) but with the [] operator."
		}*/
		string operator[]( string nodePath );
		
		/*jdoc{
			"name" : "string getString( string nodePath, string def = \"\" )",
			"params" : [
				"nodePath", "def"
			],
			"paramDesc" : [
				"Path to node", "Optional: Default returned if node DNE"
			],
			"returns" : [
				"Value of node as an std::string"
			],
			"desc" : "Gets the string value of a node or attribute. 
			If the node or attribute does not exist then the default valuse is returned. "
		}*/
		string getString( string nodePath, string def = "" );

		/*jdoc{
			"name" : "vector< string > getStringVector( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node"
			],
			"returns" : [
				"A vector of strings or an empty vector if the node DNE"
			],
			"desc" : "Gets a vector from a comma delimeted list.
			For instance, if a node contains \"1, 3, 9, 7, 16\" then the vector whould contain 5 elements.
			"
		}*/
		vector<string> getStringVector( string nodePath );

		/*jdoc{
			"name" : "int getInt( string nodePath, int def = 0 )",
			"params" : [
				"nodePath", "def"
			],
			"paramDesc" : [
				"Path to node", "Optional: Default value for node DNE"
			],
			"returns" : [
				"The nodes value converted to an integer or the default if the node DNE."
			],
			"desc" : "Gets a nodes value and returns it as an int type"
		}*/
		int getInt( string nodePath, int def = 0 );

		/*jdoc{
			"name" : "vector< int > getIntVector( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node"
			],
			"returns" : [
				"A vector of int types or an empty vector if the node DNE"
			],
			"desc" : "See getStringVector(...).
			Converts a comma separated list into a vector of int types.
			Uses atoi(...) for string to int conversion."
		}*/
		vector<int> getIntVector( string nodePath );

		/*jdoc{
			"name" : "double getDouble( string nodePath, double def = 0 )",
			"params" : [
				"nodePath", "def"
			],
			"paramDesc" : [
				"Path to node", "Optional: Default value for node DNE"
			],
			"returns" : [
				"The nodes value converted to a double or the default if the node DNE."
			],
			"desc" : "Gets a nodes value and returns it as an double type"
		}*/
		double getDouble( string nodePath, double def = 0 );

		/*jdoc{
			"name" : "vector< double > getDoubleVector( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node"
			],
			"returns" : [
				"A vector of double types or an empty vector if the node DNE"
			],
			"desc" : "See getStringVector(...).
			Converts a comma separated list into a vector of int types.
			Uses atof(...) for string to double conversion."
		}*/
		vector<double> getDoubleVector( string nodePath );

		float getFloat( string nodePath, float def = 0 );
		
		/*jdoc{
			"name" : "bool getBool( string nodePath, bool def = false )",
			"params" : [
				"nodePath", "def"
			],
			"paramDesc" : [
				"Path to node", "Optional: Default value for node DNE"
			],
			"returns" : [
				"The node value converted to a bool or the default if the node DNE.",
				"True: Int values &gt;= 1 or case insensitive \"true\"  ",
				"False: Int values &lt; 1 or case insensitive \"false\"  "
			],
			"desc" : "Gets a nodes value and returns it as an int type.
			The strings \"true\" and \"false\" are also converted to the corresponding bool values.
			The case is not important. This allows xml files to be easier to read."
		}*/
		bool getBool( string nodePath, bool def = false );

		/*jdoc{
			"name" : "bool exists( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node"
			],
			"returns" : [
				"True: node exists",
				"False: node does not exist"
			],
			"desc" : "Used to determine if a node exists"
		}*/
		bool exists( string nodePath );

		
		vector<string> childrenOf( string nodePath, int depth = -1, bool attrs = false );
		vector<string> attributesOf( string nodePath );
		vector<string> getNodes( string nodePath );

		/**
		 * Helper Methods
		 */
		vector<string> split(const string &s, char delim);
		std::string trim(const std::string& str, const std::string& whitespace = " \t\n");

		

		string tagName( string nName );
		string attributeName( string nName );

	protected:

		string manualToLower( string str );
		void makeMap( TXMLNode * parent, string cp = "" );
		string sanitize( string nodePath );

		vector<string> vectorFromString( string data );
		vector<string> &split(const string &s, char delim, vector<string> &elems);
		map<string, string> attributes( TXMLNode * node );
		
	};
}





#endif