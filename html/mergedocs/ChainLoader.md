#<cldoc:jdb::ChainLoader>

Static use class for loading .root files into a TChain

Static class used for loading files into TChains.
Can be used to load an entire directory, or files from a filelist.lis

---

**Headers** : ChainLoader.h 
**Source** : ChainLoader.cpp


### ChainLoader Example usage

``` cpp
TChain * chain = new TChain( "treeName" );
ChainLoader::load( chain, "path_to_dir_containg_.roots", maxFilesToLoad );
```

Or if you are running parallel or want to load from a file list use:

``` cpp
TChain * chain = new TChain( "treeName" );
ChainLoader::load( chain, "path_to_dir_listfile.lis", maxFilesToLoad );
```