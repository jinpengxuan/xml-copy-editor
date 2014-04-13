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

#include <wx/bookctrl.h>
#include <wx/fontenum.h>
#include "mypropertysheet.h"

BEGIN_EVENT_TABLE ( MyPropertySheet, wxPropertySheetDialog )
	EVT_BUTTON ( wxID_OK, MyPropertySheet::OnOk )
	EVT_BUTTON (
	    APPLICATION_DIR_BROWSE,
	    MyPropertySheet::OnApplicationDirBrowse )
END_EVENT_TABLE()

MyPropertySheet::MyPropertySheet (
    wxWindow *parent,
    XmlCtrlProperties& propertiesParameter,
    wxString& applicationDirParameter,
    bool &rememberOpenTabsParameter,
    bool &libxmlNetAccessParameter,
    bool &singleInstanceCheckParameter,
    bool &saveBomParameter,
    bool &unlimitedUndoParameter,
    bool &restoreLayoutParameter,
    bool &expandInternalEntitiesParameter,
    bool &showFullPathOnFrameParameter,
    int &lang,
    const std::set<const wxLanguageInfo *> &translations,
    wxWindowID id,
    wxString title,
    const wxPoint& position,
    const wxSize& size,
    long style
    )
	: wxPropertySheetDialog ( parent, id, title, position, size, style )
	, lang ( lang )
	, properties ( propertiesParameter )
	, applicationDir ( applicationDirParameter )
	, singleInstanceCheck ( singleInstanceCheckParameter )
	, rememberOpenTabs ( rememberOpenTabsParameter )
	, libxmlNetAccess ( libxmlNetAccessParameter )
	, saveBom ( saveBomParameter )
	, unlimitedUndo ( unlimitedUndoParameter )
	, restoreLayout ( restoreLayoutParameter )
	, expandInternalEntities ( expandInternalEntitiesParameter )
	, showFullPathOnFrame ( showFullPathOnFrameParameter )
{
	CreateButtons ( wxOK | wxCANCEL );

	// editor
	wxPanel *editorPanel = new wxPanel ( GetBookCtrl() );
	wxBoxSizer *editorSizer = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer *col1sizer = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer *col2sizer = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer *tablesizer = new wxBoxSizer ( wxHORIZONTAL );

	insertCloseTagBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&Always insert closing tag" ) );
	insertCloseTagBox->SetValue ( properties.insertCloseTag );
	foldBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&Folding" ) );
	foldBox->SetValue ( properties.fold );
	currentLineBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&Highlight current line" ) );
	currentLineBox->SetValue ( properties.currentLine );
	highlightSyntaxBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "Hi&ghlight syntax" ) );
	highlightSyntaxBox->SetValue ( properties.highlightSyntax );
	indentLinesBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&Indentation guides" ) );
	indentLinesBox->SetValue ( properties.indentLines );
	deleteWholeTagBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "I&ntelligent backspace/delete" ) );
	deleteWholeTagBox->SetValue ( properties.deleteWholeTag );
	numberBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&Line numbers" ) );
	numberBox->SetValue ( properties.number );
	protectHiddenElementsBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "L&ock hidden tags" ) );
	protectHiddenElementsBox->SetValue ( properties.protectHiddenElements );
	completionBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&Tag completion" ) );
	completionBox->SetValue ( properties.completion );
	validateAsYouTypeBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&Validate as you type" ) );
	validateAsYouTypeBox->SetValue ( properties.validateAsYouType );
	toggleLineBackgroundBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "Va&riable highlight in tag free view" ) );
	toggleLineBackgroundBox->SetValue ( properties.toggleLineBackground );
	whitespaceVisibleBox =
	    new wxCheckBox ( editorPanel, wxID_ANY, _ ( "&White space visible" ) );
	whitespaceVisibleBox->SetValue ( properties.whitespaceVisible );

	wxStaticText *labelFont = new wxStaticText (
	    editorPanel,
	    wxID_ANY,
	    _ ( "Font" ) );
	fontBox = new wxChoice (
	    editorPanel,
	    wxID_ANY );
	wxArrayString fontArray;
	fontArray = wxFontEnumerator::GetFacenames();
	fontArray.Sort();
	for ( size_t i = 0; i < fontArray.GetCount(); i++ )
	{
		fontBox->Insert ( fontArray.Item ( i ), i );
	}
	fontBox->SetStringSelection ( properties.font );

	col1sizer->Add ( insertCloseTagBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col1sizer->Add ( foldBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col1sizer->Add ( currentLineBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col1sizer->Add ( highlightSyntaxBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col1sizer->Add ( indentLinesBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col1sizer->Add ( deleteWholeTagBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col2sizer->Add ( numberBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col2sizer->Add ( protectHiddenElementsBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col2sizer->Add ( completionBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col2sizer->Add ( validateAsYouTypeBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col2sizer->Add ( toggleLineBackgroundBox, 0, wxALL | wxALIGN_LEFT, 5 );
	col2sizer->Add ( whitespaceVisibleBox, 0, wxALL | wxALIGN_LEFT, 5 );
	tablesizer->Add ( col1sizer, 0, wxALL | wxALIGN_LEFT, 0 );
	tablesizer->Add ( col2sizer, 0, wxALL | wxALIGN_LEFT, 0 );
	editorSizer->Add ( tablesizer, 0, wxALL | wxALIGN_LEFT, 0 );
	editorSizer->Add ( labelFont, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 5 );
	editorSizer->Add ( fontBox, 0, wxALL | wxALIGN_LEFT, 5 );
	editorPanel->SetSizer ( editorSizer );

	// general
	wxPanel *generalPanel = new wxPanel ( GetBookCtrl() );
	wxBoxSizer *vsizer = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer *hsizer = new wxBoxSizer ( wxHORIZONTAL );
	wxBoxSizer *hsizerCheckboxes = new wxBoxSizer ( wxHORIZONTAL );
	wxBoxSizer *vsizerCheckbox1 = new wxBoxSizer ( wxVERTICAL );
	wxBoxSizer *vsizerCheckbox2 = new wxBoxSizer ( wxVERTICAL );

	wxStaticText *label = new wxStaticText (
	    generalPanel,
	    wxID_ANY,
	    _ ( "Application directory" ) );
	applicationDirEdit = new wxTextCtrl (
	    generalPanel,
	    wxID_ANY,
	    applicationDir,
	    wxDefaultPosition,
	    wxSize ( 240, -1 ) );
	wxButton *browse = new wxButton (
	    generalPanel,
	    APPLICATION_DIR_BROWSE,
	    _ ( "Browse" ),
	    wxDefaultPosition,
	    wxSize ( -1, applicationDirEdit->GetSize().GetHeight() ) );

	wxStaticText *labelLanguage = new wxStaticText (
	    generalPanel,
	    wxID_ANY,
	    _ ( "Language (restart required)" ) );
	languageBox = new wxChoice (
	    generalPanel,
	    wxID_ANY );
	languageBox->SetExtraStyle ( languageBox->GetExtraStyle() | wxCB_SORT );

	languageBox->Append ( _ ( "Default" ), ( void* ) wxLANGUAGE_DEFAULT );
	languageBox->SetSelection ( 0 );

	int index;
	std::set<const wxLanguageInfo *>::const_iterator t = translations.begin();
	for ( ; t != translations.end(); ++t )
	{
		index = languageBox->Append ( wxGetTranslation ( ( **t ).Description ),
		                              ( void* )( **t ).Language );
		if (lang == ( **t ).Language)
			languageBox->SetSelection ( index );
	}

	libxmlNetAccessBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "&Enable network access for XML validation" ) );
	libxmlNetAccessBox->SetValue ( libxmlNetAccessParameter );
	expandInternalEntitiesBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "E&xpand internal entities on open" ) );
	expandInternalEntitiesBox->SetValue ( expandInternalEntitiesParameter );
	singleInstanceCheckBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "&One application instance only" ) );
	singleInstanceCheckBox->SetValue ( singleInstanceCheckParameter );
	restoreLayoutBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "Re&member layout on close" ) );
	restoreLayoutBox->SetValue ( restoreLayoutParameter );
	rememberOpenTabsBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "&Remember open tabs on close" ) );
	rememberOpenTabsBox->SetValue ( rememberOpenTabsParameter );
	unlimitedUndoBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "Re&tain undo history on save" ) );
	unlimitedUndoBox->SetValue ( unlimitedUndoParameter );

	saveBomBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "&Save UTF-8 byte order mark" ) );
	saveBomBox->SetValue ( saveBomParameter );

	fullPathBox = new wxCheckBox (
	    generalPanel, wxID_ANY, _ ( "S&how full path on frame" ) );
	fullPathBox->SetValue ( showFullPathOnFrameParameter );

	hsizer->Add ( applicationDirEdit, 0, wxALL | wxALIGN_LEFT, 0 );
	hsizer->Add ( browse, 0, wxLEFT | wxALIGN_LEFT, 5 );
	vsizer->Add ( label, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 5 );
	vsizer->Add ( hsizer, 0, wxALL | wxALIGN_LEFT, 5 );

	vsizer->Add ( labelLanguage, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 5 );
	vsizer->Add ( languageBox, 0, wxALL | wxALIGN_LEFT, 5 );

	vsizerCheckbox1->Add ( libxmlNetAccessBox, 0, wxALL | wxALIGN_LEFT, 5 );
	vsizerCheckbox1->Add ( expandInternalEntitiesBox, 0, wxALL | wxALIGN_LEFT, 5 );
	vsizerCheckbox1->Add ( singleInstanceCheckBox, 0, wxALL | wxALIGN_LEFT, 5 );
	vsizerCheckbox1->Add ( restoreLayoutBox, 0, wxALL | wxALIGN_LEFT, 5 );
	vsizerCheckbox2->Add ( rememberOpenTabsBox, 0, wxALL | wxALIGN_LEFT, 5 );
	vsizerCheckbox2->Add ( unlimitedUndoBox, 0, wxALL | wxALIGN_LEFT, 5 );
	vsizerCheckbox2->Add ( saveBomBox, 0, wxALL | wxALIGN_LEFT, 5 );
	vsizerCheckbox2->Add ( fullPathBox, 0, wxALL | wxALIGN_LEFT, 5 );
	hsizerCheckboxes->Add ( vsizerCheckbox1, 0, wxALL | wxALIGN_LEFT, 0 );
	hsizerCheckboxes->Add ( vsizerCheckbox2, 0, wxALL | wxALIGN_LEFT, 0 );
	vsizer->Add ( hsizerCheckboxes, 0, wxALL | wxALIGN_LEFT, 5 );
	generalPanel->SetSizer ( vsizer );

	editorSizer->Layout();
	vsizer->Layout();
	GetBookCtrl()->AddPage ( generalPanel, _ ( "General" ) );
	GetBookCtrl()->AddPage ( editorPanel, _ ( "Editor" ) );

	LayoutDialog();
}

MyPropertySheet::~MyPropertySheet()
{}

void MyPropertySheet::OnOk ( wxCommandEvent& e )
{
	wxString testDir = applicationDirEdit->GetValue();
	if ( !wxDirExists ( testDir ) )
	{
		wxMessageBox ( _ ( "Cannot access application directory" ), _ ( "Options" ) );
		// tbd: show general tab
		return;
	}
	applicationDir = testDir;

	properties.completion = completionBox->GetValue();
	properties.fold = foldBox->GetValue();
	properties.number = numberBox->GetValue();
	properties.currentLine = currentLineBox->GetValue();
	properties.whitespaceVisible = whitespaceVisibleBox->GetValue();
	properties.indentLines = indentLinesBox->GetValue();
	properties.protectHiddenElements = protectHiddenElementsBox->GetValue();
	properties.toggleLineBackground = toggleLineBackgroundBox->GetValue();
	properties.insertCloseTag = insertCloseTagBox->GetValue();
	properties.deleteWholeTag = deleteWholeTagBox->GetValue();
	properties.validateAsYouType = validateAsYouTypeBox->GetValue();
	properties.font = fontBox->GetStringSelection();
	properties.highlightSyntax = highlightSyntaxBox->GetValue();

	singleInstanceCheck = singleInstanceCheckBox->GetValue();
	restoreLayout = restoreLayoutBox->GetValue();
	rememberOpenTabs = rememberOpenTabsBox->GetValue();
	libxmlNetAccess = libxmlNetAccessBox->GetValue();
	saveBom = saveBomBox->GetValue();
	unlimitedUndo = unlimitedUndoBox->GetValue();
	expandInternalEntities = expandInternalEntitiesBox->GetValue();
	showFullPathOnFrame = fullPathBox->GetValue();

	int languageChoice = languageBox->GetSelection();
	if ( languageChoice != wxNOT_FOUND )
		lang =  ( wxIntPtr ) languageBox->GetClientData ( languageChoice );
	else
		lang = wxLANGUAGE_DEFAULT;

	e.Skip();
}

void MyPropertySheet::OnApplicationDirBrowse ( wxCommandEvent& e )
{
	wxDirDialog browseDir ( this );
	browseDir.SetPath ( applicationDirEdit->GetValue() );
	if ( browseDir.ShowModal() == wxID_OK )
		applicationDirEdit->SetValue ( browseDir.GetPath() );
}
