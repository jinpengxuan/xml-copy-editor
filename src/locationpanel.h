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

#ifndef LOCATION_PANEL_H
#define LOCATION_PANEL_H

#include <wx/wx.h>
#include <wx/grid.h>
#include "xmlcopyeditor.h"

class XmlDoc;

class LocationPanel : public wxPanel
{
	public:
		LocationPanel ( wxWindow *parent, int id );
		void update (
		    XmlDoc *docParameter = NULL,
		    const wxString& parent = wxEmptyString );
	private:
		void indentStructure ( wxString& structure );
		MyFrame *parentWindow;
		wxBoxSizer *sizer;
		wxTextCtrl *edit;
		wxStyledTextCtrl *structureEdit;
		wxString parent;

		DECLARE_EVENT_TABLE()
};

#endif
