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

#ifndef GLOBAL_REPLACE_DIALOG_H
#define GLOBAL_REPLACE_DIALOG_H

#include <wx/wx.h>
#include <wx/tipwin.h>

class GlobalReplaceDialog : public wxDialog
{
	public:
		GlobalReplaceDialog (
		    wxWindow *parent,
		    const wxString& findParameter = _T ( "" ),
		    const wxString& replaceParameter = _T ( "" ),
		    bool matchCaseParameter = false,
		    bool allDocumentsParameter = false,
		    bool regexParameter = true );
		~GlobalReplaceDialog();
		void OnOk ( wxCommandEvent& e );
		void OnContextHelp ( wxHelpEvent& e );
		void OnUpdateOk ( wxUpdateUIEvent& event );
		wxString getFindString();
		wxString getReplaceString();
		bool getRegex();
		bool getAllDocuments();
		bool getMatchCase();
		enum constants
		{
			ID_FIND,
			ID_REPLACE,
			ID_REGEX,
			ID_MATCHCASE,
			ID_ALLDOCUMENTS
		};

	private:
		wxTextCtrl *findCtrl, *replaceCtrl;
		wxCheckBox *matchCaseBox, *allDocumentsBox, *regexBox;
		wxString find, replace;
		bool matchCase, allDocuments, regex;
		DECLARE_EVENT_TABLE()
};

#endif
