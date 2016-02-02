

{
	int pass = 0;
	int fail = 0;
	int total = 0;

	using namespace jdb;
	Logger::setGlobalLogLevel( "info" );
	Logger::setGlobalColor( true );

	XmlConfig config( "config.xml" );

	HistoBook book( "test.root", config );

	INFO ( "Starting Unit Test : HistoBook" );

	
	total++;
	book.cd();
	book.makeAll( "Histos" );
	if ( book.size() == 8 ){
		pass++;
		INFO( "PASS : Making Histograms from config file");
		book[ "pt1d" ]->FillRandom( "gaus", 1000 );
	} else {
		ERROR( "FAIL : Making Histograms from config file");
		fail++;
	}


	total++;
	book.clone( "pt1d", "pt_0" );

	if ( true == book.exists( "pt_0" ) ){
		pass++;
		INFO( "PASS : Cloning histograms");
	} else {
		fail++;
		ERROR( "FAIL : Cloning histograms ");
	}

	
	total++;
	book.cd( "dir" );
	book.add( "pt_0", new TH1D( "pt_0", "Pt", 100, -10, 10 ) );
	if ( book.exists( "pt_0" ) ){
		book.get( "pt_0" )->FillRandom( "gaus", 1000 );
		pass++;
		INFO( "PASS : Adding histograms in directory ");
	} else {
		fail++;
		ERROR( "FAIL : Adding histograms in directory ");
	}


	// book.report();


	
	total++;
	book.cd( "inclusive" );
	book.clone( "", "pt1d", "inclusive", "pt1d" );
	if ( book.exists( "pt1d", "inclusive" ) ){
		pass++;
		INFO( "PASS : Cloning histograms in one directory to another");
	} else {
		fail++;
		ERROR( "PASS : Cloning histograms in one directory to another");
	}


	// book[ "inclusive/pt" ]->FillRandom( "gaus", 1000 );

	INFO ( "\n" << book.report() );


	INFO( "PASSED " << pass << " of " << total << " test(s)" );
	if ( fail >= 1 ){
		ERROR( "FAILED " << fail << " of " << total << " test(s)" );
	}

	book.save();
}