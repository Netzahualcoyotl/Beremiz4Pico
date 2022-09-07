///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Beremiz4PicoManager_Dialogs.h"

///////////////////////////////////////////////////////////////////////////

DlgLocateTools::DlgLocateTools( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("External tool directory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALL, 5 );

	EditToolsPath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxPoint( 0,20 ), wxDefaultSize, 0 );
	#ifdef __WXGTK__
	if ( !EditToolsPath->HasFlag( wxTE_MULTILINE ) )
	{
	EditToolsPath->SetMaxLength( 512 );
	}
	#else
	EditToolsPath->SetMaxLength( 512 );
	#endif
	EditToolsPath->SetMinSize( wxSize( 400,-1 ) );

	bSizer4->Add( EditToolsPath, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );

	BrowseBtn = new wxButton( this, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( BrowseBtn, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );


	bSizer3->Add( bSizer4, 1, 0, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();

	bSizer6->Add( m_sdbSizer2, 1, wxEXPAND, 5 );


	bSizer3->Add( bSizer6, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer3 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	BrowseBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgLocateTools::OnBrowseToolsBtn ), NULL, this );
	m_sdbSizer2Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgLocateTools::OnCancelBtn ), NULL, this );
	m_sdbSizer2OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgLocateTools::OnOKBtn ), NULL, this );
}

DlgLocateTools::~DlgLocateTools()
{
	// Disconnect Events
	BrowseBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgLocateTools::OnBrowseToolsBtn ), NULL, this );
	m_sdbSizer2Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgLocateTools::OnCancelBtn ), NULL, this );
	m_sdbSizer2OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgLocateTools::OnOKBtn ), NULL, this );

}
