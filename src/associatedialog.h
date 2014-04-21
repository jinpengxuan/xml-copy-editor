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

#ifndef ASSOCIATE_DIALOG_H
#define ASSOCIATE_DIALOG_H

#include <wx/wx.h>
#include <wx/tipwin.h>

class AssociateDialog : public wxDialog
{
	public:
		AssociateDialog (
		    wxWindow *parent,
		    const wxString& titleParameter = wxEmptyString,
		    const wxString& labelParameter = wxEmptyString,
		    const wxString& typeParameter = _T ( "All" ),
		    const wxString& extensionParameter = _T ( "*.*" ),
		    const wxString& urlParameter = wxEmptyString,
		    wxString *lastDir = NULL,
		    bool auxNeededParameter = false,
		    const wxString& auxLabelTextParameter = wxEmptyString,
		    const wxString& auxParameter = wxEmptyString );
		~AssociateDialog();
		void OnOk ( wxCommandEvent& e );
		void OnBrowse ( wxCommandEvent& e );
		void OnContextHelp ( wxHelpEvent& e );
		void OnUpdateOk ( wxUpdateUIEvent& event );
		wxString getUrl();
		wxString getAux();
		enum constants
		{
			ID_URL,
			ID_BROWSE,
			ID_AUX
		};
	private:
		wxTextCtrl *urlCtrl, *auxCtrl;
		wxButton *browseButton;
		wxStaticText *urlLabel, *auxLabel;
		wxString title, label, type, extension, url;
		wxString *mLastDir;
		bool auxNeeded;
		wxString auxLabelText, aux;
		DECLARE_EVENT_TABLE()
};

#endif
