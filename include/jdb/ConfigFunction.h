#ifndef CONFIG_FUNCTION_H
#define CONFIG_FUNCTION_H

// ROOBARB
#include "XmlConfig.h"
#include "Utils.h"

// STL
#include <memory>

// ROOT
#include "TF1.h"


namespace jdb{

	/* Loads a TF1 or subclass function from an Xml Config
	 *
	 * Loads a function from an XmlConfig.
	 */
	class ConfigFunction {
	protected:
		// The ROOT TF1 object backing the function
		unique_ptr<TF1> func = nullptr;

		// instance count to make sure wee keep a unique ROOT name on our function
		static int instances;
	public:

		/* Creates a function from xml config
		 *
		 * @cfg 		XmlConfig object
		 * @nodePath 	path to node
		 * @pMod 		Default : ':p' - the modifier to get the parameter from node path
		 * @eMod 		Default : ':e' - the modifier to get the parameter error from node path
		 * @formMod 	Default : ':formula' - the modifier to get the formula from node path
		 */
		ConfigFunction( XmlConfig * cfg, string nodePath, string pMod = ":p", string eMod = ":e", string formMod = ":formula");
		/* Destructor
		 *
		 */
		~ConfigFunction() {}

		/* Evaluate the function
		 * Checks for the valid state of function before eval
		 * 
		 * @x 		dependent variable in function
		 * @return 	evaluated value of function or 0.0 if function is undefined
		 */
		double eval( double x ){
			if ( func != nullptr ){
				return func->Eval( x );
			}
			return 0.0;
		}

		string toString(){

			string line = (string)func->GetTitle() + " : ( ";
			for ( int i = 0; i < func->GetNpar(); i++ ){
				line += dts(func->GetParameter( i ));
				if ( i != func->GetNpar() - 1 )
				 line += ", ";
			}
			line += " ) ";
			return line;
		}
	};

}


#endif