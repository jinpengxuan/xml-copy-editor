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

#include "aboutdialog.h"
#include <wx/wx.h>
#include <wx/wxhtml.h>

BEGIN_EVENT_TABLE ( AboutDialog, wxDialog )
	EVT_BUTTON ( wxID_CANCEL, AboutDialog::OnOk )
END_EVENT_TABLE()

AboutDialog::AboutDialog (
    wxWindow *parent,
    const wxString& title,
    const wxString& path,
    const wxString& hyperlink,
    wxPoint positionParameter ) :
		wxDialog ( parent, wxID_ANY, title, positionParameter )
{
	wxBoxSizer *sizer = new wxBoxSizer ( wxVERTICAL );

	html = new wxHtmlWindow ( this, wxID_ANY, wxDefaultPosition, wxSize ( 500, 300 ) );

#ifndef __WXMSW__
	const int sizeArray[] =
	{
		8, 9, 10, 11, 12, 13, 14
	};
	html->SetFonts ( wxEmptyString, wxEmptyString, sizeArray );
#endif

	html->SetBorders ( 0 );
	html->LoadPage ( path );
	// wxID_CANCEL req'd for 'Esc closes dialog' functionality
	button = new wxButton ( this, wxID_CANCEL, _ ( "OK" ) );
	button->SetDefault();

	sizer->Add ( html, 1, wxALL, 10 );
	sizer->Add ( button, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_RIGHT, 10 );

	SetSizer ( sizer );
	SetBackgroundColour ( *wxWHITE );

	sizer->Fit ( this );
	this->SetSizer ( sizer );
	sizer->SetSizeHints ( this );
	html->SetFocus();
}

AboutDialog::~AboutDialog()
{ }

void AboutDialog::OnOk ( wxCommandEvent& e )
{
	position = GetPosition();
	e.Skip();
}

wxPoint AboutDialog::getPosition()
{
	return position;
}
