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

#ifndef COMMANDPANEL_H
#define COMMANDPANEL_H

#include <wx/wx.h>
#include <iostream>
#include <fstream>

enum
{
	ID_RUN,
	ID_SYNC,
	ID_COMMAND_OUTPUT_IGNORE,
	ID_COMMAND_OUTPUT_INSERT,
	ID_COMMAND_OUTPUT_NEW_DOCUMENT,
	ID_BUTTON_PATH,
	ID_BUTTON_NAME,
	ID_BUTTON_EXTENSION,
	ID_BUTTON_FULLPATH
};

class MyFrame;

class CommandPanel : public wxPanel
{
	public:
		CommandPanel (
		    wxWindow *parent,
		    int id,
		    const wxString& cmd,
		    bool sync = false,
		    int output = ID_COMMAND_OUTPUT_IGNORE,
		    const wxString& command = wxEmptyString );
		~CommandPanel();
		void OnRun ( wxCommandEvent& event );
		void OnVariableButton ( wxCommandEvent& event );
		void OnIdle ( wxIdleEvent& event );
		void OnCharHook ( wxKeyEvent& event );
		void focusOnCommand();
		bool getSync();
		int getOutput();
		wxString getCommand();
	private:
		wxString path, name, extension, fullpath;
		wxTextCtrl *commandEdit;
		wxButton *runButton;
		wxCheckBox *syncBox;
		wxRadioButton *outputIgnore, *outputInsert, *outputNewDocument;
		wxBoxSizer *bottomSizer, *mainSizer;
		MyFrame *parent;
		DECLARE_EVENT_TABLE()
};

#endif
