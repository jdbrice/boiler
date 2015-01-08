#ifndef CONFIG_RANGE_H
#define CONFIG_RANGE_H

#include "XmlConfig.h"
#include "Utils.h"

namespace jdb{

	/*ConfigRange easily loads (min -> max) ranges from XmlConfigs
	 *
	 * This class is a utility class used for quickly loading and aliasing range data in an xml config
	 *
	 * **Headers** : ConfigRange.h
	 */
	class ConfigRange
	{
	public:
		
		//Minimum value in range
		double min;
		//Maximum value in range
		double max;

		/*Load range data from an XmlConfig
		 *@cfg XmlConfig instance to load data from
		 *@nodePath Path to the XmlNode containg range data
		 *@dMin Default = 0 : default x value if not found in node
		 *@dMax Default = 0 : default y value if not found in node
		 *@attrMin Default = ":min" : attribute tag to search for x value
		 *@attrMax Default = ":max" : attribute tag to search for y value
		 *
		 * This class reads in data from an XmlConfig instance. The underlying xml should contain a node at *nodePath* like 
		 * ``` xml
		 * <Range name="danny" min="100" max=":100" />
		 * ```
		 */
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

		/* Outputs the range as a human readable string
		 *
		 * @return A string representation of the range data in the format "( min -> max )"
		 */
		string toString(){
			return ("( " + dts( min ) + " -> " + dts( max ) + " )");
		}
		
	};

}


#endif