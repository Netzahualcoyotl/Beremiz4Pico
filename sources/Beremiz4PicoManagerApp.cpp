/////////////////////////////////////////////////////////////////////////////
// Name:        Beremiz4PicoManagerApp.cpp
// Purpose:     wxWidget main application class for Beremiz4Pico Manager
// Author:      Benoit BOUCHEZ
// Modified by:
// Created:     12/08/2020
// Copyright:   (c) Benoit BOUCHEZ
/////////////////////////////////////////////////////////////////////////////

/*
V1.0 : First release
V1.1 - 26/02/2022
	- bug corrected in IOConfigGenerator : C generation typo ("ouptut" in place of "output")
	- version displayed in About box
	- menu shortcuts replaced by Ctrl rather than Alt
	- GPIO set to output as 0 by default on IonoRP (controls the TX line, which goes high and light the LED if set as input)
V2.0 - 02/03/2022
	- removed variable "IOConfigHasChanged" (not used)
	- bug corrected in main frame constructor : ProjectHasChanged was set to true by default
	- ProjectHasChanged do not change if new CPU is selected without a project loaded (avoid false warning about not saved project when closing app)
	- added "Project not saved" warning
	- added support for servo output
	- added support for c_ext (copy CFile_*.c into arduino build library)
	- added a "Clean build directory" command (needed to remove older c_ext files, when their number has been changed)
*/

#include "Beremiz4PicoManagerApp.h"
#include "Beremiz4PicoManagerMainFrame.h"

// 'Main program' equivalent: the program execution "starts" here
bool CBeremiz4PicoManagerApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    CBeremiz4PicoManagerMainFrame *frame = new CBeremiz4PicoManagerMainFrame("Beremiz for Pico Manager");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}  // CBeremiz4PicoManagerApp::OnInit
// -----------------------------------------------------------

int CBeremiz4PicoManagerApp::OnExit()
{
    wxApp::OnExit();
    return 0;
}  // CBeremiz4PicoManagerApp::OnExit
// -----------------------------------------------------------
