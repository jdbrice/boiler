#ifndef CONFIG_RANGE_H
#define CONFIG_RANGE_H

#include "XmlConfig.h"
#include "Utils.h"

namespace jdb{

	/*jdoc{
		"class" : "ConfigRange",
		"desc" : "A range that can easily be initialized from a config file"
	}*/
	class ConfigRange
	{
	public:
		//jdoc{ "name" : "double min", "desc" : "Minimum value in range"}
		double min;
		//jdoc{ "name" : "double max", "desc" : "Maximum value in range"}
		double max;

		/*jdoc{
			"name" : "ConfigRange( XmlConfig * cfg, string np, double dMin = 0, double dMax = 0, string attrMin = \":min\", string attrMax = \":max\")",
			"params" : [
				"cfg", "nodePath", "dMin", "dMax", "attrMin", "attrMax"
			],
			"paramDesc" : [
				"Project config",
				"Path to root node in config",
				"Optional: Default min value",
				"Optional: Default max value",
				"Optional: Attribute to read from for min value",
				"Optional: Attribute to read from for max value"
			],
			"returns" : [
				
			],
			"desc" : "Creates a range object from the project config. If values are not in the config the variables are initialized and set to the default"
		}*/
		ConfigRange( XmlConfig * cfg, string np, double dMin = 0, double dMax = 0, string attrMin = ":min", string attrMax = ":max"){
			if ( cfg ){
				min = cfg->getDouble( np + attrMin, dMin );
				max = cfg->getDouble( np + attrMax, dMax );
			} else {
				min = dMin;	
				max = dMax;
			}
		}
		~ConfigRange(){}

		/*jdoc{
			"name" : "string toString()",
			"params" : [
				
			],
			"paramDesc" : [
				
			],
			"returns" : [
				"String representation of the range"
			],
			"desc" : "Example: \"( 0.0 -> 14.0 )\" "
		}*/
		string toString(){
			return ("( " + dts( min ) + " -> " + dts( max ) + " )");
		}
		
	};

}


#endif