//
// Created by James Brandenburg on 6/4/15.
//

#ifndef ROOBARB_ROOPLOTLIB_H
#define ROOBARB_ROOPLOTLIB_H

#include "XmlCanvas.h"
class HistoStyle;

class RooPlotLib {

protected:

    map<string, shared_ptr<XmlCanvas> > canvas;


public:

    RooPlotLib ();


    static HistoStyle style( TH1 * );

};


#endif //ROOBARB_ROOPLOTLIB_H
