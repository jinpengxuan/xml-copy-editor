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

#include <wx/utils.h>
#include "commandpanel.h"
#include "xmlcopyeditor.h"
#include "wraptempfilename.h"
#include "readfile.h"

BEGIN_EVENT_TABLE ( CommandPanel, wxPanel )
	EVT_BUTTON ( ID_RUN, CommandPanel::OnRun )
	EVT_IDLE ( CommandPanel::OnIdle )
	EVT_BUTTON ( ID_BUTTON_NAME, CommandPanel::OnVariableButton )
	EVT_BUTTON ( ID_BUTTON_PATH, CommandPanel::OnVariableButton )
	EVT_BUTTON ( ID_BUTTON_EXTENSION, CommandPanel::OnVariableButton )
	EVT_BUTTON ( ID_BUTTON_FULLPATH, CommandPanel::OnVariableButton )
	EVT_CHAR_HOOK ( CommandPanel::OnCharHook )
END_EVENT_TABLE()

CommandPanel::CommandPanel (
    wxWindow *parentParameter,
    int id,
    const wxString& cmd,
    bool sync,
    int output,
    const wxString& command ) : wxPanel ( parentParameter, id )
{
	parent = ( MyFrame * ) parentParameter;

	path = _ ( "{path}" );
	name = _ ( "{name}" );
	extension = _ ( "{extension}" );
	fullpath = _ ( "{fullpath}" );
	int sizerOffset = 2;

	wxButton *pathButton = new wxButton ( this, ID_BUTTON_PATH, path, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxNO_BORDER );
	wxButton *nameButton = new wxButton ( this, ID_BUTTON_NAME, name, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxNO_BORDER );
	wxButton *extensionButton = new wxButton ( this, ID_BUTTON_EXTENSION, extension,     wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxNO_BORDER );
	wxButton *fullpathButton = new wxButton ( this, ID_BUTTON_FULLPATH, fullpath,     wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxNO_BORDER );


	commandEdit = new wxTextCtrl ( this, wxID_ANY );
	commandEdit->SetValue ( cmd );

	runButton = new wxButton (
	    this,
	    ID_RUN,
	    _ ( "&Run" ),
	    wxDefaultPosition,
	    wxDefaultSize,
	    wxBU_EXACTFIT | wxNO_BORDER );

	syncBox = new wxCheckBox (
	    this,
	    ID_SYNC,
	    _ ( "&Wait" ) );
	syncBox->SetValue ( sync );

	wxStaticBox *outputBox = new wxStaticBox (
	    this,
	    wxID_ANY,
	    _ ( "Output options" ) );

	outputIgnore = new wxRadioButton (
	    this,
	    ID_COMMAND_OUTPUT_IGNORE,
	    _ ( "I&gnore" ) );
	outputIgnore->SetValue ( true );

	outputInsert = new wxRadioButton (
	    this,
	    ID_COMMAND_OUTPUT_IGNORE,
	    _ ( "I&nsert" ) );

	outputNewDocument = new wxRadioButton (
	    this,
	    ID_COMMAND_OUTPUT_IGNORE,
	    _ ( "New &document" ) );

	switch ( output )
	{
		case ID_COMMAND_OUTPUT_IGNORE:
			outputIgnore->SetValue ( true );
			break;
		case ID_COMMAND_OUTPUT_INSERT:
			outputInsert->SetValue ( true );
			break;
		case ID_COMMAND_OUTPUT_NEW_DOCUMENT:
			outputNewDocument->SetValue ( true );
			break;
		default:
			outputIgnore->SetValue ( true );
			break;
	}

	wxStaticBox *variablesBox = new wxStaticBox (
	    this,
	    wxID_ANY,
	    _ ( "Variables" ) );


	wxStaticBoxSizer *outputSizer = new wxStaticBoxSizer ( outputBox, wxHORIZONTAL );
	outputSizer->Add ( outputIgnore, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	outputSizer->Add ( outputInsert, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	outputSizer->Add ( outputNewDocument, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

	wxStaticBoxSizer *variablesSizer = new wxStaticBoxSizer ( variablesBox, wxHORIZONTAL );
	//variablesSizer->Add(label, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset);
	variablesSizer->Add ( pathButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	variablesSizer->Add ( nameButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	variablesSizer->Add ( extensionButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	variablesSizer->Add ( fullpathButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

	bottomSizer = new wxBoxSizer ( wxHORIZONTAL );
	bottomSizer->Add ( runButton, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	bottomSizer->Add ( syncBox, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	bottomSizer->Add ( variablesSizer, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );
	bottomSizer->Add ( outputSizer, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, sizerOffset );

	mainSizer = new wxBoxSizer ( wxVERTICAL );
	mainSizer->Add ( commandEdit, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND, sizerOffset );
	mainSizer->Add ( bottomSizer );


	this->SetSizer ( mainSizer );
	mainSizer->SetSizeHints ( this );
}

CommandPanel::~CommandPanel()
{}

void CommandPanel::OnRun ( wxCommandEvent& event )
{
	parent->statusProgress ( wxEmptyString );
	wxString command = commandEdit->GetValue();
	if ( command.empty() )
		return;

	// need to test for NULL ptr doc for each use
	XmlDoc *doc = parent->getActiveDocument();

	wxString fullPath = ( doc ) ? doc->getFullFileName() : _T ( "" );

	wxString path, name, extension;
	wxFileName::SplitPath ( fullPath, &path, &name, &extension );

	command.Replace ( _ ( "{fullpath}" ), fullPath, true );
	command.Replace ( _ ( "{path}" ), path, true );
	command.Replace ( _ ( "{name}" ), name, true );
	command.Replace ( _ ( "{extension}" ), extension, true );

	bool redirect = ( getSync() && !outputIgnore->GetValue() );

	if ( !redirect )
	{
		wxExecute ( command, ( getSync() ) ? wxEXEC_SYNC : wxEXEC_ASYNC );
		return;
	}

	wxArrayString stringArray;
	wxExecute ( command, stringArray, wxEXEC_NOHIDE );
	if ( stringArray.empty() )
	{
		parent->statusProgress ( _T ( "No output" ) );
		return;
	}

	size_t count = stringArray.GetCount();
	wxString outputBuffer;
	for ( size_t i = 0 ; i < count; i++ )
	{
		outputBuffer += stringArray[i];
		outputBuffer += L"\n";
	}
	if ( outputInsert->GetValue() )
	{
		if ( doc )
		{
			doc->ReplaceSelection ( outputBuffer );
		}
	}
	else if ( outputNewDocument->GetValue() )
		parent->newDocument ( outputBuffer );

	XmlDoc *newActiveDoc = parent->getActiveDocument();
	if ( newActiveDoc )
		newActiveDoc->SetFocus();
}

void CommandPanel::OnVariableButton ( wxCommandEvent& event )
{
	int id = event.GetId();
	wxString label;
	switch ( id )
	{
		case ID_BUTTON_NAME:
			label = name;
			break;
		case ID_BUTTON_PATH:
			label = path;
			break;
		case ID_BUTTON_EXTENSION:
			label = extension;
			break;
		case ID_BUTTON_FULLPATH:
			label = fullpath;
			break;
		default:
			label = wxEmptyString;
			return;
	}
	long to, from;
	commandEdit->GetSelection ( &from, &to );
	commandEdit->Replace ( from, to, label );
	commandEdit->SetFocus();
}

void CommandPanel::focusOnCommand()
{
	commandEdit->SelectAll();
	commandEdit->SetFocus();
}

bool CommandPanel::getSync()
{
	return syncBox->GetValue();
}

int CommandPanel::getOutput()
{
	if ( outputIgnore->GetValue() )
		return ID_COMMAND_OUTPUT_IGNORE;
	else if ( outputInsert->GetValue() )
		return ID_COMMAND_OUTPUT_INSERT;
	else if ( outputNewDocument->GetValue() )
		return ID_COMMAND_OUTPUT_NEW_DOCUMENT;
	else
		return ID_COMMAND_OUTPUT_IGNORE; // default
}

wxString CommandPanel::getCommand()
{
	return commandEdit->GetValue();
}

void CommandPanel::OnIdle ( wxIdleEvent& event )
{
	bool b = getSync();
	outputIgnore->Enable ( b );
	outputInsert->Enable ( b );
	outputNewDocument->Enable ( b );
}

void CommandPanel::OnCharHook ( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_ESCAPE && event.GetModifiers() == 0 )
		( ( MyFrame* ) GetParent() )->closeCommandPane();
	else
		event.Skip();
}
