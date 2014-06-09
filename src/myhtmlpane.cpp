/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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

#include "myhtmlpane.h"
#include "xmlcopyeditor.h"

BEGIN_EVENT_TABLE ( MyHtmlPane, wxHtmlWindow )
END_EVENT_TABLE()

MyHtmlPane::MyHtmlPane (
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& position,
    const wxSize& size ) : wxHtmlWindow ( parent, id, position, size )
{
}

bool MyHtmlPane::OnCellClicked(wxHtmlCell *cell, wxCoord x, wxCoord y, const wxMouseEvent& event)
{
	if ( mLastFile.empty() )
		return false;

	wxHtmlContainerCell *parent = cell->GetParent();
	if (!parent)
		return false;

	// Expect "FatalError at line 6, column 0:"
	wxString msg;
	wxHtmlCell *p = parent->GetFirstChild();
	for (; p != NULL; p = p->GetNext() )
		msg << p->ConvertToText ( NULL );

	const static wxString anchor = _T(" at line ");
	size_t pos = msg.find ( anchor );
	if ( pos == wxString::npos )
		return false;
	pos += anchor.length();

	size_t comma = msg.find ( ',', pos );
	if ( comma == wxString::npos )
		return false;
	msg = msg.Mid ( pos, comma - pos);

	wxChar *psz = NULL;
	int line = wxStrtoul ( msg, &psz, 10 );
	if ( line <= 0 )
		return false;

	MyFrame *frame = ( MyFrame * ) wxTheApp->GetTopWindow();
	XmlDoc *doc = frame->getActiveDocument();
	if ( ( !doc ) || !frame->activateTab ( mLastFile ) )
		return false;

	doc->GotoLine ( line - 1 );
	doc->SetFocus();

	return true;
}

void MyHtmlPane::setLastFile ( const wxString &file )
{
	mLastFile = file;
}
