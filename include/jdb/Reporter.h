#ifndef REPORTER_H
#define REPORTER_H

#include "TCanvas.h"
#include "Logger.h"
#include "XmlConfig.h"

namespace jdb {
	/*jdoc{
		"class" : "Reporter",
		"desc" : "A small utility class for creating PDF reports from ROOT canvas objects"
	}*/
	class Reporter
	{
	public:
		/*jdoc{
			"name" : "Reporter( string filename, int w = 791, int h = 1024 )",
			"params" : [
				"filename", "w", "h"
			],
			"paramDesc" : [
				"Output filename", "width of pdf page", "height of page"
			],
			"returns" : [
				
			],
			"desc" : ""
		}*/
		Reporter( string filename, int w = 791, int h = 1024 );
		/*jdoc{
			"name" : "Reporter( XmlConfig * config, string nodePath, string prefix = \"\" )",
			"params" : [
				"config", "nodePath", "prefix"
			],
			"paramDesc" : [
				"Project config", "Path to config node containing params", "prefix for parallel jobs - prepended"
			],
			"returns" : [
				
			],
			"desc" : ""
		}*/
		Reporter( XmlConfig * config, string np, string prefix = "" );
		~Reporter();

		/*jdoc{
			"name" : "void newPage( int dx = 1, int dy = 1)",
			"params" : [
				"dx", "dy"
			],		
			"paramDesc" : [
				"Optional: Number of divisions in x direction",
				"Optional: Number of divisions in y direction"
			],
			"returns" : [
				
			],
			"desc" : "Creates a new page with sub-divided pads"
		}*/
		void newPage( int dx = 1, int dy = 1);
		/*jdoc{
			"name" : "void cd( int pad )",
			"params" : [
				"pad"
			],
			"paramDesc" : [
				"The pad to set as the current"
			],
			"returns" : [
				
			],
			"desc" : "The 0 pad is the entire page, individual pads start a 1"
		}*/
		void cd( int pad );
		/*jdoc{
			"name" : "void cd( int padX, int padY)",
			"params" : [
				"padX", "padY"
			],
			"paramDesc" : [
				"X coord of pad",
				"Y coord of pad"
			],
			"returns" : [
				
			],
			"desc" : "Sets the padX, padY as the current pad. (1, 1) is the top left pad"
		}*/
		void cd( int padX, int padY);
		/*jdoc{
			"name" : "void next()",
			"desc" : "Pushes to the next pad or next page"
		}*/
		void next();
		/*jdoc{
			"name" : "void savePage( string name = \"\" )",
			"params" : [
				"name"
			],
			"paramDesc" : [
				"Optional: output filename"
			],
			"desc" : "To add to the main report leave name off. Otherwise a single page PDF will be made with the given name"
		}*/
		void savePage( string name = "" );
		/*jdoc{
			"name" : "void saveImage( string name )",
			"params" : [
				"name"
			],
			"paramDesc" : [
				"Output filename"
			],
			"desc" : "Saves an image to the given filename. Must have suffix to determine format, ie .png, .jpg etc."
		}*/
		void saveImage( string name );

	protected:

		XmlConfig * cfg;
		string nodePath;
		Logger * logger;
		TCanvas* canvas;
		int currentPad;
		int dx, dy;

		string filename;
		static int instances;

	};
	
}



#endif