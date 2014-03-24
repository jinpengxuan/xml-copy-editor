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

#include <wx/intl.h>
#ifdef USE_ENCHANT
#	include <enchant.h>
#else
#	include "aspell.h"
#endif
#include "styledialog.h"
#include "nocasecompare.h"

BEGIN_EVENT_TABLE ( StyleDialog, wxDialog )
	EVT_BUTTON ( ID_STYLE_REPORT, StyleDialog::OnReport )
	EVT_BUTTON ( ID_STYLE_IGNORE_ALL, StyleDialog::OnStyleIgnoreAll )
	EVT_BUTTON ( ID_STYLE_CHANGE_ALL, StyleDialog::OnStyleChangeAll )
	EVT_BUTTON ( ID_STYLE_EDIT, StyleDialog::OnStyleEdit )
	EVT_BUTTON ( ID_STYLE_WEB_REPORT, StyleDialog::OnStyleWebReport )
	EVT_BUTTON ( ID_STYLE_WEB_SUMMARY, StyleDialog::OnStyleWebSummary )
	EVT_BUTTON ( wxID_CANCEL, StyleDialog::OnCancel )
	EVT_MENU ( ID_MENU_CHANGE_ONCE, StyleDialog::OnMenuChangeOnce )
	EVT_MENU ( ID_MENU_CHANGE_ALL, StyleDialog::OnMenuChangeAll )
	EVT_MENU ( ID_MENU_IGNORE_ONCE, StyleDialog::OnMenuIgnoreOnce )
	EVT_MENU ( ID_MENU_IGNORE_ALL, StyleDialog::OnMenuIgnoreAll )
	EVT_MENU ( ID_MENU_NEW_SUGGESTION, StyleDialog::OnMenuNewSuggestion )
	EVT_MENU ( ID_MENU_APPLY_SUGGESTION_ALL, StyleDialog::OnMenuApplySuggestionAll )
	EVT_LIST_COL_CLICK ( ID_STYLE_TABLE, StyleDialog::OnColumnClick )
	EVT_LIST_ITEM_ACTIVATED ( ID_STYLE_TABLE, StyleDialog::OnItemActivated )
	EVT_LIST_ITEM_RIGHT_CLICK ( ID_STYLE_TABLE, StyleDialog::OnItemRightClick )
	EVT_UPDATE_UI_RANGE ( ID_STYLE_EDIT, ID_STYLE_CHANGE_ALL, StyleDialog::OnUpdateTableRange )
END_EVENT_TABLE()

#ifdef USE_ENCHANT
class dictdetect
{
public:
	dictdetect(wxComboBox *aCombo) : ruleSetCombo(aCombo), anyFound(false) {}
	void add(const char *lang_tag);
	bool empty() const { return !anyFound; }
private:
	wxComboBox *ruleSetCombo;
	bool anyFound;
};

void dictdetect::add(const char *lang_tag)
{
	anyFound = true;
	std::string stdEntry = lang_tag;
	wxString entry = wxString ( stdEntry.c_str(), wxConvUTF8, stdEntry.size() );
	ruleSetCombo->Append ( entry );
}

void EnchantDictDescribe(const char * const lang_tag,
	const char * const provider_name,
	const char * const provider_desc,
	const char * const provider_file,
	void * user_data)
{
	dictdetect *detected = (dictdetect*)user_data;
	detected->add(lang_tag);
}

#endif

StyleDialog::StyleDialog (
    wxWindow *parent,
    wxIcon icon,
    const std::string& bufferParameterUtf8,
    const wxString& fileNameParameter,
    const wxString& ruleSetDirectoryParameter,
    const wxString& filterDirectoryParameter,
    const wxString& ruleSetPresetParameter,
    const wxString& filterPresetParameter,
#if !defined(USE_ENCHANT) && defined(__WXMSW__)
    const wxString& aspellDataPathParameter,
    const wxString& aspellDictPathParameter,
#endif
    int typeParameter,
    bool readOnlyParameter,
    wxPoint position,
    wxSize size )
		: wxDialog (
		    parent,
		    wxID_ANY,
		    wxString ( ( typeParameter == ID_TYPE_STYLE) ? _ ( "Style" ) : _ ( "Spelling" ) ),
		    position,
		    size,
		    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX ),
		indexForContextMenu ( -1 ),
		bufferUtf8 ( bufferParameterUtf8 ),
#if !defined(USE_ENCHANT) && defined(__WXMSW__)
		aspellDataPath ( aspellDataPathParameter ),
		aspellDictPath ( aspellDictPathParameter ),
#endif
		fileName ( fileNameParameter ),
		ruleSetDirectory ( ruleSetDirectoryParameter ),
		filterDirectory ( filterDirectoryParameter ),
		ruleSetPreset ( ruleSetPresetParameter ),
		filterPreset ( filterPresetParameter ),
		type(typeParameter),
		readOnly ( readOnlyParameter )
{
	SetIcon ( icon );


	// top box
	ruleSetCombo = new wxComboBox (
	    this,
	    ID_STYLE_COMBO_RULESET,
	    _T ( "" ),
	    wxDefaultPosition,
	    wxSize ( 200, -1 )
	);
	
	int width, height;
	ruleSetCombo->GetSize ( &width, &height );
	wxSize buttonSize ( 100, height );

    	filterCombo = new wxComboBox (
		this,
		ID_STYLE_COMBO_FILTER,
		_T ( "" ),
		wxDefaultPosition,
		wxSize ( 200, -1 ) );
	//if (type != ID_TYPE_STYLE) // from v. 1.1.0.7: never show
		filterCombo->Show ( false );

	wxButton *createReportButton = new wxButton (
	    this,
	    ID_STYLE_REPORT,
	    _ ( "&Check" ),
	    wxDefaultPosition,
	    buttonSize,
	    0 );

	wxBoxSizer *comboSizer = new wxBoxSizer ( wxHORIZONTAL );
	comboSizer->Add ( ruleSetCombo, 0, wxRIGHT, 10 );
	comboSizer->Add ( filterCombo, 0, wxRIGHT, 10 );
	comboSizer->Add ( createReportButton, 0, wxRIGHT, 10 );

	// middle box
	wxListCtrl *myTable = new wxListCtrl (
	    this,
	    ID_STYLE_TABLE,
	    wxPoint ( 0, 0 ),
	    wxSize ( -1, -1 ),
	    wxLC_REPORT );
	int widthUnit = 35;
	myTable->InsertColumn ( 0, _ ( "No." ), wxLIST_FORMAT_LEFT, widthUnit * 1 );
	myTable->InsertColumn ( 1, _ ( "Context" ), wxLIST_FORMAT_RIGHT, widthUnit * 3 );
	myTable->InsertColumn ( 2, _ ( "Error" ), wxLIST_FORMAT_CENTER, widthUnit * 3 );
	myTable->InsertColumn ( 3, _ ( "Context" ), wxLIST_FORMAT_LEFT, widthUnit * 3 );
	myTable->InsertColumn ( 4, _ ( "Suggestion" ), wxLIST_FORMAT_LEFT, widthUnit * 3 );
	
	myTable->InsertColumn ( 5, _ ( "Rule" ), wxLIST_FORMAT_LEFT, widthUnit * 3 );
	myTable->InsertColumn ( 6, _ ( "Action" ), wxLIST_FORMAT_LEFT, widthUnit * 3 );
	table = myTable;

	// lower box
	wxButton *editItemsButton =
	    new wxButton (
	    this,
	    ID_STYLE_EDIT,
	    _ ( "&Apply changes" ),
	    wxDefaultPosition,
	    wxSize ( -1, buttonSize.GetHeight() ),
	    0 );
	wxButton *webReportButton =
	    new wxButton (
	    this,
	    ID_STYLE_WEB_REPORT,
	    _ ( "&Printable report" ),
	    wxDefaultPosition,
	    wxSize ( -1, buttonSize.GetHeight() ),
	    0 );
	wxButton *webSummaryButton =
	    new wxButton (
	    this,
	    ID_STYLE_WEB_SUMMARY,
	    _ ( "Pr&intable summary" ),
	    wxDefaultPosition,
	    wxSize ( -1, buttonSize.GetHeight() ),
	    0 );
	wxButton *selectAllButton =
	    new wxButton (
	    this,
	    ID_STYLE_CHANGE_ALL,
	    _ ( "C&hange all" ),
	    wxDefaultPosition,
	    wxSize ( -1, buttonSize.GetHeight() ),
	    0 );
	wxButton *deselectAllButton =
	    new wxButton (
	    this,
	    ID_STYLE_IGNORE_ALL,
	    _ ( "I&gnore all" ),
	    wxDefaultPosition,
	    wxSize ( -1, buttonSize.GetHeight() ),
	    0 );
	wxButton *cancelButton =
	    new wxButton (
	    this,
	    wxID_CANCEL,
	    _ ( "Ca&ncel" ),
	    wxDefaultPosition,
	    wxSize ( -1, buttonSize.GetHeight() ),
	    0 );

	wxBoxSizer *reportButtonSizer = new wxBoxSizer ( wxHORIZONTAL );
	reportButtonSizer->Add ( editItemsButton, 0, wxRIGHT, 10 );
	reportButtonSizer->Add ( webReportButton, 0, wxLEFT | wxRIGHT, 10 );
	reportButtonSizer->Add ( webSummaryButton, 0, wxRIGHT, 10 );
	reportButtonSizer->Add ( selectAllButton, 0, wxLEFT | wxRIGHT, 10 );
	reportButtonSizer->Add ( deselectAllButton, 0, wxRIGHT, 10 );
	reportButtonSizer->Add ( cancelButton, 0, wxLEFT, 10 );

	// status bar
	status = new wxStatusBar ( this, wxID_ANY );

	// overall sizer
	wxBoxSizer *reportTopSizer = new wxBoxSizer ( wxVERTICAL );
	reportTopSizer->Add ( comboSizer, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
	reportTopSizer->Add ( table, 1, wxEXPAND | wxALL, 5 );
	reportTopSizer->Add ( reportButtonSizer, 0, wxALL, 5 );
	reportTopSizer->Add ( status, 0, wxEXPAND | wxALL );
	this->SetSizer ( reportTopSizer );

	createReportButton->SetFocus();

	if ( readOnly )
		filterCombo->Enable ( false );

	// keyboard shortcuts
	wxAcceleratorEntry entries[7];
	entries[0].Set ( wxACCEL_ALT, ( int ) 'C', ID_STYLE_REPORT );
	entries[1].Set ( wxACCEL_ALT, ( int ) 'A', ID_STYLE_EDIT );
	entries[2].Set ( wxACCEL_ALT, ( int ) 'W', ID_STYLE_WEB_REPORT );
	entries[3].Set ( wxACCEL_ALT, ( int ) 'B', ID_STYLE_WEB_SUMMARY );
	entries[4].Set ( wxACCEL_ALT, ( int ) 'H', ID_STYLE_CHANGE_ALL );
	entries[5].Set ( wxACCEL_ALT, ( int ) 'I', ID_STYLE_IGNORE_ALL );
	entries[6].Set ( wxACCEL_ALT, ( int ) 'N', wxID_CANCEL );

	wxAcceleratorTable accel ( 7, entries );
	this->SetAcceleratorTable ( accel );
	
	// update combo lists

	// special case spellcheck
	if (type == ID_TYPE_SPELL)
	{
#ifdef USE_ENCHANT
		EnchantBroker *broker = enchant_broker_init();
		dictdetect adetected(ruleSetCombo);
		enchant_broker_list_dicts(broker, EnchantDictDescribe, &adetected);
		bool anyFound = !adetected.empty();
#else
		AspellConfig *config;
		AspellDictInfoList *dlist;
		AspellDictInfoEnumeration *dels;
		const AspellDictInfo *entry;
		
		config = new_aspell_config();
		
#ifdef __WXMSW__
		aspell_config_replace ( config, "data-dir", aspellDataPath.mb_str() ); //ASPELL_DATA_PATH );
		aspell_config_replace ( config, "dict-dir", aspellDictPath.mb_str() ); //ASPELL_DICT_PATH );
#endif
		dlist = get_aspell_dict_info_list( config );
		
		delete_aspell_config ( config );
		
		dels = aspell_dict_info_list_elements ( dlist );
		
		bool anyFound = false;
		while ( ( entry = aspell_dict_info_enumeration_next ( dels ) ) != 0 )
		{
			anyFound = true;
			std::string stdEntry = entry->name;
			wxString entry = wxString ( stdEntry.c_str(), wxConvUTF8, stdEntry.size() );
			ruleSetCombo->Append ( entry );
		}
#endif
		
		if ( anyFound )
		{
			if ( ruleSetPreset.empty() )
				ruleSetPreset = _ ( "en_US" );
			ruleSetCombo->SetValue ( ruleSetPreset );
		}
		else
		{
			ruleSetCombo->Append ( _ ( "(No dictionaries found)" ) );
			ruleSetCombo->Select ( 0 );
			createReportButton->Enable ( false );
		}
		
		return;
	}

	// all other branches
	if ( wxDirExists ( ruleSetDirectory ) )
	{
		wxString ruleMask, ruleFile;
		ruleMask = ruleSetDirectory + wxFileName::GetPathSeparator() + _T ( "*.xml" );
		ruleFile = wxFindFirstFile ( ruleMask, wxFILE );

		if ( !ruleFile.empty() )
		{
			ruleFile.Replace ( _T ( ".xml" ), _T ( "" ) );
			ruleFile.Replace ( _T ( "_" ), _T ( " " ) );
			ruleSetCombo->Append ( wxFileNameFromPath ( ruleFile ) );
			for ( ;; )
			{
				ruleFile = wxFindNextFile();
				if ( ruleFile.empty() )
					break;
				ruleFile.Replace ( _T ( ".xml" ), _T ( "" ) );
				ruleFile.Replace ( _T ( "_" ), _T ( " " ) );
				ruleSetCombo->Append ( wxFileNameFromPath ( ruleFile ) );
			}
		}
		if ( ruleSetPreset.empty() )
			ruleSetPreset = _ ( "Default" );
		ruleSetCombo->SetValue ( ruleSetPreset );
	}
	else
	{
		ruleSetCombo->Append ( _ ( "(No rule sets found)" ) );
		ruleSetCombo->Select ( 0 );
	}

	if ( wxDirExists ( filterDirectory ) )
	{
		filterCombo->Append ( _ ( "(No filter)" ) );
		wxString filterMask, filterFile;
		filterMask = filterDirectory + wxFileName::GetPathSeparator() + _T ( "*.xml" );

		filterFile = wxFindFirstFile ( filterMask, wxFILE );

		if ( !filterFile.empty() )
		{
			filterFile.Replace ( _T ( ".xml" ), _T ( "" ) );
			filterCombo->Append ( wxFileNameFromPath ( filterFile ) );
			for ( ;; )
			{
				filterFile = wxFindNextFile();
				if ( filterFile.empty() )
					break;
				filterFile.Replace ( _T ( ".xml" ), _T ( "" ) );
				filterCombo->Append ( wxFileNameFromPath ( filterFile ) );
			}
		}
		filterCombo->SetValue ( filterPreset );
	}
	else
	{
		filterCombo->Append ( _ ( "(No filters found)" ) );
		filterCombo->Select ( 0 );
	}
}

StyleDialog::~StyleDialog()
{
	std::set<wxString>::iterator it;
	for ( it = tempFiles.begin(); it != tempFiles.end(); ++it )
		wxRemoveFile ( *it );
}

void StyleDialog::OnColumnClick ( wxListEvent& event )
{
	std::auto_ptr<SortData> data ( new SortData );
	data->column = event.GetColumn();
	data->table = table;
	table->SortItems ( MyCompareFunction, ( wxIntPtr ) data.get() );

	long itemCount = table->GetItemCount();
	for ( int i = 0; i < itemCount; ++i )
		table->SetItemData ( i, i );
}

void StyleDialog::OnItemActivated ( wxListEvent& event )
{
	int index = event.GetIndex();
	bool ignore = ( getTextByColumn ( table, index, 6 ) == _ ( "Ignore" ) );
	setIgnore ( index, ( ignore ) ? false : true );
}

void StyleDialog::OnItemRightClick ( wxListEvent& event )
{
	int index = indexForContextMenu = event.GetIndex();

	wxString match, suggestion;
	match = getTextByColumn ( table, index, 2 );
	suggestion = getTextByColumn ( table, index, 4 );

	wxMenu contextMenu;
	contextMenu.Append (
	    ID_MENU_IGNORE_ONCE, _ ( "Ignore once" ) );
	contextMenu.Append (
	    ID_MENU_IGNORE_ALL, _ ( "Ignore all" ) );
	contextMenu.AppendSeparator();
	contextMenu.Append ( ID_MENU_CHANGE_ONCE, _ ( "Change once" ) );
	contextMenu.Append ( ID_MENU_CHANGE_ALL, _ ( "Change all" ) );
	contextMenu.AppendSeparator();
	contextMenu.Append (
	    ID_MENU_NEW_SUGGESTION, _ ( "New suggestion..." ) );

	wxString menuString;
	menuString.Printf ( _T ( "Change '%s' to '%s' throughout" ), match.c_str(), suggestion.c_str() );

	contextMenu.Append (
	    ID_MENU_APPLY_SUGGESTION_ALL, menuString );

	bool ignore = ( getTextByColumn ( table, index, 6 ) == _ ( "Ignore" ) );
	contextMenu.Enable ( ( ignore ) ? ID_MENU_IGNORE_ONCE : ID_MENU_CHANGE_ONCE, false );
	PopupMenu ( &contextMenu, wxPoint ( -1, -1 ) );
}

void StyleDialog::OnCancel ( wxCommandEvent& event )
{
	updateSizeInformation();
	event.Skip();
}

void StyleDialog::OnReport ( wxCommandEvent& event )
{
	table->DeleteAllItems();
	matchVector.clear();
	status->SetStatusText ( _ ( "Checking document..." ) );

	// update presets
	ruleSetPreset = ruleSetCombo->GetValue();
	filterPreset = filterCombo->GetValue();

	// reconstitute short filenames
	wxString ruleSet, filter;
	ruleSet = ruleSetPreset + _T ( ".xml" );
	ruleSet.Replace ( _(" "), _T("_") );
	filter = filterPreset + _T ( ".xml" );

	wxString separator = wxFileName::GetPathSeparator();

	std::auto_ptr<HouseStyle> hs ( new HouseStyle (
					   (type == ID_TYPE_SPELL) ? HS_TYPE_SPELL : HS_TYPE_STYLE,
	                                   bufferUtf8,
	                                   ruleSetDirectory,
	                                   ruleSet,
	                                   filterDirectory,
	                                   filter,
	                                   separator,
 #ifdef __WXMSW__
	                                   aspellDataPath,
	                                   aspellDictPath,
 #endif
	                                   5 ) );

	status->SetStatusText ( _ ( "Checking document..." ) );
	if ( !hs->createReport() )
	{
		const wxString &error = hs->getLastError();
		status->SetStatusText ( _ ( "Cannot check document: " ) + error );
		return;
	}
	matchVector = hs->getMatchVector();

	vector<ContextMatch>::iterator it;
	std::string prelogUtf8, matchUtf8, postlogUtf8, replaceUtf8, reportUtf8;
	wxString matchNo, prelog, match, postlog, replace, report;
	int i = 0;
	for ( it = matchVector.begin(); it != matchVector.end(); ++it )
	{
		matchNo.Printf ( _T ( "%i" ), i + 1 ); // display numbers from 1
		prelogUtf8 = flatWhiteSpace ( ( *it ).prelog );
		matchUtf8 = flatWhiteSpace ( ( *it ).match );
		postlogUtf8 = flatWhiteSpace ( ( *it ).postlog );
		replaceUtf8 = flatWhiteSpace ( ( *it ).replace );
		reportUtf8 = flatWhiteSpace ( ( *it ).report );
		prelog = wxString ( prelogUtf8.c_str(), wxConvUTF8, ( *it ).prelog.size() );
		match = wxString ( matchUtf8.c_str(), wxConvUTF8, ( *it ).match.size() );
		postlog = wxString ( postlogUtf8.c_str(), wxConvUTF8, ( *it ).postlog.size() );
		replace = wxString ( replaceUtf8.c_str(), wxConvUTF8, ( *it ).replace.size() );
		report = wxString ( reportUtf8.c_str(), wxConvUTF8, ( *it ).report.size() );
		table->InsertItem ( i, matchNo, 0 );
		table->SetItem ( i, 1, prelog );
		table->SetItem ( i, 2, match );
		table->SetItem ( i, 3, postlog );
		table->SetItem ( i, 4, replace );
		table->SetItem ( i, 5, report );
		setIgnore ( i, ( *it ).tentative );
		table->SetItemData ( i, i );
		++i;
	}
	wxString message;
	message.Printf ( wxPLURAL ( "%i error", "%i errors", i ), i );
	status->SetStatusText ( message );
	if ( i )
		table->SetFocus();
}

void StyleDialog::OnStyleEdit ( wxCommandEvent& event )
{
	updateSizeInformation();

	std::vector<ContextMatch> v;
	getSelectedMatches ( v );

	if ( v.empty() )
	{
		status->SetStatusText ( _ ( "No items selected" ) );
		return;
	}

	sort ( v.begin(), v.end(), elementAndOffsetCompareFunction );

/*
	HouseStyleWriter hsw ( v );
	if ( !hsw.parse ( bufferUtf8 ) )
	{
		std::string error = hsw.getLastError();
		wxString wideError = wxString (
		                         error.c_str(),
		                         wxConvUTF8,
		                         error.size() );

		status->SetStatusText ( wideError );
		return;
	}
	bufferUtf8 = hsw.getOutput();
*/


	//unsigned elementCount = 1; // from v. 1.1.0.7: one raw text element only
        int vectorsize, os_adjust, exclusion;

        vectorsize = v.size();
        os_adjust = exclusion = 0;
        string cmp1, cmp2, buffer;
	buffer = bufferUtf8;

        for ( int i = 0; i < vectorsize; ++i )
        {
/*
                unsigned vectorElementCount = v[i].elementCount;
                if ( vectorElementCount < elementCount )
                        continue;
                else if ( vectorElementCount > elementCount )
                        break;
                else if ( vectorElementCount == elementCount )
                {
*/
                        int offset = ( int ) v[i].offset + os_adjust;

                        if ( offset < exclusion )
                                continue;

                        try
                        {
                                cmp1 = v[i].match;
                                cmp2 = buffer.substr ( offset, v[i].match.size() );
                        }
                        catch ( std::exception& e )
                        {
                                continue;
                        }

                        if ( cmp1.compare ( cmp2 ) )
                                continue;

                        buffer.replace ( offset, v[i].match.size(), v[i].replace.c_str() );

                        os_adjust += v[i].replace.size() - v[i].match.size();
                        exclusion = offset + v[i].replace.size();

		//}

	}
	bufferUtf8 = buffer;

	wxCommandEvent e;
	EndModal ( wxID_OK );
}

std::string StyleDialog::getEditedString()
{
	return bufferUtf8;
}

void StyleDialog::OnStyleIgnoreAll ( wxCommandEvent& event )
{
	styleSetIgnoreAll ( true );
}

void StyleDialog::OnStyleChangeAll ( wxCommandEvent& event )
{
	styleSetIgnoreAll ( false );
}

void StyleDialog::OnStyleWebReport ( wxCommandEvent& event )
{
	std::vector<ContextMatch> v;
	getAllMatches ( v );

	sort ( v.begin(), v.end(), reportCompareFunction );

	// temporary file should be in default temporary folder
	wxString tempNameWide = wxFileName::CreateTempFileName ( _T ( "" ) );
	if ( tempNameWide.empty() )
		return;
	tempNameWide.Replace ( _T ( ".tmp" ), _T ( "_report.html" ), true );
	tempFiles.insert ( tempNameWide );
	std::string tempNameUtf8 = ( const char * ) tempNameWide.mb_str ( wxConvUTF8 );
	std::ofstream ofs ( tempNameUtf8.c_str() );
	if ( !ofs )
		return;

	WrapExpat we;

	ofs << XHTML_START;
	ofs << "<body><h2>";
	ofs << fileName.mb_str ( wxConvUTF8 );
	ofs << "</h2><table><tr>";
	ofs << "<td><b align=\"right\">No.</b></td>";
	ofs << "<td align=\"right\"><b>Context</b></td>";
	ofs << "<td align=\"center\"><b>Match</b></td>";
	ofs << "<td align=\"left\"><b>Context</b></td>";
	ofs << "<td><b>Suggestion</b></td><td><b>Report</b></td></tr>";
	std::vector<ContextMatch>::iterator it;
	int matchCount = 0;
	for ( it = v.begin(); it != v.end(); ++it )
	{
		ofs << "<tr><td align=\"right\">";
		ofs << ++matchCount;
		ofs << "</td>";
		ofs << "<td align=\"right\">";
		ofs << we.xmliseTextNode ( it->prelog );
		ofs << "</td><td align=\"center\"><font color=\"red\"><b>";
		ofs << we.xmliseTextNode ( it->match );
		ofs << "</b></font></td><td align=\"left\">";
		ofs << we.xmliseTextNode ( it->postlog );
		ofs << "</td><td><font color=\"green\"><b>";
		ofs << we.xmliseTextNode ( it->replace );
		ofs << "</b></font></td><td>";
		ofs << we.xmliseTextNode ( it->report );
		ofs << "</td></tr>";
	}
	ofs << "</table></body>";
	ofs << XHTML_END;
	ofs.close();

	// display file in browser
	if ( !wxFileExists ( tempNameWide ) )
		return;

	wxLaunchDefaultBrowser ( tempNameWide );
}

void StyleDialog::OnStyleWebSummary ( wxCommandEvent& event )
{
	std::vector<ContextMatch> v;
	getAllMatches ( v );

	std::map<std::string, int, NoCaseCompare> matchMap;
	std::vector<ContextMatch>::iterator vectorIterator;
	for (
	    vectorIterator = v.begin();
	    vectorIterator != v.end();
	    ++vectorIterator )
	{
		if ( ( matchMap.find ( vectorIterator->match ) ) != matchMap.end() )
			++ ( matchMap[vectorIterator->match] );
		else
			matchMap[vectorIterator->match] = 1;
	}

	// temporary file should be in default temporary folder
	wxString tempNameWide = wxFileName::CreateTempFileName ( _T ( "" ) );
	if ( tempNameWide.empty() )
		return;
	tempNameWide.Replace ( _T ( ".tmp" ), _T ( "_summary.html" ), true );
	tempFiles.insert ( tempNameWide );
	std::string tempNameUtf8 = ( const char * ) tempNameWide.mb_str ( wxConvUTF8 );
	std::ofstream ofs ( tempNameUtf8.c_str() );
	if ( !ofs )
		return;
	ofs << XHTML_START;
	ofs << "<body><h2>";
	ofs << fileName.mb_str ( wxConvUTF8 );
	ofs << "</h2>";

	WrapExpat we;
	ofs << "<table><tr><th>Term</th><th>Frequency</th></tr>";
	std::map<std::string, int>::iterator mapIterator;
	int matchTotal = 0;

	for (
	    mapIterator = matchMap.begin();
	    mapIterator != matchMap.end();
	    ++mapIterator )
	{
		ofs << "<tr><td>";
		ofs << we.xmliseTextNode ( mapIterator->first );
		ofs << "</td><td>";

		// handle number of matches
		matchTotal += mapIterator->second;
		ofs << mapIterator->second;
		ofs << "</td></tr>";
	}
	ofs << "<tr><th>Total</th><th>";
	ofs << matchTotal;
	ofs << "</th></tr></table></body>";
	ofs << XHTML_END;

	ofs.close();

	// display file in browser
	if ( !wxFileExists ( tempNameWide ) )
		return;

	wxLaunchDefaultBrowser ( tempNameWide );
}

void StyleDialog::styleSetIgnoreAll ( bool ignore )
{
	int count = table->GetItemCount();
	for ( int i = 0; i < count; ++i )
		setIgnore ( i, ignore );
}

bool StyleDialog::isIgnore ( int item )
{
	wxString field = getTextByColumn ( table, item, 6 );
	return ( field == _ ( "Ignore" ) ) ? true : false;
}

void StyleDialog::setIgnore ( int item, bool ignore )
{
	table->SetItem ( item, 6, ( ignore ) ? _ ( "Ignore" ) : _ ( "Change" ) );
	table->SetItemTextColour ( item, ( ignore ) ? *wxBLUE : *wxBLACK );
}

void StyleDialog::getAllMatches ( vector<ContextMatch> &v )
{
	v.clear();
	int count = table->GetItemCount();
	wxString matchNoString;
	long matchNo;
	for ( int i = 0; i < count; ++i )
	{
		matchNoString = table->GetItemText ( i );
		if ( !matchNoString.ToLong ( &matchNo ) || matchNo < 1 )
			continue;
		v.push_back ( matchVector.at ( matchNo - 1 ) ); // vector index starts at 0
	}
}

void StyleDialog::getSelectedMatches ( vector<ContextMatch> &v )
{
	v.clear();
	int count = table->GetItemCount();
	wxString selectionString, matchNoString;
	long matchNo;
	for ( int i = 0; i < count; ++i )
	{
		selectionString = getTextByColumn ( table, i, 6 );
		if ( selectionString != _ ( "Change" ) )
			continue;
		matchNoString = table->GetItemText ( i );
		if ( !matchNoString.ToLong ( &matchNo ) || matchNo < 1 )
			continue;
		v.push_back ( matchVector.at ( matchNo - 1 ) ); // vector index starts at 0
	}
}

int wxCALLBACK StyleDialog::MyCompareFunction (
#if wxCHECK_VERSION(2,9,0) || defined (_WIN64) || defined (__x86_64__)
    wxIntPtr item1,
    wxIntPtr item2,
    wxIntPtr sortData )
#else
    long item1,
    long item2,
    long sortData )
#endif
{
	SortData *data = ( SortData * ) sortData;
	int column;
	wxListCtrl *table;
	column = data->column;
	table = data->table;

	wxString string1 = getTextByColumn ( table, item1, column );
	wxString string2 = getTextByColumn ( table, item2, column );

	// special case: numerical content
	if ( string1.IsNumber() && string2.IsNumber() )
	{
		long value1, value2;
		string1.ToLong ( &value1 );
		string2.ToLong ( &value2 );
		if ( value1 < value2 )
			return -1;
		else if ( value1 > value2 )
			return 1;
		else
			return 0;
	}

	if ( string1.CmpNoCase ( string2 ) < 0 )
		return -1;
	else if ( string1.CmpNoCase ( string2 ) > 0 )
		return 1;
	else
		return 0;
}

bool StyleDialog::elementAndOffsetCompareFunction (
    ContextMatch m1,
    ContextMatch m2 )
{
	if ( m1.elementCount == m2.elementCount )
		return ( m1.offset < m2.offset );
	return ( m1.elementCount < m2.elementCount );
}

bool StyleDialog::reportCompareFunction ( ContextMatch m1, ContextMatch m2 )
{
	return ( m1.report < m2.report );
}

wxString StyleDialog::getTextByColumn ( wxListCtrl *table, long index, int col )
{
	wxListItem Item;
	Item.SetId ( index );
	Item.SetColumn ( col );
	Item.SetMask ( wxLIST_MASK_TEXT );
	table->GetItem ( Item );
	return Item.GetText();
}

void StyleDialog::OnUpdateTableRange ( wxUpdateUIEvent& event )
{
	if ( event.GetId() == ID_STYLE_EDIT && readOnly )
	{
		event.Enable ( false );
		return;
	}
	event.Enable ( table->GetItemCount() );
}

void StyleDialog::OnMenuChangeOnce ( wxCommandEvent& event )
{
	setIgnore ( indexForContextMenu, false );
}

void StyleDialog::OnMenuChangeAll ( wxCommandEvent& event )
{
	wxString match, suggestion;
	match = getTextByColumn ( table, indexForContextMenu, 2 );
	suggestion = getTextByColumn ( table, indexForContextMenu, 4 );

	long itemCount = table->GetItemCount();
	for ( int i = 0; i < itemCount; ++i )
	{
		if ( getTextByColumn ( table, i, 2 ) == match )
			setIgnore ( i, false );
	}
}

void StyleDialog::OnMenuIgnoreOnce ( wxCommandEvent& event )
{
	setIgnore ( indexForContextMenu, true );
}

void StyleDialog::OnMenuIgnoreAll ( wxCommandEvent& event )
{
	wxString match, suggestion;
	match = getTextByColumn ( table, indexForContextMenu, 2 );
	suggestion = getTextByColumn ( table, indexForContextMenu, 4 );

	long itemCount = table->GetItemCount();
	for ( int i = 0; i < itemCount; ++i )
	{
		if ( getTextByColumn ( table, i, 2 ) == match )
		{
			table->SetItem ( i, 4, suggestion );
			setIgnore ( i, true );
		}
	}
}

void StyleDialog::OnMenuNewSuggestion ( wxCommandEvent& event )
{
	wxString suggestion = getTextByColumn ( table, indexForContextMenu, 4 );
	wxTextEntryDialog *dlg = new wxTextEntryDialog (
	    this,
	    _ ( "Enter new suggestion:" ),
	    _ ( "New Suggestion" ),
	    suggestion );
	if ( !dlg )
		return;

	int ret = dlg->ShowModal();
	if ( ret == wxID_CANCEL )
		return;

	// identify item in match vector
	wxString noString = getTextByColumn ( table, indexForContextMenu, 0 );
	long no;
	if ( !noString.ToLong ( &no ) || no < 1 || no > ( long ) matchVector.size() )
		return;
	--no; // reverse display offset

	wxString wideReplace = dlg->GetValue();
	std::string replace = ( const char * ) wideReplace.mb_str ( wxConvUTF8 );
	matchVector[no].replace = replace;
	table->SetItem ( indexForContextMenu, 4, dlg->GetValue() );
	setIgnore ( indexForContextMenu, false );
}

void StyleDialog::OnMenuApplySuggestionAll ( wxCommandEvent& event )
{
	wxString match, suggestion;
	match = getTextByColumn ( table, indexForContextMenu, 2 );
	suggestion = getTextByColumn ( table, indexForContextMenu, 4 );

	long itemCount = table->GetItemCount();
	for ( int i = 0; i < itemCount; ++i )
	{
		if ( getTextByColumn ( table, i, 2 ) != match )
			continue;

		// identify item in match vector
		wxString noString = getTextByColumn ( table, i, 0 );
		long no;
		if ( !noString.ToLong ( &no ) || no < 1 || no > ( long ) matchVector.size() )
			continue;
		--no; // reverse display offset

		std::string replace = ( const char * ) suggestion.mb_str ( wxConvUTF8 );
		matchVector[no].replace = replace;
		table->SetItem ( i, 4, suggestion );
		setIgnore ( i, false );
	}
}

std::string StyleDialog::flatWhiteSpace ( std::string& s )
{
	std::string::iterator it;
	std::string output;
	output.reserve ( s.size() );
	for ( it = s.begin(); it != s.end(); ++it )
	{
		if ( *it == '\t' || *it == '\r' || *it == '\n' )
			output += ' ';
		else
			output += *it;
	}
	return output;
}

void StyleDialog::updateSizeInformation()
{
	framePosition = GetPosition();
	frameSize = GetSize();
}

wxPoint StyleDialog::getPosition()
{
	return framePosition;
}

wxSize StyleDialog::getSize()
{
	return frameSize;
}
