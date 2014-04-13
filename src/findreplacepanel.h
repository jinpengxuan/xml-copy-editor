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

#ifndef FINDREPLACEPANEL_H
#define FINDREPLACEPANEL_H

#include <wx/wx.h>
#include <iostream>
#include <fstream>

class wxFindReplaceData;

enum
{
	ID_FINDREPLACE_FIND_NEXT,
	ID_FINDREPLACE_REPLACE,
	ID_FINDREPLACE_REPLACE_ALL,
	ID_FINDREPLACE_MATCH_CASE,
	ID_FINDREPLACE_REGEX,
	ID_FINDREPLACE_CLOSE
};

class FindReplacePanel : public wxPanel
{
	public:
		FindReplacePanel (
		    wxWindow *parent,
		    int id,
		    wxFindReplaceData *findDataParameter,
		    bool isReplacePanel = true,
		    bool isRegex = true );
		~FindReplacePanel();
		void OnFindNext ( wxCommandEvent& event );
		void OnReplace ( wxCommandEvent& event );
		void OnReplaceAll ( wxCommandEvent& event );
		void OnClose ( wxCommandEvent& event );
		void focusOnFind();
		bool getIncrementalFind();
		bool getRegex();
		void refresh();
		void setReplaceVisible ( bool b );
		void setMatchCase ( bool b );
		void setRegex ( bool b );
		void flagNotFound ( bool b );
		void enableButtons ( bool b );
	private:
		wxTextCtrl *findEdit, *replaceEdit;
		wxStaticText *label1, *label2, *spacer1, *spacer2;
		wxButton *findNextButton, *replaceButton, *replaceAllButton;
		wxCheckBox *matchCaseBox, *regexBox;
		wxFindReplaceData *findData;
		wxBoxSizer *sizer;
		wxWindow *parent;
		size_t findEditLength;
		bool matchCaseMemory, regexMemory;
		bool incrementalFind, isReplaceDialog, notFoundSet, isRegex;

		void OnCharHook ( wxKeyEvent& event );
		void OnIdle ( wxIdleEvent& event );
		void sendFindEvent ( size_t flags );

		DECLARE_EVENT_TABLE()
};

#endif
