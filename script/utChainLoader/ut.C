

{

	using namespace jdb;
	Logger::setGlobalLogLevel( "all" );
	Logger::setGlobalColor( true );


	TChain * c = new TChain( "V0PicoDst" );

	ChainLoader::loadListRange( c, "list.lis", 0, 10 );


}