#include "ConfigFunction.h"
#include "Utils.h"

namespace jdb {

	int ConfigFunction::instances = 0; 

	ConfigFunction::ConfigFunction( XmlConfig * cfg, string nodePath, string pMod, string eMod, string formMod ){

		string formula = cfg->getString( nodePath + formMod );
		func = unique_ptr<TF1> (new TF1( ("xmlfunction_" + ts(instances)).c_str(), formula.c_str() ) );
	 
		int p = 0;
		while ( cfg->exists( nodePath + pMod + ts(p) ) ){
			double val = cfg->getDouble( nodePath + pMod + ts(p) );
			
			func->SetParameter( p, val );

			// set errors if they are there
			if ( cfg->exists( nodePath + eMod + ts(p) ) ){
				double error = cfg->getDouble( nodePath + eMod + ts(p) );
				func->SetParError( p, error );

				DEBUG( "p" << p << " = " << val << " +/- " << error )
			} else {
				DEBUG( "p" << p << " = " << val )
			}

			p++;
		}
	}
}
