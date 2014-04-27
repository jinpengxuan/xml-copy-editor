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

#ifndef INSERT_PANEL_H
#define INSERT_PANEL_H

#include <wx/wx.h>
#include "xmldoc.h"
#include "xmlcopyeditor.h"

enum
{
	INSERT_PANEL_TYPE_CHILD = 0,
	INSERT_PANEL_TYPE_SIBLING,
	INSERT_PANEL_TYPE_ENTITY
};

class InsertPanel : public wxPanel
{
	public:
		InsertPanel (
		    wxWindow *parent,
		    int id,
		    int type = INSERT_PANEL_TYPE_CHILD );
		void update (
		    XmlDoc *doc,
		    const wxString& parent = wxEmptyString,
		    const wxString& grandparent = wxEmptyString,
		    bool forced = false );
		void OnEnter ( wxCommandEvent& event );
		void OnDoubleClick ( wxCommandEvent& event );
		void OnListSelection ( wxCommandEvent& event );
		void setEditFocus();
		void OnSize ( wxSizeEvent& e );
		void adjustSize();
	private:
		MyFrame *parentWindow;
		int type;
		wxBoxSizer *sizer;
		wxTextCtrl *edit;
		wxListBox *list;
		wxString parent, grandparent, lastParent;
		XmlDoc *doc, *lastDoc;

		void handleChoice ( const wxString& choice );
		DECLARE_EVENT_TABLE()
};

#endif
