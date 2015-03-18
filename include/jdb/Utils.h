
#ifndef JDB_UTILS_H
#define JDB_UTILS_H 

#include <string>
#include <iostream>

// for timing functions
#include <time.h>  

// for ttys detection
#include <unistd.h> 

// for round
#include <math.h>

// for logging in the framework
#include "Logger.h"

using namespace std;


namespace jdb{

	

	/**
	 * String utilities that need >= c++ 2011
	 */
	/* Converts an integer to a string
	 * @i integer to convert
	 * @len the maximum length of string
	 * @return the string representation of the value
	 */
	inline std::string ts( int i, int len = -1){
		if ( len <= -1 )
			return (to_string( (long long int) i));
		else
			return (to_string( (long long int) i)).substr( 0, len );
	}
	/* Converts a double to a string
	 * @d double to convert
	 * @len the maximum length of string
	 * @return the string representation of the value
	 */
	inline std::string ts( double d, int len = -1){
		if ( len <= -1 )
			return to_string( (long double) d);
		else 
			return (to_string( (long double) d)).substr( 0, len );
	}
	/* Converts a float to a string
	 * @f float to convert
	 * @len the maximum length of string
	 * @return the string representation of the value
	 */
	inline std::string ts( float f, int len  = -1){
		return ts( (double) f, len );
	}
	/* Converts an unsigned integer to a string
	 * @u unsigned int to convert
	 * @len the maximum length of string
	 * @return the string representation of the value
	 */
	inline std::string ts( unsigned int u, int len  = -1){
		if ( len <= -1 )
			return to_string( (long long unsigned int) u);
		else 
			return (to_string( (long long unsigned int) u)).substr( 0, len );
	}

	/* Removes trailing zeros
	 * @in string representation of a number
	 * @return The input string with trailing zeros removed
	 */
	inline std::string trimTrailingZeros( string in ) {
		string r = in;
		while ( r[ r.size() - 1 ] == '0' && r.size() > 1 && r[ r.size() - 2 ] != '.' ){
			r = r.substr( 0, r.size() - 1 );
		}
		return r;
	}
	/* Converts a double to a string and trims trailing zeros
	 * @d double to convert
	 * @return the string representation of the value
	 */
	inline std::string dts( double d ){
		return trimTrailingZeros( ts( d ) );
	}


	/* Task Timer
	 * Utility Class for timing tasks and reporting
	 */
	class TaskTimer{
	public:

		// start time of task
		clock_t startTime;
		// set the start time to now
		void start( ) { startTime = clock(); }
		/* Return the amount of time elapsed since start() was called
		 * @return Time past in seconds as double
		 */
		double elapsed( ) { return ( (clock() - startTime) / (double)CLOCKS_PER_SEC ); }
		/* Return the elapsed time as a string
		 * Formats the string to be human readable
		 * @return The amount of elapsed time
		 */
		string elapsedTime() { 
			int nDig = 3;
			double et = elapsed();
			if ( et >= 1 )
				nDig = ceil( log10( et ) ) + 2;

			return  ts(et, nDig) + " sec";
		}

	};
	
	/**
	 * Plain text progress bar that only updates when it needs to
	 * @i the current step in a loop
	 * @nevents the maximum number of steps
	 * @textWidth The maximum width of the bar in characters
	 * @elapsedTime shows the elapsed time if positive
	 * 
	 */
	inline void progressBar( int i, int nevents, int textWidth = 60, double elapsedTime = -1 ){
		
		// for output
		double progress =  ((double)i / (double)nevents);
		if ( i == nevents - 1)
    		progress = 1.001;


    	int res = (int)((double)nevents / (double) textWidth);
    	if ( res == 0 || res == 1)
    		res = i;

    	// decide when to update
    	if ( i == 0 || (i % res ) == 0 || i == nevents - 1  ){
			// skip for non interactive output
			if (!isatty(fileno(stdout)) && progress <= 1 )
				return;

			double per = progress  * 100;
			per = round( per );

			
			cout << "[";
	    	for ( int ip = 0; ip < textWidth; ip ++ ){
	    		if ( ip < round( (progress * (double)textWidth) ) )
	    			cout << "=";
	    		else 
	    			cout << " ";
	    	}
	    	if (isatty(fileno(stdout)) ){ 
		 	   	cout << "]" << per << "%" ;
				if ( elapsedTime >= 0 ){
					
					int nDig = 3;
					
					if ( elapsedTime >= 1 )
						nDig = ceil( log10( elapsedTime ) ) + 2;

					cout << " : " << ts(elapsedTime, nDig) << " sec ";
				}
				cout << "\r";
				std::cout.flush();
				if (progress > 1)
					cout << "[" << endl;
			} else {
					cout << "]" << per << "%" << "\n";
			}
		}
	}


	/* Task Progress
	 * Integrates the task timer and progress bars into a single utility
	 */
	class TaskProgress{
	protected:
		// Maximum number of steps
		int max;
		// width of the progress bar
		int barWidth;
		// Show the title of the task
		bool showTitle;
		// Show the elapsed time
		bool showElapsed;
		// Title of the task
		string title;
		// Task Timer
		TaskTimer tt;

	public:
		/* Default Constructor
		 * 
		 */
		TaskProgress(){

		}

		/* Creates a TaskProgress
		 * @title Title of the Task
		 * @max Maximum number of steps
		 * @width The width of the progress bar
		 * @sTitle Show th title
		 * @sElapse Show elapsed time
		 * 
		 */
		TaskProgress( string title, int max = 100, int width = 60, bool sTitle = true, bool sElapse = true ){
			this->title = title;
			this->max = max;
			barWidth = width;
			showTitle = sTitle;
			showElapsed = sElapse;
			title = "";
		}

		/* Called inside a loop to show the progress of the current task
		 * @i The step in the current loop
		 */
		void showProgress( int i ){
			if ( 0 == i){
				tt.start();
				if ( showTitle )
					cout << "Running Task : " << title << endl;
			}

			if ( showElapsed )
				progressBar( i, max, barWidth, tt.elapsed() );
			else 
				progressBar( i, max, barWidth );
		}


	};

}



#endif