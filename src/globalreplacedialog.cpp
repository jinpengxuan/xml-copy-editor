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
#include "globalreplacedialog.h"
#include "wrapregex.h"

BEGIN_EVENT_TABLE ( GlobalReplaceDialog, wxDialog )
	EVT_BUTTON ( wxID_OK, GlobalReplaceDialog::OnOk )
	EVT_HELP_RANGE ( ID_FIND, ID_ALLDOCUMENTS, GlobalReplaceDialog::OnContextHelp )
	EVT_HELP ( wxID_OK, GlobalReplaceDialog::OnContextHelp )
	EVT_HELP ( wxID_CANCEL, GlobalReplaceDialog::OnContextHelp )
	EVT_UPDATE_UI ( wxID_OK, GlobalReplaceDialog::OnUpdateOk )
END_EVENT_TABLE()

GlobalReplaceDialog::GlobalReplaceDialog (
    wxWindow *parent,
    const wxString& findParameter,
    const wxString& replaceParameter,
    bool matchCaseParameter,
    bool allDocumentsParameter,
    bool regexParameter ) :
		wxDialog(),
		find ( findParameter ),
		replace ( replaceParameter ),
		matchCase ( matchCaseParameter ),
		allDocuments ( allDocumentsParameter ),
		regex ( regexParameter )
{
	SetExtraStyle ( wxDIALOG_EX_CONTEXTHELP );
	Create (
	    parent,
	    wxID_ANY,
	    wxString ( _ ( "Global Find and Replace" ) ),
	    wxDefaultPosition,
	    wxSize ( -1, -1 ),
	    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );

	wxBoxSizer *dialogSizer = new wxBoxSizer ( wxVERTICAL );
	wxStaticText *findLabel = new wxStaticText ( this, ID_FIND, _ ( "&Find what: " ) );
	wxStaticText *replaceLabel =
	    new wxStaticText ( this, ID_REPLACE, _ ( "Replace with: " ) );
	findCtrl = new wxTextCtrl (
	    this,
	    ID_FIND,
	    find,
	    wxDefaultPosition,
	    wxSize ( 400, -1 ) );

	replaceCtrl = new wxTextCtrl (
	    this,
	    ID_REPLACE,
	    replace,
	    wxDefaultPosition,
	    wxSize ( 400, -1 ) );

	matchCaseBox =
	    new wxCheckBox ( this, ID_MATCHCASE, _ ( "&Match case" ) );
	matchCaseBox->SetValue ( matchCase );

	regexBox =
	    new wxCheckBox ( this, ID_REGEX, _ ( "&Regex" ) );
	regexBox->SetValue ( regex );

	allDocumentsBox =
	    new wxCheckBox ( this, ID_ALLDOCUMENTS, _ ( "R&eplace in all open documents" ) );
	allDocumentsBox->SetValue ( allDocuments );

	dialogSizer->Add ( findLabel, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
	dialogSizer->Add ( findCtrl, 0, wxALL | wxALIGN_LEFT | wxEXPAND, 5 );
	dialogSizer->Add ( replaceLabel, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
	dialogSizer->Add ( replaceCtrl, 0, wxALL | wxALIGN_LEFT | wxEXPAND, 5 );
	dialogSizer->Add ( matchCaseBox, 0, wxALL | wxALIGN_LEFT, 5 );
	dialogSizer->Add ( regexBox, 0, wxALL | wxALIGN_LEFT, 5 );
	dialogSizer->Add ( allDocumentsBox, 0, wxALL | wxALIGN_LEFT, 5 );
	dialogSizer->Add (
	    CreateButtonSizer ( wxOK | wxCANCEL ), 0, wxTOP | wxBOTTOM | wxALIGN_RIGHT, 5 );
	this->SetSizer ( dialogSizer );
	dialogSizer->SetSizeHints ( this );
	findCtrl->SetFocus();
}

GlobalReplaceDialog::~GlobalReplaceDialog()
{ }

void GlobalReplaceDialog::OnOk ( wxCommandEvent& e )
{
	std::string findUtf8, replaceUtf8;
	find = findCtrl->GetValue();
	replace = replaceCtrl->GetValue();
	findUtf8 = ( const char * ) find.mb_str ( wxConvUTF8 );
	replaceUtf8 = ( const char * ) replace.mb_str ( wxConvUTF8 );
	regex = regexBox->GetValue();
	allDocuments = allDocumentsBox->GetValue();
	matchCase = matchCaseBox->GetValue();

	if ( regex )
	{
		try
		{
			std::auto_ptr<WrapRegex> wr ( new WrapRegex (
			                                  findUtf8,
			                                  matchCase,
			                                  replaceUtf8 ) );
		}
		catch ( std::exception& e )
		{
			std::string pcreError ( e.what() );
			wxString widePcreError =
			    wxString ( pcreError.c_str(), wxConvUTF8, pcreError.size() );
			wxMessageBox (
			    _ ( "Cannot compile regular expression '" ) +
			    findCtrl->GetValue() +
			    _T ( "': " ) +
			    widePcreError,
			    _ ( "Global Find and Replace" ) );
			return;
		}
	}
	e.Skip();
}

void GlobalReplaceDialog::OnContextHelp ( wxHelpEvent& e )
{
	int id = e.GetId();
	if ( id == ID_FIND )
		new wxTipWindow (
		    this,
		    _ ( "Provides a space for you to type the text you want to find" ) );
	else if ( id == ID_REPLACE )
		new wxTipWindow (
		    this,
		    _ ( "Provides a space for you to type the text you want to replace the text you typed in Find what" ) );
	else if ( id == ID_MATCHCASE )
		new wxTipWindow (
		    this,
		    _ ( "Finds only text with lowercase and uppercase letters as specified in Find what" ) );
	else if ( id == ID_ALLDOCUMENTS )
		new wxTipWindow (
		    this,
		    _ ( "Extends the scope to all open documents" ) );
	else if ( id == ID_REGEX )
		new wxTipWindow (
		    this,
		    _ ( "Interprets the text specified in Find what as a regular expression" ) );
	else if ( id == wxID_OK )
		new wxTipWindow (
		    this,
		    _ ( "Finds all instances of the text specified in Find what and replaces them with the text in Replace with" ) );
	else if ( id == wxID_CANCEL )
		new wxTipWindow (
		    this,
		    _ ( "Closes the dialog box without saving any changes you have made" ) );
	else
		{ }
	e.Skip();
}

void GlobalReplaceDialog::OnUpdateOk ( wxUpdateUIEvent& e )
{
	e.Enable ( !findCtrl->GetValue().empty() );
}


wxString GlobalReplaceDialog::getFindString()
{
	return find;
}
wxString GlobalReplaceDialog::getReplaceString()
{
	return replace;
}
bool GlobalReplaceDialog::getRegex()
{
	return regex;
}
bool GlobalReplaceDialog::getAllDocuments()
{
	return allDocuments;
}
bool GlobalReplaceDialog::getMatchCase()
{
	return matchCase;
}
