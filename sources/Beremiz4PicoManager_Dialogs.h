///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DlgLocateTools
///////////////////////////////////////////////////////////////////////////////
class DlgLocateTools : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* EditToolsPath;
		wxButton* BrowseBtn;
		wxStaticLine* m_staticline1;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		wxButton* m_sdbSizer2Cancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowseToolsBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancelBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOKBtn( wxCommandEvent& event ) { event.Skip(); }


	public:

		DlgLocateTools( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Locate external tools"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 410,165 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DlgLocateTools();

};

