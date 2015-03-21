


void DataSourceSpeedTest( string fname = "cDataSourceSpeedTest.xml" ){

	gSystem->Load( "libJDB.so" );
	using namespace jdb;
	
	XmlConfig cfg( fname );

	Logger::setGlobalLogLevel( Logger::llNone );

	DataSource ds( &cfg, "DataSource" );

	TChain * chain = ds.getChain();
	

	cout << "Getting Entries" << endl;
	TaskTimer tt;
	
	int nEvents = ds.getEntries();
	tt.start();
	for ( int j = 0; j < 10; j++ ){
		for ( int i = 0; i < nEvents; i++ ){
			ds.getEntry( i );
			float vX =  ds.get( "vertexX" );
		}
	}
	cout << "Completed in " << tt.elapsedTime() << endl;

	tt.start();
	for ( int j = 0; j < 10; j++ ){
		for ( int i = 0; i < nEvents; i++ ){
			ds.getEntry( i );
			float vX =  ds.getFloat( "vertexX" );
		}
	}
	cout << "Completed in " << tt.elapsedTime() << endl;

	ds.getEntry( 0 );
	Float_t * pvX = (Float_t*)ds.getPointer( "vertexX" );
	tt.start();
	for ( int j = 0; j < 10; j++ ){
		for ( int i = 0; i < nEvents; i++ ){
			ds.getEntry( i );
			
			float vX =  *pvX;
			
			//cout << "vX" << vX << ", " << ds.get("vertexX") <<endl;
		}
	}
	cout << "Completed in " << tt.elapsedTime() << endl;


}