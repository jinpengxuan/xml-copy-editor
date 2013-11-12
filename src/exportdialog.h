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

#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include <wx/wx.h>
#include <wx/tipwin.h>
#include <wx/filepicker.h>

class ExportDialog : public wxDialog
{
	public:
		ExportDialog (
		    wxWindow *parent,
		    const wxString& urlParameter,
            const wxString& folderParameter,
            bool quiet = true,
            bool suppressOptional = true,
            bool html = true,
            bool epub = true,
            bool rtf = true,
            bool doc = true,
            bool fullDaisy = true,
            bool mp3Album = true,
            bool downloadLink = false );
		~ExportDialog();
		void OnOk ( wxCommandEvent& e );
		void OnContextHelp ( wxHelpEvent& e );
		void OnUpdateOk ( wxUpdateUIEvent& e );
		void OnFolderBrowse ( wxCommandEvent& e );
		wxString getUrlString();
		wxString getFolderString();
		bool getQuiet();
		bool getMp3Album();
		bool getSuppressOptional();
		bool getHtml();
        bool getEpub();
        bool getRtf();
        bool getDoc();
        bool getFullDaisy();

		enum constants
		{
			ID_URL,
			ID_FOLDER,
			ID_QUIET,
			ID_SUPPRESS_OPTIONAL,
			ID_FULL_DAISY,
			ID_MP3,
			ID_EPUB,
			ID_RTF,
			ID_DOC,
			ID_HTML
		};

	private:
		wxString url, folder;
		wxCheckBox *quietCheckbox,
		*suppressOptionalCheckbox,
        *htmlCheckbox,
        *epubCheckbox,
        *rtfCheckbox,
	    *docCheckbox,
        *fullDaisyCheckbox,
        *mp3AlbumCheckbox;
		wxFilePickerCtrl *urlCtrl;
        wxDirPickerCtrl *dirPicker;
		bool quiet, suppressOptional, html, epub, rtf, doc, fullDaisy, mp3Album, downloadLink;
		DECLARE_EVENT_TABLE()
};

#endif
