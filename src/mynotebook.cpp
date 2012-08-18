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

#include "mynotebook.h"
#include "xmlcopyeditor.h"
#include "xmldoc.h"

BEGIN_EVENT_TABLE ( MyNotebook, wxAuiNotebook )
	EVT_LEFT_DOWN ( MyNotebook::OnLeftDown )
	EVT_MIDDLE_DOWN ( MyNotebook::OnMiddleDown )
	EVT_RIGHT_DOWN ( MyNotebook::OnRightDown )
	EVT_MENU ( ID_MENU_CLOSE, MyNotebook::OnMenuClose )
	EVT_MENU ( ID_MENU_CLOSE_ALL, MyNotebook::OnMenuCloseAll )
	//EVT_KEY_DOWN ( MyNotebook::OnKeyDown )
END_EVENT_TABLE()

MyNotebook::MyNotebook (
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& position,
    const wxSize& size,
    int style ) : wxAuiNotebook ( parent, id, position, size, style )
{
	rightClickPage = -1;

	wxTheApp->Connect ( wxEVT_KEY_DOWN,
	    wxKeyEventHandler ( MyNotebook::OnKeyDown ), NULL, this );
}

void MyNotebook::OnLeftDown ( wxMouseEvent& event )
{
	int page = HitTest ( wxPoint ( event.GetX(), event.GetY() ) );
	if ( page == -1 )
	{
		event.Skip();
		return;
	}
	XmlDoc *doc = ( XmlDoc * ) GetPage ( page );
	if ( !doc )
	{
		event.Skip();
		return;
	}
	SetSelection ( page );
	doc->SetFocus();
}

void MyNotebook::OnMiddleDown ( wxMouseEvent& event )
{
	int page = HitTest ( wxPoint ( event.GetX(), event.GetY() ) );
	if ( page == -1 )
	{
		event.Skip();
		return;
	}
	SetSelection ( page );
	MyFrame *frame = ( MyFrame * ) GetParent();
	if ( frame )
		frame->closeActiveDocument();
}

void MyNotebook::OnRightDown ( wxMouseEvent& event )
{
	rightClickPage = HitTest ( wxPoint ( event.GetX(), event.GetY() ) );
	if ( rightClickPage == -1 )
	{
		event.Skip();
		return;
	}
	SetSelection ( rightClickPage );
	wxMenu contextMenu;
	contextMenu.Append ( ID_MENU_CLOSE, _ ( "Close" ) );
	contextMenu.Append ( ID_MENU_CLOSE_ALL, _ ( "Close all" ) );
	contextMenu.Enable ( ID_MENU_CLOSE_ALL, ( this->GetPageCount() > 1 ) );
	PopupMenu ( &contextMenu, wxPoint ( -1, -1 ) );
}

void MyNotebook::OnMenuClose ( wxCommandEvent& WXUNUSED ( event ) )
{
	if ( rightClickPage == -1 )
		return;
	MyFrame *frame = ( MyFrame * ) GetParent();
	if ( frame )
		frame->closeActiveDocument();
}

void MyNotebook::OnMenuCloseAll ( wxCommandEvent& WXUNUSED ( event ) )
{
	MyFrame *frame = ( MyFrame * ) GetParent();
	if ( !frame )
		return;
	wxCommandEvent e;
	frame->OnCloseAll ( e );
}

void MyNotebook::OnKeyDown ( wxKeyEvent &event )
{
#if defined __WXGTK__
	if ( event.m_keyCode != WXK_TAB || !event.m_altDown )
#elif defined __WXOSX__
	if ( event.m_uniChar != ',' || !event.m_controlDown )
#else
	if ( event.m_keyCode != WXK_TAB || !event.m_controlDown )
#endif
	{
		event.Skip();
		return;
	}

	AdvanceSelection ( !event.m_shiftDown );

	int cur = GetSelection();
	XmlDoc *doc = ( XmlDoc * ) GetPage ( cur );
	if ( doc != NULL )
		doc->SetFocus();
}
