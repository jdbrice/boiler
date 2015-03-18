

void LoggerUnitTest(){
	
	Logger logger( Logger::llInfo );

	logger.error() << "Error messages" << endl;
	logger.warn() << "Warning messages" << endl;
	logger.info() << "Info messages" << endl;
	logger.trace() << "Trace messages" << endl;
	logger.debug() << "Debug messages" << endl;

	TaskTimer tt;
	tt.start();

	for ( int i = 0; i < 10000; i++ ){
		logger.info("", false) << " ";
	}
	double loggerOn = tt.elapsed();
	
	tt.start();
	for ( int i = 0; i < 10000; i++ ){
		cout << " ";
	}
	double coutOn = tt.elapsed();

	tt.start();
	for ( int i = 0; i < 10000; i++ ){
		logger.debug("", false) << " ";
	}
	cout << endl << "Logger Off 10K : " << tt.elapsed() << endl;
	cout << "Logger On 10K : " << loggerOn << endl;
	cout << "cout 10K : " << coutOn << endl;



}