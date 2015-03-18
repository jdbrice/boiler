#ifndef HISTOGRAM_BINS
#define HISTOGRAM_BINS

#include "XmlConfig.h"
#include "Utils.h"

#include <memory>

namespace jdb{
	
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
		
		/* Makes a vector of bin edges for bins of a fixed width
		 * @binWidth the width of each bin. The last bin may be smaller.
		 * @low the low edge of the bins
		 * @high the high edge of the bins
		 * @return vector of bin edges from low to high
		 */
		static vector<double> makeFixedWidthBins( double binWidth, double low, double high ){

			vector<double> bins;
			for (double i = low; i <= high; i += binWidth ){
				bins.push_back( i );
			}
			return bins;
		}	// binsFrom

		/* Finds the bin containing a given value
		 * @bins The bin edges to search in
		 * @val The value for which the corresponding bin is desired 
		 */
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

		/* Finds the bin containing a given value
		 * @val The value for which the corresponding bin is desired 
		 */
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

		/* Creates a HistoBins object from a vector of bin edges
		 *
		 */
		HistoBins( vector<double> bins ){
			this->bins = bins;
		}

		/*Creates histogram bins from an xml config node 
		 * Used heavily by HistoBook for config based histogram creation. 
		 *
		 * Example node:
		 * ``` xml
		 *  <Bins width="1" min="0" max="800" />
		 *  <Bins nBins="10" min="0" max="800" />
		 *  <Bins>10, 12, 14, 16, 18, 20</Bins>
		 * ```
		 */
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

		/* Gets a bin edge from the underlying vector of bin edges"
		 * @nIndex the index of the bin edge to get
		 */
		double& operator[] ( const int nIndex ){
			return bins[ nIndex ];
		}

		/* Converts the bin edges into a human readable string
		 * @return A string represenation of the bin edges.
		 */
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

		/* Destructor
		 *
		 */
		~HistoBins(){}
		
		// Returns the vector of bin edges
		vector<double> getBins(){ return bins; }

		// Vector of bin edges
		vector<double> bins;
		// Width of bins if fixed with or fixed number of bins is used
		double width;
		// min and max of bin range
		double min, max;

	};

}






#endif