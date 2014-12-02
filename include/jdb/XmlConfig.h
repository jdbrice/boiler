#ifndef XML_CONFIG_H
#define XML_CONFIG_H

#ifndef __CINT__
#include "RapidXmlWrapper.h"
#endif

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

/**
 * JDB
 */
#include "Logger.h"

namespace jdb {
	class XmlConfig
	{
	protected:
	#ifndef __CINT__
		RapidXmlWrapper *rxw;
	#endif

		Logger logger;
		
		map<string, bool> nodeExists;
		
		map<string, string> data;

		map<string, bool> isAttribute;
		
		typedef map<string, string>::iterator map_it_type;

		//pjdco{ "name" : "string filename", "desc" : "Filename of the config file"}
		string filename;

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
		XmlConfig( string filename );
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

		/*jdoc{
			"name" : "vector< string > childrenOf( string nodePath, int depth = -1, bool attrs = false )",
			"params" : [
				"nodePath", "depth", "attrs"
			],
			"paramDesc" : [
				"Path to node", "Depth to search for children", "Include attribute paths"
			],
			"returns" : [
				"List of paths to children of the given node. Empty list for no children found"
			],
			"desc" : "Gets a list of paths to all children of a given node. Very useful for automating tasks, building lots of objects, etc."
		}*/
		vector<string> childrenOf( string nodePath, int depth = -1, bool attrs = false );

		/*jdoc{
			"name" : "vector< string > attributesOf( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node"
			],
			"returns" : [
				"List of paths to attributes."
			],
			"desc" : "One path is added for each attribute of the given node or an empty list if no attributes are present. Use attributeName(...) to get only the attribute name from the full path."
		}*/
		vector<string> attributesOf( string nodePath );

		/*jdoc{
			"name" : "vector<string> getNodes( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node"
			],
			"returns" : [
				"List of paths to nodes matching the query"
			],
			"desc" : "Can be used to find nodes matching a conditions
			Case 1 ) Queries like : \"group.node\" returns a list of paths to all nodes of the form \"group.node[0..N]\"
			Case 2 ) Queries like : \"group.node:name\" returns a list of paths to all nodes of the form \"group.node[0..N]\" when they contain a \"name\" attribute 
			Case 3 ) Queries like : \"group.node:name=histo1\" returns a list of paths to all nodes of the form \"group.node[0..N]\" when they contain a name attribute and it equals \"histo1\"
			"
		}*/
		vector<string> getNodes( string nodePath );

		/**
		 * Helper Methods
		 */
		vector<string> split(const string &s, char delim);
		std::string trim(const std::string& str, const std::string& whitespace = " \t\n");

		

		/*jdoc{
			"name" : "string tagName( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to Node"
			],
			"returns" : [
				"The name of the tag pointed to by nodePath"
			],
			"desc" : "Extracts the name of the final node (tag name )
			
			For instnce:
			The node path \"node.sub.a\" yields \"a\" "
		}*/
		string tagName( string nodePath );
		/*jdoc{
			"name" : "string attributeName( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to Node"
			],
			"returns" : [
				"The attribute name only"
			],
			"desc" : "Extracts the attribute name from a full node path.
			
			For instance:
			The node path \"node.sub.a:name\" yields \"name\" "
		}*/
		string attributeName( string nodePath );

	protected:

		string manualToLower( string str );
		string sanitize( string nodePath );

		vector<string> vectorFromString( string data );
		vector<string> &split(const string &s, char delim, vector<string> &elems);

		
	};
}


#endif