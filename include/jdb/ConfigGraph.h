#ifndef CONFIG_GRAPH_H
#define CONFIG_GRAPH_H

// ROOBARB
#include "XmlConfig.h"
#include "Utils.h"

// STL
#include <memory>

// ROOT
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"


class ConfigGraph
{
protected:

	vector<double> x, y, exh, exl, eyh, eyl;
public:
	ConfigGraph( XmlConfig * cfg, string nodePath ){

		x = cfg->getDoubleVector( nodePath + ".x" );
		y = cfg->getDoubleVector( nodePath + ".y" );
	}
	~ConfigGraph(){
		
	}


	double eval( double x_val, string interpolate = "linear" ) {

		int index = -1;
		// first find the two points we are between
		// we assume monotomically increasing values of x
		for ( int iX = 0; iX < x.size() - 1; iX++ ){
			if ( x_val < x[ iX + 1] ){
				index = iX;
				break;
			}
		}

		if ( -1 == index && x_val < x[0] ){
			index = 0; // use the first two values
		} else if ( -1 == index ){
			// TODO: signal error here
			return 0.0;
		}

		if ( "closest" == interpolate ){
			if ( abs( x_val - x[index] ) <= abs( x_val - x[index + 1] ) )
				return y[index];
			else
				return y[index+1];
		}

		if ( "linear" == interpolate ){
			return linterp( x_val, x[index], y[index], x[index+1], y[index+1] );
		}

		// default if no interp method 
		return y[index];

	}

	double linterp( double x, double x0, double y0, double x1, double y1 ){
		return y0 + ( y1 - y0 ) * ( (x - x0) / (x1 - x0) );
	}
	
};

#endif