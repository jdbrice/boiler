#define UNITTEST_NO_CXX
#include "../UnitTest.h"

void ut(){

	using namespace jdb;
	Logger::setGlobalLogLevel( "info" );
	Logger::setGlobalColor( true );

	XmlConfig config( "config.xml" );

	HistoBook book( "test.root", config );
	UT_START( "HistoBook" );

	book.cd();
	book.makeAll( "Histos" );
	
	INFO( "ut", UT_EQ( book.size(), 8 ) << " : Making Histos from config file " );
	
	if ( UT_PASSED )
		book[ "pt1d" ]->FillRandom( "gaus", 1000 );
	

	book.clone( "pt1d", "pt_0" );

	INFO( "ut", UT_TRUE( book.exists( "pt_0" ) ) << " : Cloning Histograms" );
	
	book.cd( "dir" );
	book.add( "pt_0", new TH1D( "pt_0", "Pt", 100, -10, 10 ) );
	INFO( "ut", UT_TRUE( book.exists( "pt_0" ) ) << " : Adding histograms in directory" );
	if ( UT_PASSED )
		book.get( "pt_0" )->FillRandom( "gaus", 1000 );
	

	book.cd( "inclusive" );
	book.clone( "", "pt1d", "inclusive", "pt1d" );

	INFO( "ut", UT_TRUE( book.exists( "pt1de", "inclusive" ) ) << " : Cloning histograms in one directory to another" );

	
	book.save();
	
	INFO ( "Book Contents : \n" << book.report() );

	INFO( "ut", UT_SUMMARY );


}