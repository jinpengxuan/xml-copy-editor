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

#include "locationpanel.h"
#include "xmldoc.h"

BEGIN_EVENT_TABLE ( LocationPanel, wxPanel )
END_EVENT_TABLE()

LocationPanel::LocationPanel ( wxWindow *parentWindowParameter, int id ) :
		wxPanel ( parentWindowParameter, id )
{
	parentWindow = ( MyFrame * ) parentWindowParameter;

	int width = 150;
	SetSize ( wxSize ( width, -1 ) );

	sizer = new wxBoxSizer ( wxVERTICAL );
	SetSizer ( sizer );

	edit = new wxTextCtrl (
	    this,
	    wxID_ANY,
	    wxEmptyString,
	    wxDefaultPosition,
	    wxDefaultSize,
	    wxTE_READONLY );

	wxFont normalFont = wxSystemSettings::GetFont ( wxSYS_DEFAULT_GUI_FONT );
	wxFont boldFont = normalFont;
	boldFont.SetWeight ( wxFONTWEIGHT_BOLD );
	edit->SetFont ( boldFont );

	structureEdit = new wxStyledTextCtrl (
	    this,
	    wxID_ANY,
	    wxDefaultPosition,
	    wxDefaultSize);
	for (int i = 0 ; i < 3; i++ )
		structureEdit->SetMarginWidth ( i, 0 );
	structureEdit->SetReadOnly ( true );
	//structureEdit->SetWrapMode ( wxSTC_WRAP_WORD );
	//structureEdit->SetWrapVisualFlags ( wxSTC_WRAPVISUALFLAG_START );
	structureEdit->SetTabWidth ( 2 );
	structureEdit->SetIndentationGuides ( true );

	sizer->Add ( edit, 0, wxGROW | wxTOP, 0 );
	sizer->Add ( structureEdit, 1, wxGROW | wxTOP, 0 );
	sizer->Layout();
	structureEdit->Show ( false );
}

void LocationPanel::update (
    XmlDoc *doc,
    const wxString& parentParameter )
{
	parent = parentParameter;
	wxString previous = edit->GetValue();

	if ( !doc )
	{
        edit->SetValue ( wxEmptyString );
		structureEdit->Show ( false );
		return;
	}
	else
	{
		wxString structure = doc->getElementStructure ( parent );
		
		if (!structure.empty () )
		{
			indentStructure( structure );
			structureEdit->Show ( true );
			structureEdit->SetReadOnly ( false );
			structureEdit->SetText ( structure );
			structureEdit->SetReadOnly ( true );
			sizer->Layout();
		}
		else
		{
			structureEdit->Show ( false );
		}
	}
	
	if ( parentParameter == previous )
		return;
	previous = parentParameter;
	edit->SetValue ( parent );
	
}

void LocationPanel::indentStructure ( wxString& structure )
{
	wxString indented;
	wxString::const_iterator s = structure.begin();
	int indent = 0;
	const static wxString indentMark ( _T("\t") );

	int count = 0;
	bool justSeenContent = false;
	for ( ; *s; ++s, count++)
	{
		if (*s == '(')
		{
			if ( count && justSeenContent )
			{
				indented += '\n';
			}
			else if (!justSeenContent)
				indented += *s;
			for ( int i = 0; i < indent; i++ )
			{
				indented += indentMark;
			}
			if (justSeenContent)
				indented += *s;

			indent++;

			indented += '\n';
		
			for (int i = 0; indent && i < indent; i++)
				indented += indentMark;
			justSeenContent = false;
		}
		else if (*s == ')')
		{
			if ( justSeenContent )
			{
				indented += '\n';
			}
			indent--;
			for (int i = 0; indent && i < indent; i++)
				indented += indentMark;
			indented += *s;
			indented += '\n';
			if (*( s + 1 ) && *(s + 1) != ')' )
			{
				for (int i = 0; i < indent; i++)
					indented += indentMark;
			}
			justSeenContent = false;
		}
		else
		{
            if ( *s == '|' && justSeenContent)
               indented += ' ';
			indented += *s;
			if ( *s == ',' || *s == '|' )
			   indented += ' ';
			justSeenContent = true;
		}
	}
	structure = indented;
}
