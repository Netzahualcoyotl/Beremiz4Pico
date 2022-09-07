/////////////////////////////////////////////////////////////////////////////
// Name:        Beremiz4PicoManagerApp.h
// Purpose:     wxWidget main application class for Beremiz4Pico Manager
// Author:      Benoit BOUCHEZ
// Modified by:
// Created:     12/08/2020
// Copyright:   (c) Benoit BOUCHEZ
/////////////////////////////////////////////////////////////////////////////

#ifndef __BEREMIZ4PICOMANAGERAPP_H__
#define __BEREMIZ4PICOMANAGERAPP_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Define a new application type, each program should derive a class from wxApp
class CBeremiz4PicoManagerApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnExit();
};

#endif
