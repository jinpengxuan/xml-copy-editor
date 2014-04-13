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

#include <wx/fdrepdlg.h>
#include "findreplacepanel.h"
#include "nocasecompare.h"
#include "xmlcopyeditor.h"

BEGIN_EVENT_TABLE ( FindReplacePanel, wxPanel )
	EVT_BUTTON ( ID_FINDREPLACE_FIND_NEXT, FindReplacePanel::OnFindNext )
	EVT_BUTTON ( ID_FINDREPLACE_REPLACE, FindReplacePanel::OnReplace )
	EVT_BUTTON ( ID_FINDREPLACE_REPLACE_ALL, FindReplacePanel::OnReplaceAll )
#if !wxCHECK_VERSION(2,9,0)
	EVT_BUTTON ( ID_FINDREPLACE_CLOSE, FindReplacePanel::OnClose )
#else
	EVT_CHAR_HOOK ( FindReplacePanel::OnCharHook )
#endif
	EVT_IDLE ( FindReplacePanel::OnIdle )
END_EVENT_TABLE()

FindReplacePanel::FindReplacePanel (
    wxWindow *parentParameter,
    int id,
    wxFindReplaceData *findDataParameter,
    bool isReplacePanel,
    bool isRegexParameter ) : wxPanel ( parentParameter, id )
{
	parent = parentParameter;
	findData = findDataParameter;
	incrementalFind = notFoundSet = false;
	isRegex = isRegexParameter;

	matchCaseMemory = ( findData->GetFlags() ) & wxFR_MATCHCASE;
	regexMemory = isRegex;

	label1 = new wxStaticText ( this, wxID_ANY, _ ( "Find:" ) );
	spacer1 = new wxStaticText ( this, wxID_ANY, _ ( " " ) );
	spacer2 = new wxStaticText ( this, wxID_ANY, _ ( " " ) );

	int editWidth = 140;
	findEdit = new wxTextCtrl (
	    this,
	    ID_FINDREPLACE_FIND_NEXT,
	    _T ( "" ),
	    wxDefaultPosition,
	    wxSize ( editWidth, -1 )
	);
	findEdit->SetValue ( findData->GetFindString() );

	label2 = new wxStaticText ( this, wxID_ANY, _ ( "Replace with:" ) );
	replaceEdit = new wxTextCtrl (
	    this,
	    ID_FINDREPLACE_REPLACE,
	    _T ( "" ),
	    wxDefaultPosition,
	    wxSize ( editWidth, -1 ) );
	replaceEdit->SetValue ( findData->GetReplaceString() );

	findNextButton = new wxButton (
	    this,
	    ID_FINDREPLACE_FIND_NEXT,
	    _ ( "Find &Next" ),
	    wxDefaultPosition,
	    wxDefaultSize,
	    wxBU_EXACTFIT | wxNO_BORDER );
	replaceButton = new wxButton (
	    this,
	    ID_FINDREPLACE_REPLACE,
	    _ ( "&Replace" ),
	    wxDefaultPosition,
	    wxDefaultSize,
	    wxBU_EXACTFIT | wxNO_BORDER );
	replaceAllButton = new wxButton (
	    this,
	    ID_FINDREPLACE_REPLACE_ALL,
	    _ ( "Replace &All" ),
	    wxDefaultPosition,
	    wxDefaultSize,
	    wxBU_EXACTFIT | wxNO_BORDER );

	matchCaseBox = new wxCheckBox (
	    this,
	    ID_FINDREPLACE_MATCH_CASE,
	    _ ( "&Match case" ) );
	size_t flags = findData->GetFlags();
	matchCaseBox->SetValue ( flags & wxFR_MATCHCASE );

	regexBox = new wxCheckBox (
	    this,
	    ID_FINDREPLACE_REGEX,
	    _ ( "Re&gex" ) );

	int sizerOffset = 2;
	sizer = new wxBoxSizer ( wxHORIZONTAL );
	sizer->Add ( label1, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	sizer->Add ( findEdit, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

	sizer->Add ( label2, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	sizer->Add ( replaceEdit, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

	sizer->Add ( spacer1, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	sizer->Add ( findNextButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

	sizer->Add ( replaceButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	sizer->Add ( replaceAllButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

	sizer->Add ( spacer2, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	sizer->Add ( matchCaseBox, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	sizer->Add ( regexBox, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

#if !wxCHECK_VERSION(2,9,0)
	wxButton *closeButton = new wxButton (
	    this,
	    ID_FINDREPLACE_CLOSE,
	    _ ( "&Close" ),
	    wxDefaultPosition,
	    wxDefaultSize,
	    wxBU_EXACTFIT | wxNO_BORDER );
	sizer->Add ( closeButton, 0, wxLEFT | wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
#endif

	this->SetSizer ( sizer );
	sizer->SetSizeHints ( this );
	sizer->Layout();
	this->SetSize ( -1, findNextButton->GetSize().GetHeight() + 10 );

	findEditLength = findEdit->GetValue().Length();

	if ( !isReplacePanel )
	{
		label2->Hide();
		replaceEdit->Hide();
		replaceButton->Hide();
		replaceAllButton->Hide();
	}

	refresh();
}

FindReplacePanel::~FindReplacePanel()
{}

void FindReplacePanel::OnFindNext ( wxCommandEvent& event )
{
	findData->SetFindString ( findEdit->GetValue() );
	findData->SetReplaceString ( replaceEdit->GetValue() );

	incrementalFind = false;
	size_t flags = 0;
	flags |= wxFR_DOWN;
	if ( matchCaseBox->GetValue() )
		flags |= wxFR_MATCHCASE;
	sendFindEvent ( flags );
}

void FindReplacePanel::OnReplace ( wxCommandEvent& event )
{
	wxFindDialogEvent replaceEvent ( wxEVT_COMMAND_FIND_REPLACE, 0 );
	replaceEvent.SetFlags ( wxFR_DOWN );
	replaceEvent.SetFindString ( findEdit->GetValue() );
	replaceEvent.SetReplaceString ( replaceEdit->GetValue() );
#if wxCHECK_VERSION(2,9,0)
	parent->ProcessWindowEvent( replaceEvent );
#else
	parent->ProcessEvent ( replaceEvent );
#endif
}

void FindReplacePanel::OnReplaceAll ( wxCommandEvent& event )
{
	wxFindDialogEvent replaceAllEvent ( wxEVT_COMMAND_FIND_REPLACE_ALL, 0 );
	replaceAllEvent.SetFlags ( wxFR_DOWN );
	replaceAllEvent.SetFindString ( findEdit->GetValue() );
	replaceAllEvent.SetReplaceString ( replaceEdit->GetValue() );
#if wxCHECK_VERSION(2,9,0)
	parent->ProcessWindowEvent( replaceAllEvent );
#else
	parent->ProcessEvent ( replaceAllEvent );
#endif
}

void FindReplacePanel::focusOnFind()
{
	findEdit->SelectAll();
	findEdit->SetFocus();
}

void FindReplacePanel::OnIdle ( wxIdleEvent& event )
{
	size_t newLength = findEdit->GetValue().Length();

	enableButtons ( ( !newLength ) ? false : true );

	bool settingsChanged = false;
	if ( matchCaseMemory != matchCaseBox->GetValue() ||
	        regexMemory != regexBox->GetValue() )
	{
		settingsChanged = true;
		matchCaseMemory = matchCaseBox->GetValue();
		regexMemory = regexBox->GetValue();
	}

	if ( newLength != findEditLength || settingsChanged )
	{
		incrementalFind = true;

		size_t flags = 0;
		flags |= wxFR_DOWN;
		if ( matchCaseBox->GetValue() )
			flags |= wxFR_MATCHCASE;

		sendFindEvent ( flags );
		findEditLength = newLength;
		findData->SetFlags ( flags );
	}
}

void FindReplacePanel::sendFindEvent ( size_t flags )
{
	wxFindDialogEvent findEvent ( wxEVT_COMMAND_FIND_NEXT, 0 );
	findEvent.SetFlags ( flags );
	findEvent.SetFindString ( findEdit->GetValue() );

	MyFrame *frame = ( MyFrame * ) parent;
	frame->setStrictScrolling ( true );
#if wxCHECK_VERSION(2,9,0)
	frame->ProcessWindowEvent(findEvent);
#else
	frame->ProcessEvent ( findEvent ); // was parent->
#endif
	frame->setStrictScrolling ( false );

	findData->SetFindString ( findEdit->GetValue() );
	findData->SetReplaceString ( replaceEdit->GetValue() );
}

bool FindReplacePanel::getIncrementalFind()
{
	return incrementalFind;
}

void FindReplacePanel::refresh()
{
	incrementalFind = false;
	findEdit->SetValue ( findData->GetFindString() );
	replaceEdit->SetValue ( findData->GetReplaceString() );

	size_t flags = findData->GetFlags();

	bool matchCase;
	matchCase = flags & wxFR_MATCHCASE;

	matchCaseBox->SetValue ( matchCase );
	matchCaseMemory = matchCase;

	regexBox->SetValue ( isRegex );
	regexMemory = isRegex;
}

void FindReplacePanel::setReplaceVisible ( bool b )
{
	label2->Show ( b );
	replaceEdit->Show ( b );
	replaceButton->Show ( b );
	replaceAllButton->Show ( b );
	sizer->Layout();
}

void FindReplacePanel::flagNotFound ( bool b )
{
	if ( ( notFoundSet && b ) || ( !notFoundSet && !b ) )
		return;

	notFoundSet = b;
}

bool FindReplacePanel::getRegex()
{
	return regexBox->GetValue();
}

void FindReplacePanel::setMatchCase ( bool b )
{
	matchCaseBox->SetValue ( b );
}

void FindReplacePanel::setRegex ( bool b )
{
	regexBox->SetValue ( b );
}

void FindReplacePanel::enableButtons ( bool b )
{
	findNextButton->Enable ( b );
	replaceButton->Enable ( b );
	replaceAllButton->Enable ( b );
}

void FindReplacePanel::OnCharHook ( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_ESCAPE && event.GetModifiers() == 0 )
		( ( MyFrame* ) GetParent() )->closeFindReplacePane();
	else
		event.Skip();
}

void FindReplacePanel::OnClose ( wxCommandEvent & e )
{
	( ( MyFrame* ) GetParent() )->closeFindReplacePane();
}
