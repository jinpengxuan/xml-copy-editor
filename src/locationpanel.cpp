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

	/*
	  wxStaticText *label = new wxStaticText(
	    this,
	    wxID_ANY,
	    _("Attributes"),
	    wxDefaultPosition,
	    wxDefaultSize);

	  list = new wxListBox(
	    this,
	    wxID_ANY,
	    wxDefaultPosition,
	    wxDefaultSize,
	    0,
	    NULL,
	    wxLB_SORT | wxLB_HSCROLL);
	*/

	sizer->Add ( edit, 0, wxGROW | wxTOP, 0 );
	//sizer->Add(label, 0, wxGROW | wxTOP, 10);
	//sizer->Add(list, 0, wxGROW | wxTOP, 0);
	sizer->Layout();
}

void LocationPanel::update (
    XmlDoc *docParameter,
    const wxString& parentParameter )
{
	doc = docParameter;
	parent = parentParameter;
	wxString previous = edit->GetValue();
	if ( parentParameter == previous )
		return;
	previous = parentParameter;
	edit->SetValue ( parent );
}
