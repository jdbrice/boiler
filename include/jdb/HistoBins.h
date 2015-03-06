#ifndef HISTOGRAM_BINS
#define HISTOGRAM_BINS

#include "XmlConfig.h"
#include "Utils.h"

#include <memory>

namespace jdb{
	/*jdoc{
		"class" : "HistoBins",
		"desc" : "Container and utility class for histogram style binning"
	}*/
	/* HistoBins provides a container for storing and using bins
	 *
	 * Provides a stand-alone binning container. Can be create directly from configs.
	 */
	class HistoBins
	{
	public:
		
		/* Makes a vector of bins with fixed width
		 * Divides the range high - low into a fixed number of bins from low to high"
		 */
		static vector<double> makeNBins( int nBins, double low, double high ){

			vector<double> bins;
			double step = (high - low ) / (double) nBins;
			for (double i = low; i <= high; i += step ){
				bins.push_back( i );
			}
			return bins;
		}	// binsFrom
		
		/*jdoc{
			"name" : "static vector<double> makeFixedWidthBins( double binWidth, double low, double high )",
			"params" : ["double binWidth", "double low", "double high" ],
			"paramDesc" : [	"Nominal width of each bin", 
			"Lower edge of first bin", 
			"Upper edge of last bin" ],
			"returns" : [ "vector of bin edges" ],
			"desc" : "Divides the range high - low into a fixed number of bins from low to high"
		}*/
		static vector<double> makeFixedWidthBins( double binWidth, double low, double high ){

			vector<double> bins;
			for (double i = low; i <= high; i += binWidth ){
				bins.push_back( i );
			}
			return bins;
		}	// binsFrom

		/*jdoc{
			"name" : "static int findBin( vector<double> &bins, double val )",
			"params" : [
				"&bins",
				"val"
			],
			"paramDesc" : [
				"Bins to search in",
				"Search value"
			],
			"returns" : [
				"-1 : Underflow",
				"-2 : Overflow",
				"-3 : Not Found",
				"Otherwise the zero-indexed bin is returned"
			],
			"desc" : "Finds the bin corresponding to a value in the given bins"
		}*/
		static int findBin( vector<double> &bins, double val ){

			if ( bins.size() < 2 )
				return -1;

			int n = bins.size();

			// overflow and underflow
			if ( val < bins[ 0 ] )
				return -1;
			else if ( val > bins[ n - 1 ] )
				return -2;

			for ( int i = n-2; i >= 0; i-- ){
				if ( val >= bins[ i ] )
					return i;
			}

			return -3;

		}	// findBin

		/*jdoc{
			"name" : "int findBin( double val )",
			"params" : [
				"val"
			],
			"paramDesc" : [
				"Search value"
			],
			"returns" : [
				"-1 : Underflow",
				"-2 : Overflow",
				"-3 : Not Found",
				"Otherwise the zero-indexed bin is returned"
			],
			"desc" : "Finds the bin corresponding to a value in the object bins"
		}*/
		int findBin( double val ){
			return findBin( bins, val );
		} // findBin

		/*
		 * Length of underlying vector containing the bin edges
		 */
		int length() {
			return bins.size();
		}
		/*
		 * Size of underlying vector containing the bin edges
		 */
		int size() {
			return bins.size();
		}
		/*
		 * Number of bins stored in the underlying vector of bin edges. Equal to size() - 1.
  		 */
		int nBins(){
			return bins.size() - 1;
		}

		/*
			Creates bins with a nominal bin width. The final bin may be smaller than the nominal size if width does not evenly divide the range
		}*/
		HistoBins( double min, double max, double width ){
			this->bins = makeFixedWidthBins( width, min, max );
		}

		
		HistoBins( vector<double> bins ){
			this->bins = bins;
		}

		/*jdoc{
			"name" : "HistoBins( XmlConfig * config, string nodePath, string mod = \"\" )",
			"params" : [
				"config", "nodePath", "mod"
			],
			"paramDesc" : [
				"Project config", "Path to configuration node", "Optional: Attribute modifier"
			],
			"returns" : [
				
			],
			"desc" : "Creates histogram bins from a config node. Used heavily by HistoBook for config based histogram creation. Attribute modifier is used to select, x, y, etc. from the node."
		}*/
		HistoBins( XmlConfig * config, string nodePath, string mod = "" ){

			if ( config->exists( nodePath ) && config->getDoubleVector( nodePath ).size() >= 2 ){
				bins = config->getDoubleVector( nodePath );
				min = bins[ 0 ];
				max = bins[ nBins() ];
				width = -1;
				return;
			}  

			string wTag 	= ":width" + mod;
			string minTag 	= ":min" + mod;
			string maxTag 	= ":max" + mod;
			

			if ( config->exists( nodePath + wTag ) && config->exists( nodePath + minTag ) && config->exists( nodePath + maxTag ) ) {

				min = config->getDouble( nodePath + minTag );
				max = config->getDouble( nodePath + maxTag );
				width = config->getDouble( nodePath + wTag );

				bins = makeFixedWidthBins( width, min, max );
				return;
			}
		
			string nTag 	= ":nBins" + mod;
			if ( config->exists( nodePath + nTag ) && config->exists( nodePath + minTag ) && config->exists( nodePath + maxTag ) ) {

				min = config->getDouble( nodePath + minTag );
				max = config->getDouble( nodePath + maxTag );
				double n = config->getInt( nodePath + nTag );
				width = (max - min ) / (double)n;

				bins = makeNBins( n, min, max );
				return;
			}

		}

		/*jdoc{
			"name" : "double& operator[] ( const int nIndex )",
			"params" : [
				"nIndex"
			],
			"paramDesc" : [
				"Index to retrieve from the underlying vector of bin edges"
			],
			"returns" : [
				"Bin edge indexed by nIndex"
			],
			"desc" : "Gets a bin edge from the underlying vector of bin edges"
		}*/
		double& operator[] ( const int nIndex ){
			return bins[ nIndex ];
		}

		/*jdoc{
			"name" : "string toString ()",
			"params" : [
				
			],
			"paramDesc" : [
				
			],
			"returns" : [
				"String representation of bins"
			],
			"desc" : ""
		}*/
		string toString() {
			if ( width > 0 ) 
				return "< " + ts( nBins() ) + " bins ( " + ts(min) + " -> " + ts(max) + " )  >";
			else {
				string ba = "< " + ts( nBins() ) + " bins { ";

				for ( int i = 0; i < bins.size(); i++  ){
					if ( i+1  < bins.size() )
						ba += ("( " + dts( bins[i] ) +" -> " + dts( bins[i+1] ) + " )" );
					if ( i+3  < bins.size() )
						ba += ", ";
				}
				ba += " } >";
				return ba;
			}

			return "";
		}


		~HistoBins(){}
		
		vector<double> getBins(){ return bins; }

		vector<double> bins;
		double width;
		double min, max;

	};

	typedef unique_ptr<HistoBins> uptrHistoBins;
}






#endif