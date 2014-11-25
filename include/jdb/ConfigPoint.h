#ifndef CONFIG_POINT_H
#define CONFIG_POINT_H

#include "XmlConfig.h"

namespace jdb{
	/*jdoc{
		"class" : "ConfigPoint",
		"desc" : "A point that can easily be initialized from a config file"
	}*/
	class ConfigPoint
	{
	public:

		//jdoc{ "name" : "double x", "desc" : ""}
		double x;
		//jdoc{ "name" : "double y", "desc" : ""}
		double y;

		/*jdoc{
			"name" : "ConfigPoint( XmlConfig * cfg, string nodePath, double dX = 0, double dY = 0, string attrX = \":x\", string attrY = \":y\")",
			"params" : [
				"cfg",
				"nodePath",
				"dX", 
				"dY",
				"attrX",
				"attrY"

			],
			"paramDesc" : [
				"Project config",
				"Path to root node in config",
				"Optional: Default x value",
				"Optional: Default y value",
				"Optional: Attribute to read from for x value",
				"Optional: Attribute to read from for y value"
			],
			"returns" : [
				
			],
			"desc" : "Creates a point object from the project config. If values are not in the config the variables are initialized and set to the default"
		}*/
		ConfigPoint( XmlConfig * cfg, string np, double dX = 0, double dY = 0, string attrX = ":x", string attrY = ":y"){
			if ( cfg ){
				x = cfg->getDouble( np + attrX, dX );
				y = cfg->getDouble( np + attrY, dY );
			} else {
				x = dX;
				y = dY;
			}
		}
		~ConfigPoint(){}

		/*jdoc{
			"name" : "string toString()",
			"params" : [
				
			],
			"paramDesc" : [
				
			],
			"returns" : [
				"String representation of point"
			],
			"desc" : "Example: \"( 0.0, 14.0 )\" "
		}*/
		string toString(){
			return ("( " + dts( x ) + ", " + dts( y ) + " )");
		}
	};

}


#endif