/////////////////////////////////////////////////////////////////////////////
// Name:        Beremiz4PicoManagerMainFrame.h
// Purpose:     Beremiz for Pico Manager wxWidgets main frame
// Author:      Benoit BOUCHEZ
// Modified by:
// Created:     12/08/2020
// Copyright:   (c) Benoit BOUCHEZ
/////////////////////////////////////////////////////////////////////////////

#ifndef __BEREMIZ4PICOMANAGERMAINFRAME_H__
#define __BEREMIZ4PICOMANAGERMAINFRAME_H__

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

#include <wx/process.h>
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/xml/xml.h>
#include <wx/file.h>
#include <wx/grid.h>
#include <wx/filefn.h>
#ifdef __TARGET_WIN__
#include <wx/msw/registry.h>
#endif
#include "GlobalVars.h"
#include "IOConfigGenerator.h"

class CBeremiz4PicoManagerMainFrame : public wxFrame
{
public:
    CBeremiz4PicoManagerMainFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

	void OnNewProject (wxCommandEvent& event);
	void OnLoadProject (wxCommandEvent& event);
	void OnSaveProject (wxCommandEvent& event);
	void OnSaveProjectAs (wxCommandEvent& event);
	void OnConfigureToolsPath (wxCommandEvent& event);
	void OnGridSelected (wxGridEvent& event);			// User has selected a new cell
	void OnGridChanged (wxGridEvent& event);			// User has changed a cell content

	void OnLaunchEditor (wxCommandEvent& event);
	void OnGenerateArduinoFiles (wxCommandEvent& event);
	void OnCleanTarget (wxCommandEvent& event);

	void OnSelectModule (wxCommandEvent& event);		// Called when a module dropdown combobox is selected

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
	bool ToolsFound;
	bool ProjectLoaded;
	bool ProjectHasChanged;
	wxString ProjectName;

	wxBitmap* PicoImage;
	wxBitmap* IonoRPImage;
	wxBitmap* Nano2040Image;

	wxChoice* CPUSelectCombo;
	wxStaticBitmap* PinoutImage;

	wxGrid* IOConfigGrid;

    wxTimer m_timer;

	TBEREMIZ4PICO_PROJECT Project;

	//! Fills project with defauklt values
	void ClearProject (TBEREMIZ4PICO_PROJECT* Prj, int DefaultCPU);

	//! Save project configuration and displays errors if any
	void SaveProject (void);

	//! Display selected CPU image
	void ShowSelectedCPUPinout (void);

	//! Check if tools (Beremiz and XMOS toolchain) exist in the indicated path
	//! Displays a warning if tools can not be found
	bool CheckToolsExistence (const wxString &Path);

	// Update IOConfigGrid depending on the selected Arduino module
	//! \param ForceNewCPU : avoid warning message in case project is not loaded or created
	void SelectNewCPU (int NewCPUType, bool ForceNewCPU);

	void InitIOConfiguratorForPico (void);
	void InitIOConfigurationForIonoRP (void);

	// Displays I/O configuration
	void RefreshIOConfigGrid (void);

    // Save application configuration in config.xml file
    void SaveConfiguration (void);

    // Load application configuration from the config.xml file
    void LoadConfiguration (void);

	/*
	Corrects the problem of .c files generated by MatIEC for each resource
	These files contain a #include "pous.c" line which is problematic with some compilers
	like Arduino or XMOS xTimeComposer. These compilers use a makefile which compile all
	the .c files, including pous.c, which lead to errors as pous.c is in fact an include
	Manager rename the pous.c into pous2.h then need this method to replace the 
	#include "pous.c" by #include "pous2.h" */
	/* Note : it would be probably better to edit MatIEC source code and recompile
	it but for now it is easier to write this method */
	int POU_Includer (wxString InFileName, wxString OutFileName);
};

#endif
