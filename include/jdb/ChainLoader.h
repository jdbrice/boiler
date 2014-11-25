
#ifndef CHAINLOADER_H
#define CHAINLOADER_H

#include "dirent.h"
#include "TChain.h"
#include "Logger.h"



namespace jdb{

	/*jdoc{
		"class" : "ChainLoader",
		"desc" : "Static class used for loading files into TChains. Can be used to load an entire directory, or files from a filelist.lis"
	}
	*/
	class ChainLoader{

	public:
		/*jdoc{
			"name" : "static void load( TChain * chain, string dir, int maxFiles = -1 )",
			"returns" : [],
			"params" : [ 
				"chain",
				"dir",
				"maxFiles"
			],
			"paramDesc" : [
				"A pointer to an initialize TChain ready to add files",
				"The path to the directory containing .root files",
				"Optional: maximum number of files to load into chain"
			],
			"desc" : "Loads .root tree files into a TChain"
		}
		*/
		static void load( TChain * chain, string ntdir, int maxFiles = -1 );
		/*jdoc{
			"name" : "static void loadList( TChain * chain, string listFile, int maxFiles = -1 )",
			"returns" : [],
			"params" : [ 
				"chain",
				"listFile",
				"maxFiles"
			],
			"paramDesc" : [
				"A pointer to an initialize TChain ready to add files",
				"filename of a file containg the list of .root files to load",
				"Optional: maximum number of files to load into chain"
			],
			"desc" : "Loads .root tree files into a TChain - used in parallel jobs"
		}
		*/
		static void loadList( TChain * chain, string listFile, int maxFiles = -1 );
	};


}


#endif

