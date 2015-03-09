#ifndef CONFIG_POINT_H
#define CONFIG_POINT_H

#include "XmlConfig.h"
#include "Utils.h"
#include <memory>

namespace jdb{
	
	/*Config Point easily loads (x, y) points from XmlConfigs
	 *
	 * This class is a utility class used for quickly loading and aliasing point data in an xml config
	 *
	 * **Headers** : ConfigPoint.h
	 */
	class ConfigPoint
	{
	public:

		// x value
		double x;
		// y value
		double y;

	
		/*Load point data from an XmlConfig
		 *@cfg XmlConfig instance to load data from
		 *@nodePath Path to the XmlNode containg point data
		 *@dX Default = 0 : default x value if not found in node
		 *@dY Default = 0 : default y value if not found in node
		 *@attrX Default = ":x" : attribute tag to search for x value
		 *@attrY Default = ":y" : attribute tag to search for y value
		 *
		 * This class reads in data from an XmlConfig instance. The underlying xml should contain a node at *nodePath* like 
		 * ``` xml
		 * <Point name="danny" x="100" y=":100" />
		 * ```
		 */
		ConfigPoint( XmlConfig * cfg, std::string nodePath, double dX = 0, double dY = 0, string attrX = ":x", string attrY = ":y"){
			if ( cfg ){
				x = cfg->getDouble( nodePath + attrX, dX );
				y = cfg->getDouble( nodePath + attrY, dY );
			} else {
				x = dX;
				y = dY;
			}
		}
		~ConfigPoint(){}

		/* Outputs the point as a human readable string
		 *
		 * @return A string representation of the point data in the format "( x, y )"
		 */
		string toString(){
			return ("( " + dts( x ) + ", " + dts( y ) + " )");
		}
	};

}




#endif