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

#include <memory>
#include <wx/hyperlink.h>
#include <wx/sizer.h>
#include "exportdialog.h"

BEGIN_EVENT_TABLE ( ExportDialog, wxDialog )
	EVT_BUTTON ( wxID_OK, ExportDialog::OnOk )
	EVT_HELP ( wxID_OK, ExportDialog::OnContextHelp )
	EVT_HELP ( wxID_CANCEL, ExportDialog::OnContextHelp )
	EVT_HELP ( ID_URL, ExportDialog::OnContextHelp )
	EVT_HELP ( ID_FOLDER, ExportDialog::OnContextHelp )
	EVT_HELP ( ID_QUIET, ExportDialog::OnContextHelp )
	EVT_HELP ( ID_MP3, ExportDialog::OnContextHelp )
	EVT_UPDATE_UI ( wxID_OK, ExportDialog::OnUpdateOk )
END_EVENT_TABLE()

ExportDialog::ExportDialog (
    wxWindow *parent,
    const wxString& urlParameter,
    const wxString& folderParameter,
    bool quietParameter,
    bool suppressOptionalParameter,
    bool htmlParameter,
    bool epubParameter,
    bool rtfParameter,
    bool docParameter,
    bool fullDaisyParameter,
    bool mp3AlbumParameter,
    bool downloadLinkParameter) :
		wxDialog(),
		url ( urlParameter ),
		folder ( folderParameter ),
		quiet ( quietParameter ),
		suppressOptional ( suppressOptionalParameter ),
		html ( htmlParameter ),
		epub ( epubParameter ),
		rtf ( rtfParameter ),
		doc ( docParameter ),
		fullDaisy ( fullDaisyParameter ),
		mp3Album ( mp3AlbumParameter ),
		downloadLink ( downloadLinkParameter )
{
	SetExtraStyle ( wxDIALOG_EX_CONTEXTHELP );
	Create (
	    parent,
	    wxID_ANY,
	    wxString ( _ ( "DAISY export" ) ),
	    wxDefaultPosition,
	    wxSize ( 250, -1 ),
	    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );

	wxBoxSizer *dialogSizer = new wxBoxSizer ( wxVERTICAL );

	wxString labelText = _ ( "&Stylesheet for conversion to canonical XHTML (optional):" );
	// pad with whitespace
	labelText += _T("                                  ");
	wxStaticText *urlLabel =
	    new wxStaticText ( this, wxID_ANY, labelText);
	urlCtrl = new wxFilePickerCtrl ( this, ID_URL, url );

#ifndef __WXMSW__
	urlCtrl->SetPath ( url );
#endif

    wxStaticText *folderLabel =
        new wxStaticText ( this, wxID_ANY, _ ( "&Output folder:" ) );
    dirPicker = new wxDirPickerCtrl (
        this,
        ID_FOLDER,
        folder,
        _T("Select a folder"),
        wxDefaultPosition,
        wxSize ( 300, urlCtrl->GetSize().GetHeight() )
         );

    wxStaticBoxSizer *prodnoteSizer = new wxStaticBoxSizer ( wxVERTICAL, this, _T("Production notes") );
    quietCheckbox = new wxCheckBox ( this, ID_QUIET, _ ( "&De-emphasize production notes" ) );
    quietCheckbox->SetValue ( quiet );
    suppressOptionalCheckbox = new wxCheckBox ( this, ID_SUPPRESS_OPTIONAL, _ ( "&Suppress optional production notes" ) );
    suppressOptionalCheckbox->SetValue ( suppressOptional );

    wxStaticBoxSizer *outputSizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Outputs") );
    fullDaisyCheckbox = new wxCheckBox ( this, ID_FULL_DAISY, _ ( "&Full DAISY 2.02 and 3.0 Talking Books" ) );
    fullDaisyCheckbox->SetValue ( fullDaisy );
    htmlCheckbox = new wxCheckBox ( this, ID_HTML, _ ( "&HTML" ) );
    htmlCheckbox->SetValue ( html );
    epubCheckbox = new wxCheckBox ( this, ID_EPUB, _ ( "&ePub ebook" ) );
    epubCheckbox->SetValue ( epub );
    rtfCheckbox = new wxCheckBox ( this, ID_EPUB, _ ( "&RTF document" ) );
    rtfCheckbox->SetValue ( rtf );
    docCheckbox = new wxCheckBox ( this, ID_DOC, _ ( "&Word document" ) );
    docCheckbox->SetValue ( doc );
    mp3AlbumCheckbox = new wxCheckBox ( this, ID_MP3, _ ( "&MP3 album" ) );
    mp3AlbumCheckbox->SetValue ( mp3Album );

    dialogSizer->Add ( urlLabel, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    dialogSizer->Add ( urlCtrl, 0, wxALL | wxALIGN_LEFT | wxEXPAND, 5 );
    dialogSizer->Add ( folderLabel, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    dialogSizer->Add ( dirPicker, 0, wxALL | wxALIGN_LEFT | wxEXPAND, 5 );
    prodnoteSizer->Add ( quietCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    prodnoteSizer->Add ( suppressOptionalCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    outputSizer->Add ( fullDaisyCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    outputSizer->Add ( htmlCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    outputSizer->Add ( epubCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    outputSizer->Add ( rtfCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    outputSizer->Add ( docCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    outputSizer->Add ( mp3AlbumCheckbox, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    dialogSizer->Add ( prodnoteSizer, 0, wxTOP | wxLEFT | wxALIGN_LEFT | wxEXPAND, 5 );
    dialogSizer->Add ( outputSizer, 0, wxTOP | wxLEFT | wxALIGN_LEFT | wxEXPAND, 5 );
    
    if ( downloadLink )
    {
        wxHyperlinkCtrl *downloadCtrl = new wxHyperlinkCtrl (
            this,
            wxID_ANY,
            _ ( "Download DAISY extension" ),
#ifdef __WXMSW__
            _T ( "https://sourceforge.net/projects/xml-copy-editor/files/xmlcopyeditor-daisy/xmlcopyeditor-daisy-1.0.1.exe/download" )
#else
            _T ( "http://xml-copy-editor.sourceforge.net" )
#endif
        );
        dialogSizer->AddSpacer ( 5 );
        dialogSizer->Add ( downloadCtrl, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 5 );
    }
    
   	dialogSizer->AddSpacer ( 5 );
	dialogSizer->Add (
	    CreateButtonSizer ( wxOK | wxCANCEL ), 0, wxTOP | wxBOTTOM | wxALIGN_RIGHT, 5 );
	this->SetSizer ( dialogSizer );
	dialogSizer->SetSizeHints ( this );
	urlCtrl->SetFocus();
}

ExportDialog::~ExportDialog()
{ }

void ExportDialog::OnOk ( wxCommandEvent& e )
{
#ifdef __WXMSW__
	url = urlCtrl->GetTextCtrlValue();
#else
	url = urlCtrl->GetPath();
#endif
	folder = dirPicker->GetPath();
	quiet = quietCheckbox->IsChecked();
	mp3Album = mp3AlbumCheckbox->IsChecked();
    suppressOptional = suppressOptionalCheckbox->IsChecked();
	html = htmlCheckbox->IsChecked();
    epub = epubCheckbox->IsChecked();
    rtf = rtfCheckbox->IsChecked();
    doc = docCheckbox->IsChecked();
    fullDaisy = fullDaisyCheckbox->IsChecked();

	e.Skip();
}

void ExportDialog::OnContextHelp ( wxHelpEvent& e )
{
	int id = e.GetId();
	if ( id == ID_URL )
		new wxTipWindow (
		    this,
		    _ ( "Provides a space for you to enter or select a stylesheet for conversion to canonical XHTML" ) );
	if ( id == ID_FOLDER )
		new wxTipWindow (
		    this,
		    _ ( "Provides a space for you to enter or select the output folder" ) );
	else if ( id == wxID_OK )
		new wxTipWindow (
		    this,
		    _ ( "Starts the export" ) );
	else if ( id == wxID_CANCEL )
		new wxTipWindow (
		    this,
		    _ ( "Closes the dialog box without exporting the file" ) );
	else
		{ }
	e.Skip();
}

void ExportDialog::OnUpdateOk ( wxUpdateUIEvent& e )
{
    bool enable = true;
    if (
        dirPicker->GetPath().empty() ||
        downloadLink )
        enable = false;
    e.Enable ( enable );
}

wxString ExportDialog::getUrlString()
{
	return url;
}

wxString ExportDialog::getFolderString()
{
    return folder;
}

bool ExportDialog::getQuiet()
{
    return quiet;
}

bool ExportDialog::getMp3Album()
{
    return mp3Album;   
}

bool ExportDialog::getSuppressOptional() { return suppressOptional; }
bool ExportDialog::getHtml() { return html; }
bool ExportDialog::getEpub() { return epub; }
bool ExportDialog::getRtf() { return rtf; }
bool ExportDialog::getDoc() { return doc; }
bool ExportDialog::getFullDaisy() { return fullDaisy; }

void ExportDialog::OnFolderBrowse ( wxCommandEvent& e )
{
}
