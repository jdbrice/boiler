#include "TreeMap.h"

#include "TLeafElement.h"

namespace jdb{

	TreeMap::TreeMap( TChain * chain ) {

		lg.setClassSpace( "TreeMap" );

		if ( 0 == chain )
			lg.info(__FUNCTION__) << " Not a valid chain" << endl;

		this->chain = chain;

		mapTree();

		lg.info(__FUNCTION__) << endl;

	}

	TreeMap::~TreeMap(){

		lg.info(__FUNCTION__) << endl;
		

		typedef std::map<string, Int_t*>::iterator sip_it_type;
		for(sip_it_type iterator = intArrays.begin(); iterator != intArrays.end(); iterator++) {

			if ( iterator->second ){
				lg.trace(__FUNCTION__) << "Releasing " << iterator->first << endl;
				delete[] (intArrays[ iterator->first ]);
			}

		}

		lg.info(__FUNCTION__) << endl;
	}

	void TreeMap::mapTree(){

		lg.info( __FUNCTION__ ) << endl;

		nEntries = chain->GetEntries();
		nTrees = chain->GetNtrees();

		TObjArray * lfs = chain->GetListOfLeaves();
		int l = lfs->GetEntries();

		for ( int i = 0; i < l; i++ ){

			// get info on this leaf
			string tName = ((TLeafElement*)lfs->At( i ))->GetTypeName();
			string name = ((TLeafElement*)lfs->At( i ))->GetName();
			string title = ((TLeafElement*)lfs->At( i ))->GetTitle();

			type[ name ] = tName;

			// look for its size if it is an array
			string::size_type open = title.find( "[", 0 );
			string::size_type close = title.find( "]", 0 );
			
			if( open != string::npos && close != string::npos ) {
				lengthName[ name ] = title.substr( open + 1, ( close - open - 1 ) );
				isArray[ name ] = true;
			} else {
				isArray[ name ] = false;
			}
		}

		findLengths();
		setAddresses();
	}

	void TreeMap::findLengths() {

		lg.info( __FUNCTION__ ) << endl;

		typedef std::map<string, string>::iterator ss_it_type;
		for(ss_it_type iterator = lengthName.begin(); iterator != lengthName.end(); iterator++) {
		    
		    string lp = iterator->second;
		    
		    if ( maxLength[ lp ] <= 0 ){
		    	int max = findMax( lp );
		    	lg.info(__FUNCTION__) << "Max for " << lp << " === " << max << endl;

		    	if ( 0 >= max )
		    		max = atoi( lp.c_str() );

		    	maxLength[ lp ] = max;
		    }
		}

	}

	int TreeMap::findMax( string name ){
		
		int nSamples = nTrees * 2;
		if ( nTrees >= 10 )
			nSamples = 10;
		int step = (nEntries-1) / nSamples;

		int finalMax = 0;
		for ( int i = 0; i < nSamples; i ++ ){

			if ( chain->LoadTree( i * step ) >=0 ) {
				int cMax = chain->GetTree()->GetMaximum( name.c_str() );
				if ( cMax > finalMax )
					finalMax = cMax;
			}

		}
		return finalMax;
	}


	void TreeMap::setAddresses(){

		TObjArray * lfs = chain->GetListOfLeaves();
		int l = lfs->GetEntries();

		for ( int i = 0; i < l; i++ ){
			string tName = ((TLeafElement*)lfs->At( i ))->GetTypeName();
			string name = ((TLeafElement*)lfs->At( i ))->GetName();
			string title = ((TLeafElement*)lfs->At( i ))->GetTitle();

			// if it is an array then get the size
			int aSize = -1;
			if ( isArray[ name ]  ){
				lg.trace(__FUNCTION__) << "Size of " << name << " = " << maxLength[ lengthName[ name ] ] << endl; 
				aSize = maxLength[ lengthName[ name ] ];
			}

			if ( 0 >= aSize && isArray[ name ] ){
				lg.warn(__FUNCTION__) << "Zero size for array " << name << endl;
			}


			if ( isArray[ name ] )
				lg.info(__FUNCTION__) << "Addressing : " << tName << " " << name << "[" << lengthName[ name ] << " === " << maxLength[ lengthName[ name ]] << "]" << endl;
			else
				lg.info(__FUNCTION__) << "Addressing : " << tName << " " << title << endl;

			/**
			 * Blocks for each data type
			 */
			// Int_t
			if ( "Int_t" == tName ){

				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					intArrays[ name ] = new Int_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(intArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					ints[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(ints[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}
			// UInt_t
			if ( "UInt_t" == tName ){
				
				
				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					uintArrays[ name ] = new UInt_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(uintArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					uints[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(uints[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}
			// Char_t
			if ( "Char_t" == tName ){
				
				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					charArrays[ name ] = new Char_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(charArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					chars[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(chars[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}
			// UChar_t
			if ( "UChar_t" == tName ){
				
				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					ucharArrays[ name ] = new UChar_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(ucharArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					uchars[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(uchars[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}
			// Short_t
			if ( "Short_t" == tName ){
				
				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					shortArrays[ name ] = new Short_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(shortArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					shorts[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(shorts[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}
			// UShort_t
			if ( "UShort_t" == tName ){
				
				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					ushortArrays[ name ] = new UShort_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(ushortArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					ushorts[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(ushorts[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}
			// Float_t
			if ( "Float_t" == tName ){
				
				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					floatArrays[ name ] = new Float_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(floatArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					floats[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(floats[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}
			// Double_t
			if ( "Double_t" == tName ){
				
				branches[ name ] = 0;
				
				if ( 0 < aSize && isArray[ name ] ){
					doubleArrays[ name ] = new Double_t[ aSize ];
					chain->SetBranchAddress( name.c_str(), &(doubleArrays[ name ][0]), &(branches[ name ] ) );
					usable[ name ] = true;
				} else {
					// not an array, just a single var
					doubles[ name ] = 0;
					chain->SetBranchAddress( name.c_str(), &(doubles[ name ]), &(branches[ name ] ) );
					usable[ name ] = true;
				}
			}

		}

	}

	Double_t TreeMap::get( string name, int index  ){

		if ( !usable[ name ])
			return numeric_limits<double>::quiet_NaN();

		if ( !isArray[ name ]){
			if ( "Int_t" == type[ name ] )
				return (double)ints[ name ];
			else if ( "UInt_t" == type[ name ] )
				return (double)uints[ name ];
			else if ( "Char_t" == type[ name ] )
				return (double)chars[ name ];
			else if ( "UChar_t" == type[ name ] )
				return (double)uchars[ name ];
			else if ( "Short_t" == type[ name ] )
				return (double)shorts[ name ];
			else if ( "UShort_t" == type[ name ] )
				return (double)ushorts[ name ];
			else if ( "Float_t" == type[ name ] )
				return (double)floats[ name ];
			else if ( "Double_t" == type[ name ] )
				return 0;
		} else if ( index >= 0 ) {

			if ( "Int_t" == type[ name ] )
				return (double)intArrays[ name ][ index ];
			else if ( "UInt_t" == type[ name ] )
				return (double)uintArrays[ name ][ index ];
			else if ( "Char_t" == type[ name ] )
				return (double)charArrays[ name ][ index ];
			else if ( "UChar_t" == type[ name ] )
				return (double)ucharArrays[ name ][ index ];
			else if ( "Short_t" == type[ name ] )
				return (double)shortArrays[ name ][ index ];
			else if ( "UShort_t" == type[ name ] )
				return (double)ushortArrays[ name ][ index ];
			else if ( "Float_t" == type[ name ] )
				return (double)floatArrays[ name ][ index ];
			else if ( "Double_t" == type[ name ] )
				return 0;

		}

		return numeric_limits<double>::quiet_NaN();

	}

	Double_t TreeMap::operator[](string name ){

		if ( isArray[ name ] || !usable[ name ] )
			return numeric_limits<double>::quiet_NaN();

		return get( name );
	}


}