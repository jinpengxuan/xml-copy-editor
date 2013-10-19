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

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <wx/wx.h>
#include <wx/wxhtml.h>

class AboutDialog : public wxDialog
{
	public:
		AboutDialog (
		    wxWindow *parent,
		    const wxString& title,
		    const wxString& path,
		    const wxString& hyperlink,
		    wxPoint positionParameter );
		~AboutDialog();
		void OnOk ( wxCommandEvent& e );
		wxPoint getPosition();
	private:
		wxPoint position;
		wxHtmlWindow *html;
		wxButton *button;
		DECLARE_EVENT_TABLE()
};

#endif
