/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * Xml Copy Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xml Copy Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <memory>
#include "associatedialog.h"

BEGIN_EVENT_TABLE ( AssociateDialog, wxDialog )
	EVT_BUTTON ( wxID_OK, AssociateDialog::OnOk )
	EVT_BUTTON ( ID_BROWSE, AssociateDialog::OnBrowse )
	EVT_HELP_RANGE ( ID_URL, ID_AUX, AssociateDialog::OnContextHelp )
	EVT_HELP ( wxID_OK, AssociateDialog::OnContextHelp )
	EVT_HELP ( wxID_CANCEL, AssociateDialog::OnContextHelp )
	EVT_UPDATE_UI ( wxID_OK, AssociateDialog::OnUpdateOk )
END_EVENT_TABLE()

AssociateDialog::AssociateDialog (
    wxWindow *parent,
    const wxString& titleParameter,
    const wxString& labelParameter,
    const wxString& typeParameter,
    const wxString& extensionParameter,
    const wxString& urlParameter,
    wxString *lastDir,
    bool auxNeededParameter,
    const wxString& auxLabelTextParameter,
    const wxString& auxParameter ) :
		wxDialog(),
		title ( titleParameter ),
		label ( labelParameter ),
		type ( typeParameter ),
		extension ( extensionParameter ),
		url ( urlParameter ),
		mLastDir ( lastDir ),
		auxNeeded ( auxNeededParameter ),
		auxLabelText ( auxLabelTextParameter ),
		aux ( auxParameter )
{
	SetExtraStyle ( wxDIALOG_EX_CONTEXTHELP );
	Create (
	    parent,
	    wxID_ANY,
	    title,
	    wxDefaultPosition,
	    wxSize ( -1, -1 ),
	    wxDEFAULT_DIALOG_STYLE );

	wxBoxSizer *dialogSizer = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer *horizontalSizer = new wxBoxSizer ( wxHORIZONTAL );
	auxCtrl = NULL; // may or may not be used
	urlLabel = new wxStaticText ( this, ID_URL, label );
	urlCtrl = new wxTextCtrl (
	    this,
	    ID_URL,
	    url,
	    wxDefaultPosition,
	    wxSize ( 240, -1 ) );
	browseButton = new wxButton (
	    this,
	    ID_BROWSE,
	    _ ( "Browse" ) );
	horizontalSizer->Add ( urlCtrl, 0, wxLEFT, 0 );
	horizontalSizer->Add ( browseButton, 0, wxLEFT, 5 );

	if ( auxNeeded )
	{
		auxLabel = new wxStaticText ( this, ID_AUX, auxLabelText );
		auxCtrl = new wxTextCtrl (
		    this,
		    ID_AUX,
		    aux,
		    wxDefaultPosition,
		    wxSize ( 240, -1 ) );
		dialogSizer->Add ( auxLabel, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 10 );
		dialogSizer->Add ( auxCtrl, 0, wxALL, 5 );
	}

	dialogSizer->Add ( urlLabel, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 10 );
	dialogSizer->Add ( horizontalSizer, 0, wxALL, 5 );
	dialogSizer->Add (
	    CreateButtonSizer ( wxOK | wxCANCEL ), 0, wxTOP | wxBOTTOM | wxALIGN_RIGHT, 10 );
	this->SetSizer ( dialogSizer );
	dialogSizer->SetSizeHints ( this );

	if ( auxNeeded )
		auxCtrl->SetFocus();
	else
		urlCtrl->SetFocus();
}

AssociateDialog::~AssociateDialog()
{ }

void AssociateDialog::OnOk ( wxCommandEvent& e )
{
	url = urlCtrl->GetValue();
	if ( auxCtrl )
		aux = auxCtrl->GetValue();
	e.Skip();
}

void AssociateDialog::OnContextHelp ( wxHelpEvent& e )
{
	int id = e.GetId();
	if ( id == ID_URL )
		new wxTipWindow (
		    this,
		    _ ( "Provides a space for you to type the path of the file" ) );
	else if ( id == ID_BROWSE )
		new wxTipWindow (
		    this,
		    _ ( "Opens a standard file dialog" ) );
	else if ( id == ID_AUX )
		new wxTipWindow (
		    this,
		    _ ( "Provides a space for you to type additional information" ) );
	else if ( id == wxID_CANCEL )
		new wxTipWindow (
		    this,
		    _ ( "Closes this dialog without making any changes" ) );
	else if ( id == wxID_OK )
		new wxTipWindow (
		    this,
		    _ ( "Selects the file specified" ) );
	else
		{ }
	e.Skip();
}

void AssociateDialog::OnUpdateOk ( wxUpdateUIEvent& e )
{
	e.Enable ( !urlCtrl->GetValue().empty() );
}

wxString AssociateDialog::getUrl()
{
	return url;
}
wxString AssociateDialog::getAux()
{
	return aux;
}

void AssociateDialog::OnBrowse ( wxCommandEvent& e )
{
	wxString extensionArgument;
	extensionArgument =
	    type +
	    _T ( " (" ) +
	    extension +
	    _T ( ")|" ) +
	    extension +
	    _ ( "|All files (*.*)|*.*" );
	std::auto_ptr<wxFileDialog> fd ( new wxFileDialog (
	                                     this,
	                                     _ ( "Select " ) + type,
	                                     mLastDir ? *mLastDir : _T ( "" ),
	                                     _T ( "" ),
	                                     extensionArgument,
#if wxCHECK_VERSION(2,9,0)
	                                     wxFD_OPEN | wxFD_FILE_MUST_EXIST
#else
	                                     wxOPEN | wxFILE_MUST_EXIST
#endif
	                                     ) );

	if ( fd->ShowModal() == wxID_OK )
	{
		wxString newValue = fd->GetPath();
		urlCtrl->SetValue ( newValue );

		if ( mLastDir )
			*mLastDir = fd->GetDirectory();
	}
}
