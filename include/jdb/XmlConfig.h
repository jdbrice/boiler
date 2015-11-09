#ifndef XML_CONFIG_H
#define XML_CONFIG_H

#ifndef __CINT__
#include "RapidXmlWrapper.h"
#endif

#include "TObject.h"

// STD
#include <string>
#include <iostream>
#include <vector>
#include <exception>
#include <sstream>
#include <map>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <utility>      // std::pair, std::make_pair

// JDB
#include "Logger.h"

namespace jdb {
	class XmlConfig
	{
	protected:


		// Logger instance
		Logger logger;
		
		// Node existance
		map<string, bool> nodeExists;
		
		// data - map of node path to string representation
		map<string, string> data;

		// does the nodepath point to an attribute value
		map<string, bool> isAttribute;
		
		// typedef of commonly used iterator for the class
		typedef map<string, string>::iterator map_it_type;

		//Filename of the config file
		string filename;

		//The delimiter used for attributes - Default is ":"
		char attrDelim;

		//The delimiter used for paths - Default is "."
		char pathDelim;

		// The delimeter used for equality - Default is '='
		char equalDelim;

		// The delimeter used for maps - Default is "::"
		string mapDelim;

		//The delimeter for index open - Default is "["
		string indexOpenDelim;
		
		//The delimeter for index open - Default is "]"
		string indexCloseDelim;

		// Current node for relative path finding
		string currentNode = "";
	public:

		/* Creates an XmlConfig from the given xml file
		 *@filename The file containg valid xml to load
		 *
		 * Loads the file, parses its content and makes its data available
		 */
		XmlConfig( string filename );
		~XmlConfig();

		void saveXml(string filename);


		/**
		 * Sets the current node so that relative node paths can be used
		 * @nodePath 	Path to Node - this will be prepended the nodePath passed in to lookups
		 *
		 * @return 		Current Node before changing
		 */
		string cn( string nodePath = "" ){
			string tCN = currentNode;
			currentNode = nodePath;
			return tCN;
		}

		/* Same as getString(...) but with the [] operator.
		 * @nodePath See getString(...)
		 * @returns The underlying xml data at nodePath as a string
		 */
		string operator[]( string nodePath );
		
		/* Gets xml node or attribute data as a string
		   @nodePath The path to the desired node from the root node. See examples below.
		   @def **Default** = "" The value to return if the node or attribute is not found

		   For an xml block like
		   ``` xml
			<root>
				<data>
					<category>
						<cut1 name="cut1" />
					</category>
				</data>
			</root>
		   ```
		   The node "<cut1 />" can be accessed using 
		   the nodePath = "data.category.cut1" and its 
		   attribute can be accessed using the nodePath =
		   "data.category.cut1:name"
		   
		   @return The underlying xml data at nodePath as a string
		 */
		string getString( string nodePath, string def = "" );

		/* Gets a vector of strings from a comma delimeter list
		 * @nodePath Path to node. See getString.
		 * Gets a vector from a comma delimeted list. 
		 * For instance, if a node contains "1, 3, 9, 7, 16" then the vector whould contain 5 elements
		 *
		 * @return A vector of strings or an empty vector if the node DNE
		 */
		vector<string> getStringVector( string nodePath );

		/* Gets a node or attribute as integer data
		 * @nodePath Path to node. See getString(...)
		 * @def Default value if the endpoint DNE or conversion to int fails
		 *
		 * Uses atoi(...) to convert string data to builtin type int
		 * @return The underlying xml data at nodePath as an builtin type integer
		 */
		int getInt( string nodePath, int def = 0 );

		/* Gets a vector of integers from a comma delimeted list
		 * @nodePath See getString(...)
		 * See getStringVector(...).
		 * Converts a comma separated list into a vector of int types. 
		 * Uses atoi(...) for string to int conversion.
		 * @return vector of integers, one for each item in the comma delimeted list
		 */
		vector<int> getIntVector( string nodePath );

		/* Gets a map<string, string> from a config node 
		 * @nodePath See getString(...)
		 * Converts a node like :
		 * ``` xml
		 * <Map>
		 * 		from : to,
		 * 		alpha : beta
		 * </Map>
		 * ```
		 * Into a map where map[ "from" ] = "to" and map[ "alpha" ] = "beta".
		 * 
		 */
		map<string, string> getStringMap( string nodePath );
		map<int, int> getIntMap( string nodePath );
		/* Gets a node or attribute as double type
		 * @nodePath Path to node. See getString(...)
		 * @def Default value if the endpoint DNE or conversion to double fails
		 *
		 * Uses atof(...) to convert string data to builtin type double
		 * @return The underlying xml data at nodePath as an builtin type double
		 */
		double getDouble( string nodePath, double def = 0 );

		/* Gets a vector of doubles from a comma delimeted list
		 * @nodePath See getString(...)
		 * See getStringVector(...).
		 * Converts a comma separated list into a vector of double types. 
		 * Uses atof(...) for string to double conversion.
		 * @return vector of doubles, one for each item in the comma delimeted list
		 */
		vector<double> getDoubleVector( string nodePath );

		/* Gets a node or attribute as foat type
		 * @nodePath Path to node. See getString(...)
		 * @def Default value if the endpoint DNE or conversion to foat fails
		 *
		 * Uses atof(...) and cast to convert string data to builtin type foat
		 * @return The underlying xml data at nodePath as an builtin type foat
		 */
		float getFloat( string nodePath, float def = 0 );
		
		/* Gets a node or attribute as bool type
		 * @nodePath Path to node. See getString(...)
		 * @def Default value if the endpoint DNE or conversion to bool fails
		 *
		 * Can be string of any case "true", or "false"
		 * Uses atoi(...)  to convert string data to builtin type bool
		 * @return The underlying xml data at nodePath as an builtin type bool
		 */
		bool getBool( string nodePath, bool def = false );

		/* Determine whether a node exists in the xml or not
		 * @nodePath Path to node. See getString(...)
		 *
		 * Searches the xml structure for the given node.
		 * @return **True** - node or attribute is found. **False** otherwise
		 */
		bool exists( string nodePath );

		/* Lists the children of a node
		 * @nodePath Path to node. See getString(...)
		 * @depth The number of levels to search for children
		 * @attrs Show Attributes of node and children or not
		 * 
		 * Gets a list of paths to all children of a given node. 
		 * Very useful for automating tasks, building lots of objects, etc.
		 *
		 * @return Vector of strings containg paths to each node or attribute
		 */
		vector<string> childrenOf( string nodePath, int depth = -1, bool attrs = false );

		/* Lists the children of a node selecting only a given tag
		 * @nodePath Path to node. See getString(...)
		 * @tagName Filters children of the given tagName
		 * @depth depth to search - not yet implemented
		 * 
		 * Gets a list of paths to all children of a given node of the given tagName type. 
		 * Very useful for automating tasks, building lots of objects, etc.
		 *
		 * 	Just a special case of the more general childrenOf(...)
		 * 
		 * @return Vector of strings containg paths to each node or attribute
		 */
		vector<string> childrenOf( string nodePath, string tagName, int depth = -1);

		/* Get the attributes of a node
		 * @nodePath Path to node. See getString(...)
		 * 
		 * One path is added for each attribute of the given
		 * node or an empty list if no attributes 
		 * are present. Use attributeName( attributePath ) to 
		 * get only the attribute name from the full path.
		 *
		 * @return Vector of strings containg paths to each attribute
		 */
		vector<string> attributesOf( string nodePath );

		/* Find nodes based on search criteria
		 * @nodePath Path to node. See getString(...). May also 
		 * contain conditional parts. See description.
		 *
		 * Can be used to find nodes matching a conditions   
		 * 
		 * **Case 1 )** Queries like : "group.node" returns a list 
		 * of paths to all nodes of the form "group.node[0..N]"   
		 * 
		 * **Case 2 )** Queries like : "group.node:name" returns a 
		 * list of paths to all nodes of the form "group.node[0..N]" 
		 * when they contain a "*name*" attribute   
		 * 
		 * **Case 3 )** Queries like : "group.node:name=value" 
		 * returns a list of paths to all nodes of the 
		 * form "group.node[0..N]" when they contain a "*name*" 
		 * attribute and it equals "*value*"
		 *
		 * @return A Vector of strings. One for each path to a 
		 * node or attribute matching the query
		 */
		vector<string> getNodes( string nodePath );

		/*Splits a string into pieces by the delimeter character
		 *@s Input string to split
		 *@delim delimeting character
		 *
		 *@return A vector of strings, one for each piece after 
		 *splitting on the delimeting character. If the delimeting 
		 *character is not found a zero length Vector is returned.
		 */
		vector<string> split(const string &s, char delim);

		/*Trims characters off the front and back of strings
		 *@str Input string to trim
		 *@whitespace A string containing characters to trim. **Default** = " \t\n".
		 */
		std::string trim(const std::string& str, const std::string& whitespace = " \t\n");

		
		/* Get just the tag name from a full path
		 * @nodePath Path to node. See getString(...)
		 *
		 * Extracts the name of the final node (tag name )
		 * For instnce:
		 * The node path "category.sub.a" yields "a"
		 *
		 * @return the tag name of the node pointed to by nodePath
		 */
		string tagName( string nodePath );

        /* Get the path to the parent of this node
		 * @nodePath Path to node. See getString(...)
		 *
		 * Extracts the path to the parent of this node
		 * For instnce:
		 * The node path "category.sub.a" yields "category.sub"
		 *
		 * @return the path to the parent of given node
		 */
        string pathToParent( string nodePath );
		
		/* Get the attribute name from a full path
		 *@nodePath Path to node. See getString(...)
		 *
		 * Extracts the attribute name from a full node path.
		 * For instance:
		 * The node path "category.sub.a:name" yields "name"
		 *
		 * @return The attribute name if the path contains one.
		 * Empty string otherwise
		 */
		string attributeName( string nodePath );

		/* Determines the depth of a node
		 * @nodePath Path to Node
		 *
		 * Calulates the depth of the node from the root node
		 * @return the depth of the given node. Children of the root node have depth 0
		 */
		int depthOf( string nodePath ){

			nodePath = sanitize( nodePath );
			int depth = 0;
			for ( int i = 0; i < nodePath.length(); i++ ){
				if ( pathDelim == nodePath[ i ] )
					depth++;
			}
			return depth;
		}

		/* Calculates the depth of a node relative to another node
		 * @nodePath Path to node of interest
		 * @relativeTo The node whose depth is considered 0
		 */
		int depthOf( string nodePath, string relativeTo ){
			return depthOf( nodePath ) - depthOf( relativeTo );
		}

        string report( string nodePath = "" );

        void applyOverrides( map< string, string > over );

	protected:

		// A manual case lowing function
		string manualToLower( string str );
		// Sanatizes node paths
		string sanitize( string nodePath );
		// Gets a vector from comma delimeted strings
		vector<string> vectorFromString( string data );
		// Splits strings using the given delim character
		vector<string> &split(const string &s, char delim, vector<string> &elems);

		// A special case version of split used for the map decoding
		// Allows string delimeter
		pair<string, string> stringToPair( string &s, string delim  );



        string pathFromFilename (const string& str) {
            size_t found;
            found=str.find_last_of("/\\");
            //cout << " folder: " << str.substr(0,found+1) << endl;
            //cout << " file: " << str.substr(found+1) << endl;
            return str.substr(0,found+1);
        }

		// 
		void parseIncludes(  );



		ClassDef( jdb::XmlConfig, 1 )
	};

}


#endif