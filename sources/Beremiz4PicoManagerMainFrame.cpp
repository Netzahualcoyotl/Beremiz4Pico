/////////////////////////////////////////////////////////////////////////////
// Name:        Beremiz4PicoManagerMainFrame.cpp
// Purpose:     Beremiz for Pico Manager wxWidgets main frame
// Author:      Benoit BOUCHEZ
// Modified by:
// Created:     12/08/2020
// Copyright:   (c) Benoit BOUCHEZ
/////////////////////////////////////////////////////////////////////////////

#include "Beremiz4PicoManagerMainFrame.h"
#include "Beremiz4PicoManagerApp.h"

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../images/sample.xpm"
#endif

#define TIMER_ID					1000
#define ID_MENU_COMPILE_APPLICATION		1001
#define ID_MENU_NEW_PROJECT	1002
#define ID_MENU_LOAD_PROJECT		1003
#define ID_MENU_LAUNCH_EDITOR		1004
#define ID_MENU_LOAD_APPLICATION	1005
#define ID_MENU_CONFIGURE_TOOLSPATH	1006
#define ID_MENU_GENERATE_TARGET		1007
#define ID_MENU_CLEAN_TARGET		1008
#define ID_COMBO_SELECT_MODULE		1011
#define ID_IO_GRID_CHANGED			1012
#define ID_MENU_SAVE_PROJECT		1013
#define ID_MENU_SAVE_PROJECT_AS		1014

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(CBeremiz4PicoManagerMainFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  CBeremiz4PicoManagerMainFrame::OnQuit)
    EVT_MENU(Minimal_About, CBeremiz4PicoManagerMainFrame::OnAbout)
	EVT_MENU (ID_MENU_NEW_PROJECT, CBeremiz4PicoManagerMainFrame::OnNewProject)
	EVT_MENU (ID_MENU_LOAD_PROJECT, CBeremiz4PicoManagerMainFrame::OnLoadProject)
	EVT_MENU (ID_MENU_SAVE_PROJECT, CBeremiz4PicoManagerMainFrame::OnSaveProject)
	EVT_MENU (ID_MENU_SAVE_PROJECT_AS, CBeremiz4PicoManagerMainFrame::OnSaveProjectAs)
	EVT_MENU (ID_MENU_CONFIGURE_TOOLSPATH, CBeremiz4PicoManagerMainFrame::OnConfigureToolsPath)
	EVT_MENU (ID_MENU_LAUNCH_EDITOR, CBeremiz4PicoManagerMainFrame::OnLaunchEditor)
	EVT_MENU (ID_MENU_GENERATE_TARGET, CBeremiz4PicoManagerMainFrame::OnGenerateArduinoFiles)
	EVT_MENU (ID_MENU_CLEAN_TARGET, CBeremiz4PicoManagerMainFrame::OnCleanTarget)
	EVT_CHOICE (ID_COMBO_SELECT_MODULE, CBeremiz4PicoManagerMainFrame::OnSelectModule)
	EVT_GRID_SELECT_CELL (CBeremiz4PicoManagerMainFrame::OnGridSelected)
	EVT_GRID_CELL_CHANGED (CBeremiz4PicoManagerMainFrame::OnGridChanged)
    EVT_CLOSE(CBeremiz4PicoManagerMainFrame::OnClose)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(CBeremiz4PicoManagerApp);

// frame constructor
CBeremiz4PicoManagerMainFrame::CBeremiz4PicoManagerMainFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title), m_timer (this, TIMER_ID)
{
    wxSize ChoiceSize;      // Size of choice box

	// Init global variables with default values
	this->ClearProject(&this->Project, -1);
	ProjectPath="";
	ProjectLoaded=false;
	ProjectHasChanged=false;
	this->LoadConfiguration();

	// Display a warning if tools path has not yet been configured
	if (ToolsPath=="")
	{
		wxMessageBox ("Path to tools is not configured\n\nIEC61131-3 edition and compilation will not be possible until configured\n\nUse 'File / Set tools path' menu to locate Beremiz for Pico toolchain", "Configuration needed", wxOK+wxICON_INFORMATION);
	}

	SetClientSize (820, 580);

    // set the frame icon
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *fileMenu = new wxMenu;
	fileMenu->Append (ID_MENU_NEW_PROJECT, "&New project\tCtrl-N");
	fileMenu->Append (ID_MENU_LOAD_PROJECT, "&Load project\tCtrl-L");
	fileMenu->Append (ID_MENU_SAVE_PROJECT, "&Save project\tCtrl-S");
	//fileMenu->Append (ID_MENU_SAVE_PROJECT_AS, "&Save project as...");
	fileMenu->AppendSeparator ();
	fileMenu->Append (ID_MENU_CONFIGURE_TOOLSPATH, "&Set tools path", "Set location of external tools");
	fileMenu->AppendSeparator ();
    fileMenu->Append(Minimal_Quit, "E&xit\tCtrl-X", "Quit this program");

	wxMenu* projectMenu = new wxMenu;
	projectMenu->Append (ID_MENU_LAUNCH_EDITOR, "Open Beremiz for Pico &Editor", "Launch Beremiz application");
	projectMenu->Append (ID_MENU_GENERATE_TARGET, "&Generate target files", "Generates project files from PLC code");
	projectMenu->Append (ID_MENU_CLEAN_TARGET, "&Clean target files", "Removes all files from build directory");

	wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
	menuBar->Append(projectMenu, "&Project");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(1);
    SetStatusText("");
#endif // wxUSE_STATUSBAR

    wxInitAllImageHandlers();

#ifdef __TARGET_WIN__
	PicoImage = new wxBitmap (wxT("pico_pinout"), wxBITMAP_TYPE_BMP_RESOURCE);
	IonoRPImage = new wxBitmap (wxT("iono_rp_pinout"), wxBITMAP_TYPE_BMP_RESOURCE);
#endif

#ifdef __TARGET_LINUX__
	PicoImage = new wxBitmap (wxT("./images/pico.bmp"), wxBITMAP_TYPE_BMP);
	IonoRPImage = new wxBitmap (wxT("./images/iono_rp.bmp"), wxBITMAP_TYPE_BMP);
#endif

	wxArrayString HardwareModelStrings;
	HardwareModelStrings.Add ("Raspberry Pi Pico");
	HardwareModelStrings.Add ("Iono RP IRMB10X/R/S");

	CPUSelectCombo = new wxChoice (this, ID_COMBO_SELECT_MODULE, wxPoint (2, 2), wxSize (512, -1), HardwareModelStrings);
	CPUSelectCombo->SetSelection(0);

	ChoiceSize=CPUSelectCombo->GetSize ();      // Needed to place image and IOGrid just below the selector

	PinoutImage = new wxStaticBitmap (this, wxID_ANY, *PicoImage, wxPoint (2, ChoiceSize.GetHeight()), wxSize(512, 512));

	IOConfigGrid = new wxGrid (this, wxID_ANY, wxPoint (516, ChoiceSize.GetHeight()), wxSize (300, 512));
	IOConfigGrid->CreateGrid (0, 3);		// No lines for now, they will be added by call to SelectNewArduinoCPU()
	IOConfigGrid->HideRowLabels();

	IOConfigGrid->SetColLabelValue (0, wxT("Pin"));
	IOConfigGrid->SetColLabelValue (1, wxT("Function"));
	IOConfigGrid->SetColLabelValue (2, wxT("PLC name"));
	IOConfigGrid->SetColSize (0, 100);
	IOConfigGrid->SetColSize (1, 100);
	IOConfigGrid->SetColSize (2, 80);

	SelectNewCPU (CPU_TYPE_PICO, true);
	RefreshIOConfigGrid();

	//IOConfigGrid->AutoSize();
}  // CBeremiz4PicoManagerMainFrame::CBeremiz4AvrManagerMainFrame
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);		// true is to force the frame to close
}  // CBeremiz4PicoManagerMainFrame::OnQuit
// -----------------------------------------------------------

//! Click on close button on window or request to close the window sent by OnQuit
void CBeremiz4PicoManagerMainFrame::OnClose (wxCloseEvent& event)
{
	int DialogResult;

	// Warn user if project has not been saved
	if (ProjectHasChanged)
	{
		DialogResult = wxMessageBox ("Latest changes in the project have not been saved.\nDo you really want to quit?", "Unsaved changes", wxYES_NO+wxICON_WARNING);
		if (DialogResult == wxNO) 
		{
			event.Veto();		// Ignore the close command
			return;
		}
	}

	// Delete static bitmap
	delete PinoutImage;

	// Delete pre-loaded bitmaps
	delete IonoRPImage;
	delete PicoImage;

    Destroy();
}  // CBeremiz4PicoManagerMainFrame::OnClose
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Beremiz for Pico/Arduino Project Manager V2.0\nDevelopment : B.BOUCHEZ\n\n(c) B.BOUCHEZ 2021/2022\n\nBeremiz4Pico uses wxWidgets 3.1 framework",
                 "About Beremiz4Pico Manager",
                 wxOK | wxICON_INFORMATION,
                 this);
}  // CBeremiz4PicoManagerMainFrame::OnAbout
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnSelectModule (wxCommandEvent& WXUNUSED(event))
{
	SelectNewCPU (CPUSelectCombo->GetSelection(), false);
	ProjectHasChanged = true;
}  // CBeremiz4PicoManagerMainFrame::OnSelectModule
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnNewProject (wxCommandEvent& WXUNUSED(event))
{
	bool FileOK;
	wxString EmptyProjectCheck;

	if (ToolsFound == false)
	{
		wxMessageBox ("Path to Beremiz for Pico toolchain is not yet defined\nUse Files/Set Tools Path menu to configure this application first", "Can not create a new project", wxOK+wxICON_ERROR);
		return;
	}

	// Enter name and path for the new project (use the "Create folder" button)
	// TODO : Maybe we should create a more understandable dialog (rather than using the Create Folder button)
	wxDirDialog dlg(NULL, "Create new project directory", "", 0);
	if (dlg.ShowModal() == wxID_CANCEL) return;

	// Check that directory is empty. If not, warn user and return
	/*
	EmptyProjectCheck = wxFindFirstFile ("*.*");
	if (EmptyProjectCheck != "")
	{
		if (wxMessageBox ("Project directory is not empty. All projects files will be erased\nPlease confirm !", "Project already exists", wxYES_NO+wxICON_WARNING) == wxNO) return;
	}
	*/

	// If directory is empty, directory name becomes project name and manager shall point to the directory

	ClearProject(&this->Project, CPU_TYPE_PICO);
	ShowSelectedCPUPinout();
	RefreshIOConfigGrid();
	ProjectLoaded=true;
	ProjectPath=dlg.GetPath();
	ProjectName = ProjectPath.AfterLast(wxFILE_SEP_PATH);
	this->SetTitle ("Beremiz for Pico Manager - "+ProjectName);

	// Copy XML files from basic project into the new folder (use a basic project that can be compiled)
	FileOK = wxCopyFile (ToolsPath+"/BaseFiles/beremiz.xml", ProjectPath+"/beremiz.xml");
	FileOK &= wxCopyFile (ToolsPath+"/BaseFiles/plc.xml", ProjectPath+"/plc.xml");

	if (FileOK == false)
	{
		wxMessageBox ("An error occured while project was created", "Can not create base project files", wxOK+wxICON_WARNING);
	}

	// Save configuration file with default I/O configuration
	SaveProject();
}  // CBeremiz4PicoManagerMainFrame::OnNewProject
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnLoadProject (wxCommandEvent& WXUNUSED(event))
{
	wxXmlDocument doc;
	wxXmlNode* RootNode;
	wxString FilePath;
	wxXmlNode* TopNode;
	wxXmlNode* IOConfigNode;
	wxString CPUType;
	wxString IOPortStr;
	int PortIndex;
	int ProjectCPU=CPU_TYPE_UNDEF;

	// User shall select a directory (directory name = project name)
	wxDirDialog dlg(NULL, "Choose Beremiz4Pico project directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dlg.ShowModal() == wxID_CANCEL)	return;

	// TODO : check if directory contains default files generated when New Project has been clicked

	// clear project structure (default values)
	ClearProject (&this->Project, CPU_TYPE_UNDEF);

	ProjectLoaded=true;

	// If we find default files, make manager point to the directory
	ProjectPath=dlg.GetPath();

	// Load I/O configuration (TODO : make this an independent method)
	FilePath=ProjectPath+"/project_config.xml";

	if (!doc.Load(FilePath))
	{
		wxMessageBox ("Can not load I/O configuration for this project", "Error", wxOK+wxICON_ERROR);
		// TODO : clear the I/O configuration in project in case XML project file is not complete
		return;
	}

	RootNode=doc.GetRoot();
	if (RootNode->GetName() != "project")
	{
		wxMessageBox ("'project_config.xml' file is not a Beremiz4Pico project file", "Error", wxOK+wxICON_EXCLAMATION);
		return;
	}

	// Next level node can be "cpu" or "ioconfig"
	TopNode=RootNode->GetChildren();

	do
	{
		if (TopNode->GetName()=="cpu")
		{
			TopNode->GetAttribute ("type", &CPUType);
			if (CPUType=="pico") ProjectCPU=CPU_TYPE_PICO;
			else if (CPUType=="iono_rp") ProjectCPU=CPU_TYPE_IONO_RP;
			else
			{
				wxMessageBox ("Unknown hardware. Can not load project", "Error in project_config.xml", wxOK+wxICON_EXCLAMATION);
				return;
			}
			// This must be done before reading I/O configuration as SelectNewArduinoCPU clears the I/O assignation table
			this->SelectNewCPU (ProjectCPU, true);		// This method updates Project.BoardType
		}

		else if (TopNode->GetName()=="ioconfig")
		{
			IOConfigNode=TopNode->GetChildren();
			do
			{
				if (IOConfigNode->GetName()=="ioport")
				{
					IOConfigNode->GetAttribute ("index", &IOPortStr);
					PortIndex = atoi (IOPortStr);
					if ((PortIndex>=0) && (PortIndex<=MAX_IO_PINS))
					{
						IOConfigNode->GetAttribute ("mode", &IOPortStr);
						if (IOPortStr=="di") Project.PinUsage[PortIndex]=IO_FUNC_DIGITAL_INPUT;
						else if (IOPortStr=="do") Project.PinUsage[PortIndex]=IO_FUNC_DIGITAL_OUTPUT;
						else if (IOPortStr=="so") Project.PinUsage[PortIndex]=IO_FUNC_SERVO_OUTPUT;
						else if (IOPortStr=="ai") Project.PinUsage[PortIndex]=IO_FUNC_ANALOG_INPUT;
						else if (IOPortStr == "ao") Project.PinUsage[PortIndex]=IO_FUNC_ANALOG_OUTPUT;
						else Project.PinUsage[PortIndex]=IO_FUNC_NONE;
					}
				}  // name = ioport

				IOConfigNode=IOConfigNode->GetNext();
			} while (IOConfigNode != 0);
		}  // ioconfig

		TopNode=TopNode->GetNext();
	} while (TopNode!=0);

	ProjectName = ProjectPath.AfterLast(wxFILE_SEP_PATH);
	this->SetTitle ("Beremiz for Pico Manager - "+ProjectName);
	this->ShowSelectedCPUPinout();
	this->RefreshIOConfigGrid();
	switch (Project.HardwareType)
	{
		case CPU_TYPE_PICO :
			CPUSelectCombo->SetSelection(CPU_TYPE_PICO);
			break;
		case CPU_TYPE_IONO_RP :
			CPUSelectCombo->SetSelection(CPU_TYPE_IONO_RP);
			break;
	}
}  // CBeremiz4AVRManagerMainFrame::OnLoadProject
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnSaveProject (wxCommandEvent& WXUNUSED(event))
{
	SaveProject();
}  // CBeremiz4PicoManagerMainFrame::OnSaveProject
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnSaveProjectAs (wxCommandEvent& WXUNUSED(event))
{
	if (ProjectLoaded == false) return;

	// Enter new name and path for the project (use the "Create folder" button)
	// TODO : Maybe we should create a more understandable dialog (rather than using the Create Folder button)
	wxDirDialog dlg(NULL, "Create new directory to save project", "", 0);
	if (dlg.ShowModal() == wxID_CANCEL) return;

	ProjectPath=dlg.GetPath();
	ProjectName = ProjectPath.AfterLast(wxFILE_SEP_PATH);
	this->SetTitle ("Beremiz for Pico Manager - "+ProjectName);

	// TODO : copy all files from current project to the new directory and save the project file in it (configuration)
	wxMessageBox ("Not yet implemented", "Save project as...", wxOK+wxICON_WARNING);

	SaveProject ();
}  // CBeremiz4PicoManagerMainFrame::OnSaveProjectAs
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnConfigureToolsPath (wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog dlg(this, "Select Beremiz4Pico tools directory", ToolsPath, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dlg.ShowModal() == wxID_CANCEL)	return;

	if (this->CheckToolsExistence(dlg.GetPath()))
	{
        // Update path variable
        ToolsPath=dlg.GetPath();
        this->SaveConfiguration();
    }
}  // CBeremiz4PicoManagerMainFrame::OnConfigureToolsPath
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnLaunchEditor (wxCommandEvent& WXUNUSED(event))
{
	long m_pidLast;
	wxString Cmd;

	// Check first that a project has been loaded
	if (ProjectLoaded==false)
	{
		wxMessageBox ("A project must be created or loaded before Beremiz Editor can be launched", "Can not launch Beremiz4Pico Editor", wxOK+wxICON_INFORMATION);
		return;
	}

	this->SaveProject();

	// Add leading/trailing quotes to the path in case there are spaces
	// NOTE : if we add the path to project as second argument, project is loaded automatically
#ifdef __TARGET_WIN__
	Cmd="\""+ToolsPath+"\\python\\pythonw.exe\" \""+ToolsPath+"\\beremiz\\Beremiz.py\" \""+ProjectPath+"\"";
	//MyProcess* const process = new MyProcess(this, &CmdString[0]);
	// TODO : using a non-NULL callback below allows to know when Beremiz closes
   // m_pidLast = wxExecute(Cmd, wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE, 0, 0);
   m_pidLast = wxExecute(Cmd, wxEXEC_ASYNC, 0, 0);

	if (m_pidLast==0)
	{
		wxMessageBox ("Can not start Beremiz", "Error", wxOK+wxICON_ERROR);
		return;
	}
#endif // __TARGET_WIN__
#ifdef __TARGET_LINUX__
	Cmd="python \""+ToolsPath+"/beremiz_linux/editor/Beremiz.py\" \""+ProjectPath+"\"";
	wxShell (Cmd);
#endif
}  // CBeremiz4PicoManagerMainFrame::OnLaunchEditor
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnGenerateArduinoFiles (wxCommandEvent& WXUNUSED(event))
{
	wxString PLCIOConfigFile;
	wxString BaseFilesDir;
	wxString RuntimeDir;
	wxString BuildDir;
	wxArrayString Output;
	wxArrayString Error;
	long EditPOUSResult;
	wxString POUIncluderCmd;
	wxString ErrorMsg;
	wxDir Directory;
	bool FoundCFile;
	wxString CFilename;

	if (ProjectLoaded==false)
	{
		wxMessageBox ("Target files can only be generated within a project.\nPlease first load or create a project", "Can not generate target files", wxOK+wxICON_EXCLAMATION);
		return;
	}

	if (ToolsFound==false)
	{
		wxMessageBox ("Path to Beremiz4Pico toolchain is not yet defined or invalid", "Can not generate Arduino project files", wxOK+wxICON_EXCLAMATION);
		return;
	}

	RuntimeDir = ProjectPath+"/"+ProjectName+"_arduino/";
	wxMkDir (RuntimeDir, wxS_DIR_DEFAULT);
	BaseFilesDir = ToolsPath+"/BaseFiles/";
	
	BuildDir = ProjectPath+"/build/";

	PLCIOConfigFile=RuntimeDir+"plc_ioconfig.cpp";		// C++ is needed for this file as we will need to use classes (e.g : servos)
	switch (Project.HardwareType)
	{
		case CPU_TYPE_PICO :
			GenerateIOConfigurationForPicoRuntime (&Project, PLCIOConfigFile);		// TODO : report error if any
			break;
		case CPU_TYPE_IONO_RP :
			GenerateIOConfigurationForIonoRuntime (&Project, PLCIOConfigFile);		// TODO : report error if any
			break;
		default : 
			wxMessageBox ("Unknown hardware target!\nCan not generate PLC I/O configuration", "Can not generate plc_ioconfig.cpp", wxOK+wxICON_ERROR);
			return;
	}

	// copy the .c and .h soure files from runtime
	wxCopyFile (BaseFilesDir+"accessor.h", RuntimeDir+"accessor.h");
	wxCopyFile (BaseFilesDir+"beremiz.h", RuntimeDir+"beremiz.h");
	wxCopyFile (BaseFilesDir+"iec_std_FB.h", RuntimeDir+"iec_std_FB.h");
	wxCopyFile (BaseFilesDir+"iec_std_FB.c", RuntimeDir+"iec_std_FB.c");
	wxCopyFile (BaseFilesDir+"iec_std_lib.c", RuntimeDir+"iec_std_lib.c");
	wxCopyFile (BaseFilesDir+"iec_std_lib.h", RuntimeDir+"iec_std_lib.h");
	wxCopyFile (BaseFilesDir+"iec_types.h", RuntimeDir+"iec_types.h");
	wxCopyFile (BaseFilesDir+"iec_types_all.h", RuntimeDir+"iec_types_all.h");
	wxCopyFile (BaseFilesDir+"iec_std_functions.h", RuntimeDir+"iec_std_functions.h");
	wxCopyFile (BaseFilesDir+"pico_runtime.ino", RuntimeDir+ProjectName+"_arduino.ino");

	// copy files generated by MatIEC
	// IMPORTANT : for now, we are limited to ONE resource and it must be called Resource1
	// TODO : we have to copy all .h and .c files from generated files as there are more files to copy in we create our own POUs
	wxCopyFile (BuildDir+"config.c", RuntimeDir+"config.c");
	wxCopyFile (BuildDir+"config.h", RuntimeDir+"config.h");
	wxCopyFile (BuildDir+"POUS.h", RuntimeDir+"POUS.h");
	wxCopyFile (BuildDir+"POUS.c", RuntimeDir+"POUS2.h");		// Rename file

	// copy c_ext source files (for the C extensions)
	// These files are named CFile_0.c, CFile_1.c, etc...
	// TODO : make a loop to find all CFile_*.c files and copy them
	// Note a bug in Beremiz : if the C extension number is changed, it keeps the old c_ext file in build directory
	// TODO : this can be solved be cleaning the build directory before generating C code (but this should be done in Beremiz not here)
	BuildDir = ProjectPath+"/build/";
	Directory.Open (BuildDir);
	FoundCFile = Directory.GetFirst (&CFilename, "CFile_*.c");
	while (FoundCFile)
	{
		wxCopyFile (BuildDir+CFilename, RuntimeDir+CFilename);
		FoundCFile = Directory.GetNext (&CFilename);
	}

	// For now, we restrict the user to have only one resource and it must be called Resource1 in Beremiz
	wxCopyFile (BuildDir+"Resource1.c", RuntimeDir+"Resource1_Temp.c");			// Rename file

	// Edit the .c file containing #include "pous.c"
	// Resource1_Temp.c is edited and renamed into Resource1.c
	EditPOUSResult = POU_Includer (RuntimeDir+"Resource1_Temp.c", RuntimeDir+"Resource1.c");
	if (EditPOUSResult!=0)
	{
		switch (EditPOUSResult)
		{
			case -1 : ErrorMsg = "Can not open "+RuntimeDir+"Resource1_Temp.cpp"; break;
			case -2 : ErrorMsg = "Can not create "+RuntimeDir+"Resource1.cpp"; break;
			default : ErrorMsg = "An error occured during edition of resource file.\nMake sure that your resource is called Resource1";
		}
		wxMessageBox (ErrorMsg, "Error in POU_Includer", wxOK+wxICON_ERROR);
	}

	// Delete Resource1_Temp.c
	wxRemoveFile (RuntimeDir+"Resource1_Temp.c");

	wxMessageBox ("Runtime files have been generated successfully.\nYou can now compile the PLC project using Arduino IDE", "PLC project generation", wxOK+wxICON_INFORMATION);
}  // CBeremiz4PicoManagerMainFrame::OnGenerateArduino
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnCleanTarget (wxCommandEvent& WXUNUSED(event))
{
	int DialogResult;
	bool FoundFile;
	wxString BuildDir;
	wxString RuntimeDir;
	wxString Filename;
	wxDir Directory;

	if (ProjectLoaded==false)
	{
		wxMessageBox ("Target files can only be generated within a project.\nPlease first load or create a project", "No project files to clean", wxOK+wxICON_EXCLAMATION);
		return;
	}

	// Warn user about is going to happen
	DialogResult = wxMessageBox ("This will erase all generated target files. You will need to make a complete file generation (Beremiz and this project manager).\nDo you confirm you want to clean the target files?", "Clean target files", wxYES_NO+wxICON_INFORMATION);
	if (DialogResult == wxNO) return;

	// Remove all files in build directory
	BuildDir = ProjectPath+"/build/";
	Directory.Open (BuildDir);
	FoundFile = Directory.GetFirst (&Filename, "*.*");

	while (FoundFile)
	{
		wxRemoveFile (BuildDir + Filename);
		FoundFile = Directory.GetNext (&Filename);
	}

	Directory.Close ();

	// Remove all files in runtime directory
	RuntimeDir = ProjectPath+"/"+ProjectName+"_arduino/";
	Directory.Open (RuntimeDir);
	FoundFile = Directory.GetFirst (&Filename, "*.*");

	while (FoundFile)
	{
		wxRemoveFile (RuntimeDir + Filename);
		FoundFile = Directory.GetNext (&Filename);
	}

	Directory.Close ();
}  // CBeremiz4PicoManagerMainFrame::OnCleanTarget
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::SaveProject (void)
{
	wxString FilePath;
	FILE* ProjectFile;
	int IOPortIndex;
	char IOConfStr[256];

	if (ProjectLoaded==false) return;			// No path defined yet

	FilePath=ProjectPath+"/project_config.xml";
	ProjectFile=fopen (FilePath, "wt");
	if (ProjectFile==0)
	{
		wxMessageBox("Can not create project file", "Project file error", wxOK+wxICON_ERROR);
		return;
	}

	// Save XML header
	fprintf (ProjectFile, "<?xml version='1.0' encoding='utf-8'?>\n");
	fprintf (ProjectFile, "<project type=\"beremiz4pico\">\n");

	// Save CPU type
	switch (Project.HardwareType)
	{
		case CPU_TYPE_PICO : fprintf (ProjectFile, "  <cpu type=\"pico\"/>\n"); break;
		case CPU_TYPE_IONO_RP : fprintf (ProjectFile, "  <cpu type=\"iono_rp\"/>\n"); break;
	}

	// Save I/O configuration
	fprintf (ProjectFile, "  <ioconfig>\n");

	for (IOPortIndex=0; IOPortIndex<MAX_IO_PINS; IOPortIndex++)
	{
		sprintf (&IOConfStr[0], "    <ioport index=\"%d\" mode=\"", IOPortIndex);
		switch (Project.PinUsage[IOPortIndex])
		{
			case IO_FUNC_NONE : strcat (&IOConfStr[0], "nc\"/>\n"); break;
			case IO_FUNC_DIGITAL_INPUT : strcat (&IOConfStr[0], "di\"/>\n"); break;
			case IO_FUNC_DIGITAL_OUTPUT : strcat (&IOConfStr[0], "do\"/>\n"); break;
			case IO_FUNC_ANALOG_INPUT : strcat (&IOConfStr[0], "ai\"/>\n"); break;
			case IO_FUNC_ANALOG_OUTPUT : strcat (&IOConfStr[0], "ao\"/>\n"); break;
			case IO_FUNC_SERVO_OUTPUT : strcat (&IOConfStr[0], "so\"/>\n"); break;
		}
		fprintf (ProjectFile, "%s", IOConfStr);
	}

	fprintf (ProjectFile, "  </ioconfig>\n");

	// Close main tag
	fprintf (ProjectFile, "</project>\n");

	fclose (ProjectFile);

	ProjectHasChanged=false;
}  // CBeremiz4PicoManagerMainFrame::SaveProject
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::ShowSelectedCPUPinout (void)
{
	switch (Project.HardwareType)
	{
		case CPU_TYPE_PICO : PinoutImage->SetBitmap(*PicoImage); break;
		case CPU_TYPE_IONO_RP : PinoutImage->SetBitmap(*IonoRPImage); break;
	}
}  // CBeremiz4PicoManagerMainFrame::ShowSelectedCPUPinout
// -----------------------------------------------------------

bool CBeremiz4PicoManagerMainFrame::CheckToolsExistence (const wxString &Path)
{
	wxString FilePath;
	wxFile* TestFile;

	TestFile=new wxFile();
	if (TestFile==0) return false;

	// Check that \Beremiz\beremiz\Beremiz.py exists
#ifdef __TARGET_WIN__
	FilePath=Path+"/beremiz/Beremiz.py";
#endif // __TARGET_WIN__
#ifdef __TARGET_LINUX__
    FilePath = Path+"/beremiz_linux/editor/Beremiz.py";
#endif
	if (TestFile->Exists(FilePath)==false)
	{
		wxMessageBox ("Can not find Beremiz.\nPlease check that tools path is defined correctly.", "Can not find IEC61131-3 editor", wxOK+wxICON_ERROR);
		delete TestFile;
		return false;
	}

	// TODO : check that Arduino compiler can be found

	delete TestFile;
	ToolsFound=true;
	return true;
}  // CBeremiz4PicoManagerMainFrame::CheckToolsExistence
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnGridSelected (wxGridEvent& event)
{
	int Col = event.GetCol();
	int Row = event.GetRow();
	wxString IOChoices[10];		// Maximum 10 functions per pin

	if (Col != 1) return;

	if (ProjectLoaded==false)
	{
		wxMessageBox ("I/O configuration can only be edited from within a project.\nPlease first load or create a project", "Can not change I/O function", wxOK+wxICON_EXCLAMATION);
		return;
	}

	IOChoices[0] = NotUsedStr;		// Always valid

	// Choose list depending on CPU type and pin number
	if (Project.HardwareType == CPU_TYPE_PICO)
	{
		// All pins have "Not used / Digital Input / Digital Output" capability at least
		IOChoices[0] = NotUsedStr;
		IOChoices[1] = DigitalInputStr;
		IOChoices[2] = DigitalOutputStr;
		// Pins 0 to 15 are analog outputs, remaining pins are copies of channels 0 to 15
		// We reserve the same pins for servo outputs, this should cover most application to have 16 pins shared between analog outputs and servo outputs
		// TODO : reserved pins
		//	- pin 23 should be digital output only
		//	- pin 24 should be digital input only
		//	- pin 25 should be analog output only
		//  - pin 29 should be analog input only
		// Pins 26 to 28 can be analog inputs, digital inputs or digital outputs

		if ((Row <= 15))
		{  // First 16 GPIO have analog output capabilities (other pins are copies)
			IOChoices[3] = AnalogOutputStr;
			IOChoices[4] = ServoOutputStr;
			IOConfigGrid->SetCellEditor(Row, 1, new wxGridCellChoiceEditor(5, IOChoices, false));		// Display "Analog output" and "Servo output" items added
		}
		else if ((Row >= 26) && (Row <=29))
		{
			IOChoices[3] = AnalogInputStr;
			IOConfigGrid->SetCellEditor(Row, 1, new wxGridCellChoiceEditor(4, IOChoices, false));		// Display "Analog input" item added
		}
		else
		{
			IOConfigGrid->SetCellEditor(Row, 1, new wxGridCellChoiceEditor(3, IOChoices, false));		// Stop list at "Digital output"
		}
	}
	else if (Project.HardwareType == CPU_TYPE_IONO_RP)
	{
		// Four first lines are Digital Inputs or Analog Inputs
		if (Row <= 3)	
		{
			IOChoices[1] = DigitalInputStr;
			IOChoices[2] = AnalogInputStr;
			IOConfigGrid->SetCellEditor (Row, 1, new wxGridCellChoiceEditor(3, IOChoices, false));
		}
		else if (Row <= 5)  // DI5 / DI6 
		{  // TODO : allow to set digital outputs (these lines can be set to direct TTL)
			IOChoices[1] = DigitalInputStr;
			IOConfigGrid->SetCellEditor (Row, 1, new wxGridCellChoiceEditor(2, IOChoices, false));
		}
		else if (Row <= 9)	// DO1..DO4
		{
			IOChoices[1] = DigitalOutputStr;
			IOConfigGrid->SetCellEditor (Row, 1, new wxGridCellChoiceEditor(2, IOChoices, false));
		}
		else if (Row == 10)		// AO1
		{
			IOChoices[1] = AnalogOutputStr;
			IOConfigGrid->SetCellEditor (Row, 1, new wxGridCellChoiceEditor(2, IOChoices, false));
		}
		else  // LED PWM, but can be also set as a digital output
		{
			IOChoices[1] = AnalogOutputStr;
			IOChoices[2] = DigitalOutputStr;
			IOConfigGrid->SetCellEditor (Row, 1, new wxGridCellChoiceEditor(3, IOChoices, false));			
		}
	}

	//IOConfigGrid->EnableCellEditControl(true);
	IOConfigGrid->ShowCellEditControl();
}  // CBeremiz4PicoManagerMainFrame::OnGridSelected
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::OnGridChanged (wxGridEvent& event)
{
	unsigned int Function=IO_FUNC_NONE;
	int Col = event.GetCol();
	int Row = event.GetRow();
	if (Col != 1) return;

	// Identify selected function
	if (IOConfigGrid->GetCellValue(Row, 1)==NotUsedStr) Function=IO_FUNC_NONE;
	else if (IOConfigGrid->GetCellValue(Row, 1)==DigitalInputStr) Function=IO_FUNC_DIGITAL_INPUT;
	else if (IOConfigGrid->GetCellValue(Row, 1)==DigitalOutputStr) Function=IO_FUNC_DIGITAL_OUTPUT;
	else if (IOConfigGrid->GetCellValue(Row, 1)==AnalogInputStr) Function=IO_FUNC_ANALOG_INPUT;
	else if (IOConfigGrid->GetCellValue(Row, 1)==AnalogOutputStr) Function=IO_FUNC_ANALOG_OUTPUT;
	else if (IOConfigGrid->GetCellValue(Row, 1)==ServoOutputStr) Function=IO_FUNC_SERVO_OUTPUT;

	// TODO : check that function is valid for the pin (normally, the choices in list prevents this, but just to be sure...)
	Project.PinUsage[Row]=Function;

	ProjectHasChanged=true;
	RefreshIOConfigGrid();
}  // CBeremiz4PicoManagerMainFrame::OnGridChanged
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::SelectNewCPU (int NewCPUType, bool ForceNewCPU)
{
	int NewLines;
	int NumRows = IOConfigGrid->GetNumberRows();		// This function does not count the grid title row

	if (ForceNewCPU==false)
	{
		if (ProjectLoaded==false)
		{
			wxMessageBox ("Hardware type can only be changed in a project.\nPlease create or load first a project.", "New hardware selected", wxOK+wxICON_EXCLAMATION);
			return;
		}
	}

	if (NewCPUType == Project.HardwareType) return;		// Same CPU type : nothing to do

	// Make sure that all I/O pins become unused (because number of pins may change)
	ClearProject (&this->Project, CPU_TYPE_PICO);

	// Resize the grid depending on the new CPU
	switch (NewCPUType)
	{
		case CPU_TYPE_PICO : NewLines=PICO_TOTAL_PINS; break;
		case CPU_TYPE_IONO_RP : NewLines = IONO_RP_TOTAL_PINS; break;
		default : return;
	}

	// Delete all existing rows from previous configuration
	if (NumRows!=0)
		IOConfigGrid->DeleteRows (0, NumRows, false);
	// and create rows for the new configuration
	IOConfigGrid->AppendRows (NewLines, false);

	// Update grid now that it has the current number of lines
	switch (NewCPUType)
	{
		case CPU_TYPE_PICO :
			this->InitIOConfiguratorForPico();
			break;
		case CPU_TYPE_IONO_RP :
			this->InitIOConfigurationForIonoRP();
			break;
		default : return;
	}

	Project.HardwareType = NewCPUType;
	ShowSelectedCPUPinout();
	RefreshIOConfigGrid();
}  // CBeremiz4PicoManagerMainFrame::SelectNewCPU
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::InitIOConfiguratorForPico (void)
{
	int Pin;
	wxString PinLabel;

	for (Pin=0; Pin<PICO_TOTAL_PINS; Pin++)
	{
		PinLabel="GP"+wxString::Format(wxT("%d"), Pin);
		// Special pins on Pico
		if (Pin == 23) PinLabel = PinLabel + " (PS Mode)";
		else if (Pin == 24) PinLabel = PinLabel + " (VBUS)";
		else if (Pin == 25) PinLabel = PinLabel + " (LED)";
		else if (Pin == 29) PinLabel = PinLabel + " (VSYS)";
		IOConfigGrid->SetCellValue (Pin, 0, PinLabel);
	}

	for (int i=0; i<PICO_TOTAL_PINS; i++)
	{
		IOConfigGrid->SetReadOnly (i, 0, true);
		IOConfigGrid->SetReadOnly (i, 2, true);
	}
}  // CBeremiz4PicoManagerMainFrame::InitIOConfiguratorForPico
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::InitIOConfigurationForIonoRP (void)
{
	IOConfigGrid->SetCellValue (0, 0, "DI1 / AI1");
	IOConfigGrid->SetCellValue (1, 0, "DI2 / AI2");
	IOConfigGrid->SetCellValue (2, 0, "DI3 / AI3");
	IOConfigGrid->SetCellValue (3, 0, "DI4 / AI4");
	IOConfigGrid->SetCellValue (4, 0, "DI5");		// This should be also DIO, as it can be set as TTL I/O
	IOConfigGrid->SetCellValue (5, 0, "DI6");		// Same as DI5
	IOConfigGrid->SetCellValue (6, 0, "DO1");
	IOConfigGrid->SetCellValue (7, 0, "DO2");
	IOConfigGrid->SetCellValue (8, 0, "DO3");
	IOConfigGrid->SetCellValue (9, 0, "DO4");
	IOConfigGrid->SetCellValue (10, 0, "AO1");
	IOConfigGrid->SetCellValue (11, 0, "LED PWM");
}  // CBeremiz4PicoManagerMainFrame::InitIOConfigurationForIonoRP
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::RefreshIOConfigGrid (void)
{
	int NumLines;
	int Lines;
	char PLCNameStr[256];

	switch (Project.HardwareType)
	{
		case CPU_TYPE_PICO :
			NumLines=PICO_TOTAL_PINS;
			break;
		case CPU_TYPE_IONO_RP :
			NumLines=IONO_RP_TOTAL_PINS;
			break;
		default : return;
	}

	// IonoRP has special numbering
	if (Project.HardwareType == CPU_TYPE_IONO_RP)
	{
		for (Lines=0; Lines<NumLines; Lines++)
		{
			if (Project.PinUsage[Lines] == IO_FUNC_NONE)
			{
				IOConfigGrid->SetCellValue (Lines, 1, "Not used");
				IOConfigGrid->SetCellValue (Lines, 2, "---");
			}
			else
			{
				if (Lines<=3)
				{
					switch (Project.PinUsage[Lines])
					{
						case IO_FUNC_DIGITAL_INPUT :
							IOConfigGrid->SetCellValue (Lines, 1, DigitalInputStr);
							sprintf (&PLCNameStr[0], "%%IX0.%d", Lines+1);
							IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);							
							break;
						case IO_FUNC_ANALOG_INPUT :
							IOConfigGrid->SetCellValue (Lines, 1, AnalogInputStr);
							sprintf (&PLCNameStr[0], "%%IW0.%d", Lines+1);
							IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);
							break;
						default :
							IOConfigGrid->SetCellValue (Lines, 1, "????");
							IOConfigGrid->SetCellValue (Lines, 2, "");
					}
				}
				else if (Lines <= 5)
				{
					switch (Project.PinUsage[Lines])
					{
						case IO_FUNC_DIGITAL_INPUT :
							IOConfigGrid->SetCellValue (Lines, 1, DigitalInputStr);
							sprintf (&PLCNameStr[0], "%%IX0.%d", Lines+1);
							IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);							
							break;
						default :
							IOConfigGrid->SetCellValue (Lines, 1, "????");
							IOConfigGrid->SetCellValue (Lines, 2, "");
					}
				}
				else if (Lines <= 9)
				{
					if (Project.PinUsage[Lines] == IO_FUNC_DIGITAL_OUTPUT)
					{
						IOConfigGrid->SetCellValue (Lines, 1, DigitalOutputStr);
						sprintf (&PLCNameStr[0], "%%QX0.%d", Lines-5);
						IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);							
					}
					else
					{
						IOConfigGrid->SetCellValue (Lines, 1, "????");
						IOConfigGrid->SetCellValue (Lines, 2, "");
					}
				}
				else if (Lines == 10)
				{
					if (Project.PinUsage[Lines] == IO_FUNC_ANALOG_OUTPUT)
					{
						IOConfigGrid->SetCellValue (Lines, 1, AnalogOutputStr);
						IOConfigGrid->SetCellValue (Lines, 2, "%QB0.1");
					}
					else
					{
						IOConfigGrid->SetCellValue (Lines, 1, "????");
						IOConfigGrid->SetCellValue (Lines, 2, "");
					}
				}
				else if (Lines == 11)
				{
					if (Project.PinUsage[Lines] == IO_FUNC_ANALOG_OUTPUT)
					{
						IOConfigGrid->SetCellValue (Lines, 1, AnalogOutputStr);
						IOConfigGrid->SetCellValue (Lines, 2, "%QB0.2");
					}
					else if (Project.PinUsage[Lines] == IO_FUNC_DIGITAL_OUTPUT)
					{
						IOConfigGrid->SetCellValue (Lines, 1, DigitalOutputStr);
						IOConfigGrid->SetCellValue (Lines, 2, "%QX0.5");
					}
					else
					{
						IOConfigGrid->SetCellValue (Lines, 1, "????");
						IOConfigGrid->SetCellValue (Lines, 2, "");
					}
				}
			}
		}
		return;
	}

	for (Lines=0; Lines<NumLines; Lines++)
	{
		switch (Project.PinUsage[Lines])
		{
			case IO_FUNC_NONE :
				IOConfigGrid->SetCellValue (Lines, 1, NotUsedStr);
				IOConfigGrid->SetCellValue (Lines, 2, "---");
				break;
			case IO_FUNC_DIGITAL_INPUT :
				IOConfigGrid->SetCellValue (Lines, 1, DigitalInputStr);
				sprintf (&PLCNameStr[0], "%%IX0.%d", Lines);
				IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);
				break;
			case IO_FUNC_DIGITAL_OUTPUT :
				IOConfigGrid->SetCellValue (Lines, 1, DigitalOutputStr);
				sprintf (&PLCNameStr[0], "%%QX0.%d", Lines);
				IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);
				break;
			case IO_FUNC_ANALOG_INPUT :
				IOConfigGrid->SetCellValue (Lines, 1, AnalogInputStr);
				sprintf (&PLCNameStr[0], "%%IW0.%d", Lines);
				IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);
				break;
			case IO_FUNC_ANALOG_OUTPUT :
				IOConfigGrid->SetCellValue (Lines, 1, AnalogOutputStr);
				sprintf (&PLCNameStr[0], "%%QB0.%d", Lines);
				IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);
				break;
			case IO_FUNC_SERVO_OUTPUT :
				IOConfigGrid->SetCellValue (Lines, 1, ServoOutputStr);
				sprintf (&PLCNameStr[0], "%%QB0.%d", Lines);
				IOConfigGrid->SetCellValue (Lines, 2, PLCNameStr);
				break;
			default :
				IOConfigGrid->SetCellValue (Lines, 1, "????");
				IOConfigGrid->SetCellValue (Lines, 2, "");
		}
	}
}  // CBeremiz4PicoManagerMainFrame::RefreshIOConfigGrid
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::ClearProject (TBEREMIZ4PICO_PROJECT* Prj, int DefaultCPU)
{
	memset (Prj, 0, sizeof(TBEREMIZ4PICO_PROJECT));
	Prj->HardwareType=DefaultCPU;
}  // CBeremiz4PicoManagerMainFrame::ClearProject
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::SaveConfiguration (void)
{
    wxString FilePath;
    FILE* ConfigFile;
    wxString Path2Tools;
    //char Path2Tools[512];

	FilePath="config.xml";
	ConfigFile=fopen (FilePath, "wt");
	if (ConfigFile==0)
	{
		wxMessageBox("Can not create project file", "Project file error", wxOK+wxICON_ERROR);
		return;
	}

	fprintf (ConfigFile, "<?xml version='1.0' encoding='utf-8'?>\n");
	fprintf (ConfigFile, "<beremiz4pico_config>\n");

    Path2Tools = "  <toolchain path=\""+ToolsPath+"\"/>\n";
    fprintf (ConfigFile, Path2Tools);

	fprintf (ConfigFile, "</beremiz4pico_config>\n");

	fclose (ConfigFile);
}  // CBeremiz4PicoManagerMainFrame::SaveConfiguration
// -----------------------------------------------------------

void CBeremiz4PicoManagerMainFrame::LoadConfiguration(void)
{
    wxString ConfigPath;
    wxXmlDocument doc;
	wxXmlNode* RootNode;
    wxXmlNode* TopNode;

	this->ToolsFound=false;
	ToolsPath="";

	ConfigPath="config.xml";

	// We have to check first if config file exists, otherwise doc.Load triggers an exception
	// and displays a warning the first time the application is launched
	if (!wxFileExists(ConfigPath)) return;

	if (doc.Load(ConfigPath))
	{
        RootNode=doc.GetRoot();
        if (RootNode->GetName() == "beremiz4pico_config")
        {
            TopNode=RootNode->GetChildren();
            if (TopNode->GetName() == "toolchain")
            {
                ToolsPath = TopNode->GetAttribute("path", "");
                if (CheckToolsExistence(ToolsPath))
                {
                    this->ToolsFound=true;
                }
            }
        }
	}
}  // CBeremiz4PicoManagerMainFrame::LoadConfiguration
// -----------------------------------------------------------

int CBeremiz4PicoManagerMainFrame::POU_Includer (wxString InFileName, wxString OutFileName)
{
	FILE* InFile;
	FILE* OutFile;
	char line[1024];

	InFile = fopen (InFileName, "rt");
	if (InFile == 0) return -1;

	OutFile = fopen (OutFileName, "wt");
	if (OutFile == 0) 
	{
		fclose (InFile);
		return -2;
	}

	// Processing loop
	while (fgets(line, sizeof(line), InFile))
	{
		// Check if we find the #include "pous.c"
		if (strcmp (line, "#include \"POUS.c\"\n")==0)
		{
			// If line is found, replace it
			strcpy (line, "#include \"POUS2.h\"\n");
		}
		
		fprintf (OutFile, "%s", line);
	}

	fclose (InFile);
	fclose (OutFile);

	return 0;
}  // CBeremiz4PicoManagerMainFrame::POU_Includer
// -----------------------------------------------------------

