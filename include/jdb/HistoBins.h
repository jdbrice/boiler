#ifndef HISTOGRAM_BINS
#define HISTOGRAM_BINS

// ROOBARB
#include "XmlConfig.h"
#include "Utils.h"
#include "Logger.h"

	#include "IObject.h"

// STL
#include <memory>
#include <string> 
#include <algorithm>
using namespace std;

namespace jdb{

	enum class BinEdge: int {
		lower,
		upper,
		underflow = -1,
		overflow = -2,
		undefined = -3
	};
	
	/* HistoBins provides a container for storing and using bins
	 *
	 * Provides a stand-alone binning container. Can be create directly from configs.
	 */
	class HistoBins : public IObject
	{
	public:
		
		virtual const char* classname() const { return "HistoBins"; }

		/* Makes a vector of bins with fixed width
		 * Divides the range high - low into a fixed number of bins from low to high"
		 */
		static vector<double> makeNBins( int nBins, double low, double high ){

			vector<double> bins;
			double step = (high - low ) / (double) nBins;
			for (double i = low; i <= high; i += step ){
				bins.push_back( i );
			}
			if ( bins.size() >= 1 && high != bins[ bins.size() - 1 ] ) bins.push_back( high );
			return bins;
		}	// binsFrom
		
		/* Makes a vector of bin edges for bins of a fixed width
		 * @binWidth the width of each bin. The last bin may be smaller.
		 * @low the low edge of the bins
		 * @high the high edge of the bins
		 * @return vector of bin edges from low to high
		 */
		static vector<double> makeFixedWidthBins( double binWidth, double low, double high ){
            DEBUG( "HistoBins", "( binWidth=" << binWidth << ", low="<<low << ", high=" << high << " )" );
			vector<double> bins;
			for (double i = low; i <= high; i += binWidth ){
				bins.push_back( i );
			}
			return bins;
		}	// binsFrom

		/* Finds the bin containing a given value
		 * @bins 		The bin edges to search in
		 * @val 		The value for which the corresponding bin is desired
		 * @includeEdge	The edge to include in the bin, either lower edge or higher edge
		 *
		 * @return 		Underflow 	: -1
		 * 				Overflow 	: -2
		 * 				Undefined 	: -3
		 * 				Otherwise 	: Bin Index starting at 0
		 */
		static int findBin( vector<double> &bins, double val, BinEdge includeEdge = BinEdge::lower ){
            string sbe = "invalid";
            if ( BinEdge::lower == includeEdge )
                sbe = "lower";
            else
                sbe = "upper";
            DEBUG( "HistoBins", "( vector<double> , value=" << val << ", binEdge=" << sbe << " ) " );


			int n = bins.size();

			if ( n < 2 )
				return -3;

			// overflow and underflow
			if ( includeEdge == BinEdge::lower ){
				if ( val < bins[ 0 ] )
					return -1;
				else if ( val >= bins[ n - 1 ] )
					return -2;
			} else if ( includeEdge == BinEdge::upper ){
				if ( val <= bins[ 0 ] )
					return -1;
				else if ( val > bins[ n - 1 ] )
					return -2;
			} else {
				return -3;
			}


			// if not overflow / underflow
			// then find the corresponding bin
			for ( int i = n-2; i >= 0; i-- ){
				if ( val >= bins[ i ] && includeEdge == BinEdge::lower )
					return i;
				else if ( val > bins[ i ] && includeEdge == BinEdge::upper )
					return i;
			}

			return -3;

		}	// findBin

		/* Bin Width from vector of bin edges and bin index
		 * @bins 		vector of bin edges
		 * @binIndex 	bin Index
		 * @return 		bin width or 0.0 for error (also issues an ERROR(...) log)
		 */
		static double binWidth( vector<double> &bins, unsigned int binIndex = 0 ){

			if ( binIndex >= bins.size() ){
				ERROR( "HistoBins", "Bin Index " << binIndex << " out of range ( 0, " << bins.size() << " )" );
				return 0.0;
			}
			return (bins[ binIndex + 1 ] - bins[ binIndex ]);
		}

		/* Finds the bin containing a given value
		 * @val The value for which the corresponding bin is desired 
		 * @includeEdge 	which edge of the bin to include, upper or lower
		 * @return 			the index of the bin see static implementation for more detail
		 */
		int findBin( double val, BinEdge includeEdge = BinEdge::lower ){
			return findBin( bins, val, includeEdge );
		} // findBin

		/* Bin Width
		 * @binIndex 		The index of the bin whose width you want to know
		 * @return 			The bin width or 0.0 for error ( also issues an ERROR(...) log)
		 */
		double binWidth( int binIndex = 0 ){
			return binWidth( bins, binIndex );
		}

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

		/* Constructor : fixed width bins
		 *	Creates bins with a nominal bin width. The final bin may be smaller than the nominal size if width does not evenly divide the range
		 */
		HistoBins( double min, double max, double width ){
			this->bins = makeFixedWidthBins( width, min, max );
		}

		HistoBins() {
			
		}

		/* Creates a HistoBins object from a vector of bin edges
		 *
		 */
		HistoBins( vector<double> bins ){
			this->bins = bins;
            width = -1;
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
		HistoBins( XmlConfig config, string nodePath, string ml = "" ){

			// get the bins as an array of edges
			if ( config.exists( nodePath ) && config.getDoubleVector( nodePath ).size() >= 2 ){
				bins = config.getDoubleVector( nodePath );
				min = bins[ 0 ];
				max = bins[ nBins() ];
				width = -1;
				return;
			}  

			string wt = ":width";
			string mint = ":min";
			string maxt = ":max";
			string nt = ":nBins";

			DEBUG( classname(), "Lowercase! mod is " << ml );

			// Specail case : for modifier "x" also allow no modifier - this makes th histobook able to make 1D histos without a modifier in the xml
			// skip for others so that we dont make the same bins for other modifiers
			if ( "" == ml || "x" == ml ){
				getValuesFromConfig( config, nodePath, wt, nt, mint, maxt );
				if ( goodValues() ){
					
					DEBUG( classname(), "Found HistoBins @ " << nodePath << " with " << wt << nt << mint << maxt );
					
					bins = makeFixedWidthBins( width, min, max );
					return;
				}
			}

			// something like "width_y, max_y, min_y, nBins_y"
			string mml = "_" + ml;
			getValuesFromConfig( config, nodePath, wt + mml, nt + mml, mint + mml, maxt + mml );
			if ( goodValues() ){
				DEBUG( classname(), "Found HistoBins @ " << nodePath << " with " << wt + mml << nt + mml << mint + mml << maxt + mml );
				bins = makeFixedWidthBins( width, min, max );
				return;
			}
			
			
			TRACE( classname(), "Could not make HistoBins @ " << nodePath );

		} // Constructor


		void getValuesFromConfig( XmlConfig config, string nodePath, 
			string widthTag =":width", string nBinsTag=":nBins", string minTag = ":min", string maxTag=":max" ){

			DEBUG( classname(), nodePath );
			DEBUG( classname(), widthTag << " " << nBinsTag << " " << minTag << " " << maxTag );
			min = 1;
			max = 0;
			width = 0.0;

			min = config.getDouble( nodePath + minTag, min );
			max = config.getDouble( nodePath + maxTag, max );
			width = config.getDouble( nodePath + widthTag, width );



			if ( config.exists( nodePath + nBinsTag ) ){
				int  n = config.getInt( nodePath + nBinsTag );
				width = ( max - min ) / (double)n;
			}
			DEBUG( classname(), "min=" << min << ", max=" << max << ", width=" << width );
		}

		bool goodValues(){
			if ( max > min && width > 0 )
				return true;
			return false;
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
				return "< " + ts( nBins() ) + " bins ( " + ts(min) + "->" + ts(max) + " )  >";
			else {
				string ba = "< " + ts( nBins() ) + " bins { ";

				for ( unsigned int i = 0; i < bins.size(); i++  ){
					if ( i+1  < bins.size() )
						ba += ("(" + dts( bins[i] ) +"->" + dts( bins[i+1] ) + ")" );
					if ( i+2  < bins.size() )
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
		
		// Returns a copy of the vector of bin edges
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