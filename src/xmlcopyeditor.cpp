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

#include <iostream>
#include <fstream>
#include <string>
#include <wx/aboutdlg.h>
#include "xmlcopyeditor.h"
#include "xmlcopyeditorcopy.h"
#include "readfile.h"
#include "xmldoc.h"
#include "xmlctrl.h"
#include "wraplibxml.h"
#include "xmlschemalocator.h"
#include "xsllocator.h"
#include "xmlutf8reader.h"
#include "xmlpromptgenerator.h"
#include "xmlencodingspy.h"
#include "styledialog.h"
#include "mypropertysheet.h"
#include "wraptempfilename.h"
#include "globalreplacedialog.h"
#include "replace.h"
#include "associatedialog.h"
#include "xmlassociatexsd.h"
#include "xmlassociatexsl.h"
#include "xmlassociatedtd.h"
#include "wrapdaisy.h"
#include "aboutdialog.h"
#include "pathresolver.h"
#include "locationpanel.h"
#include "insertpanel.h"
#include "xmlwordcount.h"
#include "mynotebook.h"
#include "commandpanel.h"
#include "binaryfile.h"
#include "exportdialog.h"
#include <wx/aui/auibook.h>
#include <wx/richtext/richtextsymboldlg.h>
#include <wx/textctrl.h>
#include <wx/artprov.h>
#include <wx/stockitem.h>
#include <iconv.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/dir.h>
#include "xmlschemagenerator.h"
#include "threadreaper.h"
#include <wx/wupdlock.h>
#include "dtd2schema.h"
#include "myipc.h"

#ifdef NEWFINDREPLACE
#include "findreplacepanel.h"
#endif

#include "wrapxerces.h"
#ifndef __WXMSW__
#include "xpm/appicon.xpm"
#endif

typedef size_t universal_iconv (iconv_t cd,
        char* * inbuf, size_t * inbytesleft,
        char* * outbuf, size_t * outbytesleft);
/* On other platform, it could be:
size_t iconv (iconv_t cd,
        const char* * inbuf, size_t * inbytesleft,
        char* * outbuf, size_t * outbytesleft);
and a char ** can't be assigned to const char **
    http://c-faq.com/ansi/constmismatch.html
Let's deal with this mess.
*/

BEGIN_EVENT_TABLE ( MyFrame, wxFrame )
	EVT_ACTIVATE_APP ( MyFrame::OnActivateApp )
	EVT_CLOSE ( MyFrame::OnFrameClose )
	EVT_KEY_DOWN ( MyFrame::OnKeyPressed )
	EVT_MENU ( wxID_ABOUT, MyFrame::OnAbout )
	EVT_MENU ( wxID_CLOSE, MyFrame::OnClose )
	EVT_MENU ( wxID_CLOSE_ALL, MyFrame::OnCloseAll )
	EVT_MENU ( wxID_CUT, MyFrame::OnCut )
	EVT_MENU ( wxID_COPY, MyFrame::OnCopy )
	EVT_MENU ( wxID_HELP, MyFrame::OnHelp )
	EVT_MENU ( wxID_PASTE, MyFrame::OnPaste )
	EVT_MENU ( ID_PASTE_NEW_DOCUMENT, MyFrame::OnPasteNewDocument )
	EVT_MENU ( wxID_EXIT, MyFrame::OnQuit )
	EVT_MENU ( wxID_NEW, MyFrame::OnNew )
	EVT_MENU ( wxID_OPEN, MyFrame::OnOpen )
	EVT_MENU ( wxID_SAVE, MyFrame::OnSave )
	EVT_MENU ( wxID_SAVEAS, MyFrame::OnSaveAs )
	EVT_MENU ( ID_RELOAD, MyFrame::OnReload )
	EVT_MENU ( wxID_UNDO, MyFrame::OnUndo )
	EVT_MENU ( wxID_REDO, MyFrame::OnRedo )
	EVT_MENU ( wxID_REVERT, MyFrame::OnRevert )
	EVT_MENU ( ID_INSERT_CHILD, MyFrame::OnInsertChild )
	EVT_MENU ( ID_INSERT_SIBLING, MyFrame::OnInsertSibling )
	EVT_MENU ( ID_INSERT_ENTITY, MyFrame::OnInsertEntity )
	EVT_MENU ( ID_INSERT_TWIN, MyFrame::OnInsertTwin )
	EVT_MENU ( ID_INSERT_SYMBOL, MyFrame::OnInsertSymbol )
	EVT_MENU ( ID_TOGGLE_FOLD, MyFrame::OnToggleFold )
	EVT_MENU ( ID_FOLD_ALL, MyFrame::OnFoldAll )
	EVT_MENU ( ID_UNFOLD_ALL, MyFrame::OnUnfoldAll )
	EVT_MENU ( ID_OPEN_LARGE_FILE, MyFrame::OnOpen )
	EVT_MENU ( ID_PRINT_PREVIEW, MyFrame::OnPrintPreview )
	EVT_MENU ( ID_PRINT_SETUP, MyFrame::OnPrintSetup )
	EVT_MENU ( ID_PRINT, MyFrame::OnPrint )
	EVT_MENU ( ID_WORD_COUNT, MyFrame::OnWordCount )
	EVT_MENU ( ID_IMPORT_MSWORD, MyFrame::OnImportMSWord )
	EVT_MENU ( ID_EXPORT_MSWORD, MyFrame::OnExportMSWord )
	EVT_MENU ( ID_EXPORT, MyFrame::OnExport )
	EVT_MENU ( ID_CLOSE_MESSAGE_PANE, MyFrame::OnCloseMessagePane )
	EVT_MENU ( ID_CLOSE_FIND_REPLACE_PANE, MyFrame::OnCloseFindReplacePane )
	EVT_MENU ( ID_CLOSE_COMMAND_PANE, MyFrame::OnCloseCommandPane )
	EVT_MENU ( ID_COMMAND, MyFrame::OnCommand )
	EVT_MENU ( ID_FIND, MyFrame::OnFind )
	EVT_MENU ( ID_FIND_AGAIN, MyFrame::OnFindAgain )
	EVT_MENU ( ID_GOTO, MyFrame::OnGoto )
	EVT_MENU ( ID_TOGGLE_COMMENT, MyFrame::OnToggleComment )
	EVT_MENU ( ID_FEEDBACK, MyFrame::OnFeedback )
	EVT_MENU ( ID_PREVIOUS_DOCUMENT, MyFrame::OnPreviousDocument )
	EVT_MENU ( ID_NEXT_DOCUMENT, MyFrame::OnNextDocument )
	EVT_MENU ( ID_BROWSER, MyFrame::OnBrowser )
	EVT_MENU ( ID_REPLACE, MyFrame::OnFindReplace )
	EVT_MENU ( ID_GLOBAL_REPLACE, MyFrame::OnGlobalReplace )
	EVT_MENU ( ID_CHECK_WELLFORMED, MyFrame::OnCheckWellformedness )
	EVT_MENU ( ID_VALIDATE_RELAX_NG, MyFrame::OnValidateRelaxNG )
	EVT_MENU ( ID_VALIDATE_W3C_SCHEMA, MyFrame::OnValidateSchema )
	EVT_MENU ( ID_CREATE_SCHEMA, MyFrame::OnCreateSchema )
	EVT_MENU ( ID_DTD_TO_SCHEMA, MyFrame::OnDtd2Schema )
	EVT_MENU ( ID_XPATH, MyFrame::OnXPath )
	EVT_MENU_RANGE ( ID_XSLT, ID_XSLT_WORDML_DOCBOOK, MyFrame::OnXslt )
	EVT_MENU ( ID_PRETTYPRINT, MyFrame::OnPrettyPrint )
	EVT_MENU ( ID_ENCODING, MyFrame::OnEncoding )
	EVT_MENU ( ID_STYLE, MyFrame::OnSpelling )
	EVT_MENU ( ID_SPELL, MyFrame::OnSpelling )
	EVT_MENU ( ID_FONT_SMALLER, MyFrame::OnFontSmaller )
	EVT_MENU ( ID_FONT_NORMAL, MyFrame::OnFontMedium )
	EVT_MENU ( ID_FONT_LARGER, MyFrame::OnFontLarger )
	EVT_MENU ( ID_OPTIONS, MyFrame::OnOptions )
	EVT_MENU ( ID_HOME, MyFrame::OnHome )
	EVT_MENU ( ID_DOWNLOAD_SOURCE, MyFrame::OnDownloadSource )
	EVT_MENU ( ID_TOOLBAR_VISIBLE, MyFrame::OnToolbarVisible )
	EVT_MENU ( ID_LOCATION_PANE_VISIBLE, MyFrame::OnLocationPaneVisible )
	EVT_MENU ( ID_PROTECT_TAGS, MyFrame::OnProtectTags )
	EVT_MENU ( ID_WRAP_WORDS, MyFrame::OnWrapWords )
	EVT_MENU_RANGE ( ID_SHOW_TAGS, ID_HIDE_TAGS, MyFrame::OnVisibilityState )
	EVT_MENU_RANGE ( ID_ASSOCIATE_DTD_PUBLIC, ID_ASSOCIATE_XSL, MyFrame::OnAssociate )
	EVT_MENU_RANGE ( wxID_FILE1, wxID_FILE9, MyFrame::OnHistoryFile )
	EVT_MENU_RANGE (
	    ID_VALIDATE_PRESET1, ID_VALIDATE_PRESET9, MyFrame::OnValidatePreset )
	EVT_MENU_RANGE (
	    ID_COLOR_SCHEME_DEFAULT,
	    ID_COLOR_SCHEME_NONE,
	    MyFrame::OnColorScheme )
	
	EVT_MENU_RANGE (
	    ID_SPLIT_TAB_TOP,
	    ID_SPLIT_TAB_LEFT,
	    MyFrame::OnSplitTab )
	
	EVT_UPDATE_UI_RANGE ( ID_REPLACE, ID_GLOBAL_REPLACE, MyFrame::OnUpdateReplaceRange )
	EVT_FIND ( wxID_ANY, MyFrame::OnDialogFind )
	EVT_FIND_NEXT ( wxID_ANY, MyFrame::OnDialogFind )
	EVT_FIND_REPLACE ( wxID_ANY, MyFrame::OnDialogReplace )
	EVT_FIND_REPLACE_ALL ( wxID_ANY, MyFrame::OnDialogReplaceAll )
	EVT_ICONIZE ( MyFrame::OnIconize )
	EVT_UPDATE_UI ( ID_LOCATION_PANE_VISIBLE, MyFrame::OnUpdateLocationPaneVisible )
	EVT_UPDATE_UI ( wxID_CLOSE, MyFrame::OnUpdateDocRange )
	EVT_UPDATE_UI ( wxID_SAVEAS, MyFrame::OnUpdateDocRange )
	EVT_UPDATE_UI ( wxID_CLOSE_ALL, MyFrame::OnUpdateCloseAll )
	EVT_UPDATE_UI_RANGE ( ID_SPLIT_TAB_TOP, ID_SPLIT_TAB_LEFT, MyFrame::OnUpdateCloseAll )
	EVT_UPDATE_UI ( wxID_REVERT, MyFrame::OnUpdateUndo )
	EVT_UPDATE_UI ( wxID_SAVE, MyFrame::OnUpdateDocRange ) // always allow save if doc present
	EVT_UPDATE_UI ( wxID_UNDO, MyFrame::OnUpdateUndo )
	EVT_UPDATE_UI ( wxID_REDO, MyFrame::OnUpdateRedo )
	EVT_UPDATE_UI ( wxID_PASTE, MyFrame::OnUpdatePaste )
	EVT_UPDATE_UI ( wxID_CUT, MyFrame::OnUpdateCutCopy )
	EVT_UPDATE_UI ( wxID_COPY, MyFrame::OnUpdateCutCopy )
	EVT_UPDATE_UI ( ID_FIND_AGAIN, MyFrame::OnUpdateFindAgain )
	EVT_UPDATE_UI ( ID_TOGGLE_COMMENT, MyFrame::OnUpdateToggleComment )
	EVT_UPDATE_UI_RANGE ( ID_FIND, ID_EXPORT_MSWORD, MyFrame::OnUpdateDocRange )
	EVT_UPDATE_UI ( ID_PREVIOUS_DOCUMENT, MyFrame::OnUpdatePreviousDocument )
	EVT_UPDATE_UI ( ID_NEXT_DOCUMENT, MyFrame::OnUpdateNextDocument )
	EVT_UPDATE_UI ( ID_CLOSE_MESSAGE_PANE, MyFrame::OnUpdateCloseMessagePane )
	EVT_UPDATE_UI ( ID_CLOSE_FIND_REPLACE_PANE, MyFrame::OnUpdateCloseFindReplacePane )
	EVT_UPDATE_UI ( ID_CLOSE_COMMAND_PANE, MyFrame::OnUpdateCloseCommandPane )
	EVT_UPDATE_UI ( ID_RELOAD, MyFrame::OnUpdateReload )
	EVT_IDLE ( MyFrame::OnIdle )
	EVT_AUINOTEBOOK_PAGE_CLOSE ( wxID_ANY, MyFrame::OnPageClosing )
#ifdef __WXMSW__
	EVT_DROP_FILES ( MyFrame::OnDropFiles )
#endif
	EVT_NOTIFY ( myEVT_NOTIFY_PROMPT_GENERATED, wxID_ANY, MyFrame::OnPromptGenerated )
END_EVENT_TABLE()

IMPLEMENT_APP ( MyApp)

MyApp::MyApp()
	: checker ( NULL )
	, server ( NULL )
	, singleInstanceCheck ( false )
	, lang ( 0 )
#if defined(__WXMSW__) && !wxCHECK_VERSION(2,9,0)
	, config ( new wxFileConfig ( _T ( ".xmlcopyeditor" ) ) )//( _T ( "SourceForge Project\\XML Copy Editor" ) ) )
#else
	, config ( new wxFileConfig ( _T ( "xmlcopyeditor" ) ) )
#endif
{
#if defined ( __WXGTK__ ) && !defined ( __WXDEBUG__ )
	int fdnull = open ( "/dev/null", O_WRONLY, 0 );
	dup2 ( fdnull, STDERR_FILENO );
#endif
}

MyApp::~MyApp()
{
	delete checker;
	delete server;
}

bool MyApp::OnInit()
{
#ifdef __WXDEBUG__
	wxLog::SetActiveTarget ( new wxLogStderr() );
	wxLog::SetLogLevel ( wxLOG_Max );
#endif

	int systemLocale = myLocale.GetSystemLanguage();
	switch ( systemLocale )
	{
		case wxLANGUAGE_GERMAN:
		case wxLANGUAGE_GERMAN_AUSTRIAN:
		case wxLANGUAGE_GERMAN_BELGIUM:
		case wxLANGUAGE_GERMAN_LIECHTENSTEIN:
		case wxLANGUAGE_GERMAN_LUXEMBOURG:
		case wxLANGUAGE_GERMAN_SWISS:
			systemLocale = wxLANGUAGE_GERMAN;
			break;
		case wxLANGUAGE_CHINESE_SIMPLIFIED:
			systemLocale = wxLANGUAGE_CHINESE_SIMPLIFIED;
			break;
		case wxLANGUAGE_CHINESE_TRADITIONAL:
			systemLocale = wxLANGUAGE_CHINESE_TRADITIONAL;
			break;
		case wxLANGUAGE_CATALAN:
			systemLocale = wxLANGUAGE_CATALAN;
			break;
		case wxLANGUAGE_SPANISH:
		case wxLANGUAGE_SPANISH_ARGENTINA:
		case wxLANGUAGE_SPANISH_BOLIVIA:
		case wxLANGUAGE_SPANISH_CHILE:
		case wxLANGUAGE_SPANISH_COLOMBIA:
		case wxLANGUAGE_SPANISH_COSTA_RICA:
		case wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC:
		case wxLANGUAGE_SPANISH_ECUADOR:
		case wxLANGUAGE_SPANISH_EL_SALVADOR:
		case wxLANGUAGE_SPANISH_GUATEMALA:
		case wxLANGUAGE_SPANISH_HONDURAS:
		case wxLANGUAGE_SPANISH_MEXICAN:
		case wxLANGUAGE_SPANISH_MODERN:
		case wxLANGUAGE_SPANISH_NICARAGUA:
		case wxLANGUAGE_SPANISH_PANAMA:
		case wxLANGUAGE_SPANISH_PARAGUAY:
		case wxLANGUAGE_SPANISH_PERU:
		case wxLANGUAGE_SPANISH_PUERTO_RICO:
		case wxLANGUAGE_SPANISH_URUGUAY:
		case wxLANGUAGE_SPANISH_US:
		case wxLANGUAGE_SPANISH_VENEZUELA:
			systemLocale = wxLANGUAGE_SPANISH;
			break;
		case wxLANGUAGE_SLOVAK:
			systemLocale = wxLANGUAGE_SLOVAK;
			break;
		case wxLANGUAGE_SWEDISH:
			systemLocale = wxLANGUAGE_SWEDISH;
			break;
		case wxLANGUAGE_FRENCH:
			systemLocale = wxLANGUAGE_FRENCH;
			break;
		case wxLANGUAGE_UKRAINIAN:
			systemLocale = wxLANGUAGE_UKRAINIAN;
			break;
		case wxLANGUAGE_ITALIAN:
			systemLocale = wxLANGUAGE_ITALIAN;
			break;
		case wxLANGUAGE_RUSSIAN:
			systemLocale = wxLANGUAGE_RUSSIAN;
			break;
		case wxLANGUAGE_DUTCH:
			systemLocale = wxLANGUAGE_DUTCH;
			break;
		default:
			systemLocale = wxLANGUAGE_DEFAULT;
			break;
	}

	wxString dataDir = wxStandardPaths::Get().GetDataDir();
#ifdef __WXMSW__
	singleInstanceCheck = true;
#else
	singleInstanceCheck = false;
#endif
	if ( config.get() )
	{
		singleInstanceCheck = config->Read ( _T ( "singleInstanceCheck" ),
				singleInstanceCheck );
		lang = config->Read ( _T ( "lang" ), systemLocale );
		dataDir = config->Read ( _T ( "applicationDir" ), dataDir );
	}
	else
	{
		lang = systemLocale;
	}

	if ( singleInstanceCheck )
	{
		wxString name;
		name.Printf ( _T ( "xmlcopyeditor-%s" ), wxGetUserId().c_str() );
		checker = new wxSingleInstanceChecker ( name );
		if ( checker->IsAnotherRunning() )
		{
			MyClient client;
			if ( client.talkToServer ( argc, argv ) )
				return false;
		}
	}

	server = new MyServer;
	server->Create ( IPC_SERVICE );

	myLocale.Init ( lang, wxLOCALE_LOAD_DEFAULT );

	wxArrayString prefixes;
#ifdef __WXGTK__
	prefixes.Add ( _T ( "/usr/share/locale" ) );
	prefixes.Add ( _T ( "/usr/share/locale-langpack" ) );
	prefixes.Add ( _T ( "/usr/local/share/locale" ) );
#endif
	wxString poDir = dataDir + wxFileName::GetPathSeparator() + _T ( "po" )
			+ wxFileName::GetPathSeparator();
	prefixes.Add ( poDir );
	for ( size_t i = 0; i < prefixes.Count(); )
	{
		if ( wxDirExists ( prefixes[i] ) )
			wxLocale::AddCatalogLookupPathPrefix ( prefixes[i++] );
		else
			prefixes.RemoveAt ( i );
	}
	
	wxString catalog = _T ( "messages" );
	getAvailableTranslations ( &prefixes, &catalog );

	if ( !myLocale.AddCatalog ( catalog ) )
	{}

#ifndef __WXMSW__
	{
		wxLogNull noLog;
		myLocale.AddCatalog ( _T ( "coreutils" ) );
	}
#endif

	MyFrame *frame;
	try
	{
		wxImage::AddHandler ( new wxPNGHandler );
		wxSystemOptions::SetOption ( _T ( "msw.remap" ), 0 );

		frame = new MyFrame (
		    _ ( "XML Copy Editor" ),
		    config.get(),
		    myLocale,
		    singleInstanceCheck,
		    lang );
		frame->Show ( true );

		bool rememberOpenTabs = config->Read ( _T ( "rememberOpenTabs" ), true );
		if ( rememberOpenTabs )
			frame->openRememberedTabs();
		else if ( !frame->getHandleCommandLineFlag() )
			frame->newDocument ( wxEmptyString );

		if ( frame->getHandleCommandLineFlag() )
			frame->handleCommandLine();
	}
	catch ( const XMLException &e )
	{
		wxString error;
		error << _ ( "Failed to initialize Xerces-C:\n" )
		    << WrapXerces::toString ( e.getMessage() );
		wxMessageBox ( error, _ ( "Error" ), wxOK | wxICON_ERROR );
		return false;
	}
	catch ( exception &e )
	{
		const char *what;
		what = e.what();
		wxString wideWhat, errorString;
		wideWhat = wxString ( what, wxConvLocal, strlen ( what ) );

		if ( wideWhat.empty() )
			wideWhat = _ ( "(unknown error)" );

		errorString = _ ( "XML Copy Editor has encountered the following error and needs to close: " );
		errorString += wideWhat;
		errorString += _T ( "." );
#ifdef __WXMSW__
		::MessageBox (
		    NULL,
		    errorString,
		    _ ( "Error" ),
		    MB_ICONERROR | MB_TASKMODAL );
#else
		wxMessageBox ( errorString, _ ( "Error" ), wxICON_ERROR );
#endif
		exit ( EXIT_FAILURE );
	}
	catch ( ... )
	{
		exit ( EXIT_FAILURE );
	}
	return true;
}

void MyApp::OnUnhandledException()
{
#ifdef __WXMSW__
	::MessageBox (
	    NULL,
	    _ ( "XML Copy Editor has encountered an error and needs to close." ),
	    _ ( "Error" ),
	    MB_ICONERROR | MB_TASKMODAL );
#else
	wxMessageBox (
	    _ ( "XML Copy Editor has encountered an error and needs to close." ),
	    _ ( "Error" ),
	    wxICON_ERROR );
#endif
	exit ( EXIT_FAILURE );
}

bool MyApp::OnExceptionInMainLoop()
{
	try
	{
		throw;
	}
#ifdef __WXMSW__
	catch ( bad_alloc& )
	{
		::MessageBox (
		    NULL,
		    _ ( "The operating system has turned down a request for additional memory" ),
		    _ ( "Out of memory" ),
		    MB_ICONERROR );
		return true;
	}
#endif
	catch ( exception &e )
	{
		const char *what;
		what = e.what();
		wxString wideWhat, errorString;
		wideWhat = wxString ( what, wxConvLocal, strlen ( what ) );

		if ( wideWhat.empty() )
			_ ( "(unknown error)" );

		errorString = _ ( "The following error has occurred: " );
		errorString += wideWhat;
		errorString += _ ( ".\n\nSelect \"Abort\" to exit, \"Retry\" to close this window and \"Ignore\" to continue." );
#ifdef __WXMSW__
		int ret = ::MessageBox (
		              NULL,
		              errorString,
		              _ ( "Error" ),
		              MB_ABORTRETRYIGNORE |
		              MB_ICONERROR |
		              MB_TASKMODAL );
		switch ( ret )
		{
			case IDABORT:
				exit ( EXIT_FAILURE );
				break;
			case IDRETRY:
				return false;
			case IDIGNORE:
				return true;
			default:
				throw;
		}
#else
		// wxGTK does not reach this point; see HandleEvent below
		wxMessageBox (
		    errorString,
		    _ ( "Error" ),
		    wxICON_ERROR );
		return false;
#endif
	}
	catch ( ... )
	{
		wxString otherError ( _ ( "XML Copy Editor has encountered an error and needs to close." ) );
#ifdef __WXMSW__
		::MessageBox (
		    NULL,
		    otherError,
		    _ ( "Error" ),
		    MB_ICONERROR );
		return false;
#else
		wxMessageBox (
		    otherError,
		    _ ( "Error" ),
		    wxICON_ERROR );
		return false;
#endif
	}
	return false;
}

#ifndef __WXMSW__
void MyApp::HandleEvent ( wxEvtHandler *handler, wxEventFunction func, wxEvent& event ) const
{
	try
	{
		wxApp::HandleEvent ( handler, func, event );
	}
	catch ( std::bad_alloc& )
	{
		wxMessageBox (
		    _ ( "The operating system has turned down a request for additional memory" ),
		    _ ( "Out of memory" ),
		    wxICON_ERROR );
		return;
	}
	catch ( std::exception& e )
	{
		std::string s ( e.what() );
		wxString ws = wxString ( s.c_str(), wxConvUTF8, s.size() );
		wxMessageBox (
		    ws,
		    _ ( "Error" ),
		    wxICON_ERROR );
		return;
	}
	catch ( ... )
	{
		throw;
	}
}
#endif

const std::set<const wxLanguageInfo *> &MyApp::getAvailableTranslations (
    const wxArrayString *catalogLookupPathPrefixes /*= NULL*/,
    const wxString *catalog /*= NULL*/ )
{
	static class Translations // Most of the code was copied from wxTranslations
	{
	public:
		Translations ( const wxArrayString *catalogLookupPathPrefixes,
		    const wxString *catalog )
		{
			if ( catalogLookupPathPrefixes == NULL )
				throw std::invalid_argument ( "catalogLookupPathPrefixes" );
			if ( catalog == NULL )
				throw std::invalid_argument ( "catelog" );

			const wxLanguageInfo *info;
#if wxCHECK_VERSION(2,9,0)
			wxTranslations *t = wxTranslations::Get();
			if ( t != NULL )
			{
				wxArrayString all = t->GetAvailableTranslations ( *catalog );
				wxArrayString::const_iterator trans = all.begin();
				for ( ; trans != all.end(); trans++ )
				{
					info = wxLocale::FindLanguageInfo ( *trans );
					if ( info != NULL )
						translations.insert ( info );
				}
			}
#else
			wxArrayString::const_iterator i = catalogLookupPathPrefixes->begin();
			for ( i = catalogLookupPathPrefixes->begin();
			    i != catalogLookupPathPrefixes->end(); ++i )
			{
				if ( i->empty() )
					continue;

				wxDir dir;
				if ( !dir.Open(*i) )
					continue;

				wxString lang;
				for ( bool ok = dir.GetFirst ( &lang, wxEmptyString, wxDIR_DIRS ); ok;
						ok = dir.GetNext (&lang) ) {
					const wxString langdir = *i + wxFILE_SEP_PATH + lang;
					if ( HasMsgCatalogInDir ( langdir, *catalog ) ) {
#ifdef __WXOSX__
						wxString rest;
						if ( lang.EndsWith(".lproj", &rest) )
						lang = rest;
#endif // __WXOSX__
						info = wxLocale::FindLanguageInfo ( lang );
						if ( info != NULL )
							translations.insert ( info );
					}
				}
			}
#endif
		}
		bool HasMsgCatalogInDir ( const wxString &dir, const wxString &catelog )
		{
			return wxFileName ( dir, catelog, _T ( "mo" ) ).FileExists()
				|| wxFileName ( dir + wxFILE_SEP_PATH + _T ( "LC_MESSAGES" ), catelog, _T ( "mo" ) ).FileExists();
		}
		const std::set<const wxLanguageInfo *> &operator()() { return translations; }
	protected:
		std::set<const wxLanguageInfo *> translations;
	} translations ( catalogLookupPathPrefixes, catalog );

	return translations();
}

MyFrame::MyFrame (
    const wxString& title,
    wxFileConfig *configParameter,
    wxLocale& locale,
    bool singleInstanceCheckParameter,
    int langParameter ) :
		wxFrame ( NULL, wxID_ANY, title ),
		config ( configParameter ),
		myLocale ( locale ),
		singleInstanceCheck ( singleInstanceCheckParameter ),
		lang ( langParameter ),
		htmlPrinting ( new wxHtmlEasyPrinting (
		                   wxEmptyString,
		                   this ) ),
		findDialog ( 0 ),
#ifndef __WXMSW__
		helpController ( new wxHtmlHelpController() ),
#endif
		menuBar ( 0 ),
		toolBar ( 0 ),
		xmlMenu ( 0 ),
		mainBook ( 0 ),
		restoreFocusToNotebook ( false )
{
	manager.SetManagedWindow ( this );

	lastPos = 0;
	htmlReport = NULL;
	lastDoc = NULL;

	wxString defaultFont = wxSystemSettings::GetFont ( wxSYS_ANSI_VAR_FONT ).GetFaceName();

	bool findMatchCase;

	// fetch configuration
	if ( config ) // config found
	{
		history.Load ( *config );
		properties.insertCloseTag =
		    config->Read ( _T ( "insertCloseTag" ), true );
		properties.completion =
		    config->Read ( _T ( "completion" ), true );
		properties.number =
		    config->Read ( _T ( "number" ), true );
		properties.fold =
		    config->Read ( _T ( "fold" ), true );
		properties.currentLine =
		    config->Read ( _T ( "currentLine" ), true );
		properties.highlightSyntax =
		    config->Read ( _T ( "highlightSyntax" ), true );
		properties.whitespaceVisible =
		    config->Read ( _T ( "whitespaceVisible" ), true );
		properties.indentLines =
		    config->Read ( _T ( "indentLines" ), true );
		properties.toggleLineBackground =
		    config->Read ( _T ( "toggleLineBackground" ), true );
		properties.protectHiddenElements =
		    config->Read ( _T ( "protectHiddenElements" ), true );
		properties.deleteWholeTag =
		    config->Read ( _T ( "deleteWholeTag" ), true );
		properties.validateAsYouType =
		    config->Read ( _T ( "validateAsYouType" ), true );
		properties.font =
		    config->Read ( _T ( "font" ), defaultFont );
		findRegex =
		    config->Read ( _T ( "findRegex" ), true );
		xpathExpression =
		    config->Read ( _T ( "xpathExpression" ), wxEmptyString );
		lastXslStylesheet =
		    config->Read ( _T ( "lastXslStylesheet" ), wxEmptyString );
		lastRelaxNGSchema =
		    config->Read ( _T ( "lastRelaxNGSchema" ), wxEmptyString );

		exportQuiet =
		  config->Read ( _T ( "exportQuiet" ), (long)true );
		exportMp3Album =
		  config->Read ( _T ( "exportMp3Album" ), (long)true );
        exportSuppressOptional = 
            config->Read ( _T ( "exportSuppressOptional" ), (long)true );
        exportHtml =
            config->Read ( _T ( "exportHtml" ), (long)true );
        exportEpub =
            config->Read ( _T ( "exportEpub" ), (long)true );
        exportRtf =
            config->Read ( _T ( "exportRtf" ), (long)true );
        exportDoc =
            config->Read ( _T ( "exportDoc" ), (long)true );
        exportFullDaisy =
            config->Read ( _T ( "exportFullDaisy" ), (long)true );

		applicationDir =
		    config->Read ( _T ( "applicationDir" ), wxStandardPaths::Get().GetDataDir() );

		// if default value != true, type as long int
		long valZoom, longFalse;
		longFalse = false;
		valZoom = 0;
		frameWidth = frameHeight = framePosX = framePosY = 0;

		properties.wrap =
		    config->Read ( _T ( "wrap" ), longFalse );

		properties.zoom =
		    config->Read ( _T ( "zoom" ), valZoom );

		properties.colorScheme = config->Read ( _T ( "colorScheme" ), COLOR_SCHEME_DEFAULT );

		globalReplaceAllDocuments =
		    config->Read ( _T ( "globalReplaceAllDocuments" ), longFalse );
		showFullPathOnFrame =
		    config->Read ( _T ( "showFullPathOnFrame" ), longFalse );
		findMatchCase =
		    config->Read ( _T ( "findMatchCase" ), longFalse );

		commandSync = config->Read ( _T ( "commandSync" ), longFalse );
		commandOutput = config->Read ( _T ( "commandOutput" ), ID_COMMAND_OUTPUT_IGNORE );
		commandString = config->Read ( _T ( "commandString" ), wxEmptyString );
		
		exportStylesheet = config->Read ( _T ( "exportStylesheet" ), wxEmptyString );
		exportFolder = config->Read ( _T ( "exportFolder" ), wxEmptyString );

		ruleSetPreset =
		    config->Read ( _T ( "ruleSetPreset" ), _ ( "Default style" ) );
		dictionaryPreset = 
		    config->Read ( _T ( "dictionaryPreset" ), _ ( "en_US" ) );
		
		filterPreset =
		    config->Read ( _T ( "filterPreset" ), _ ( "(No filter)" ) );
		findData.SetFindString ( config->Read ( _T ( "findReplaceFind" ), _T ( "" ) ) );
		findData.SetReplaceString ( config->Read ( _T ( "findReplaceReplace" ), _T ( "" ) ) );

		toolbarVisible =
		    config->Read ( _T ( "toolbarVisible" ), true );
		protectTags = config->Read ( _T ( "protectTags" ), longFalse );
		visibilityState = config->Read ( _T ( "visibilityState" ), ID_SHOW_TAGS );

		framePosX = config->Read ( _T ( "framePosX" ), framePosX );
		framePosY = config->Read ( _T ( "framePosY" ), framePosY );
		frameWidth = config->Read ( _T ( "frameWidth" ), frameWidth );
		frameHeight = config->Read ( _T ( "frameHeight" ), frameHeight );
		rememberOpenTabs = config->Read ( _T ( "rememberOpenTabs" ), true );
		libxmlNetAccess = config->Read ( _T ( "libxmlNetAccess" ), longFalse );
		openTabsOnClose = config->Read ( _T ( "openTabsOnClose" ), _T ( "" ) );
		notebookStyle = config->Read ( _T ( "notebookStyle" ), ID_NOTEBOOK_STYLE_VC8_COLOR );
		saveBom = config->Read ( _T ( "saveBom" ), true );
		unlimitedUndo = config->Read ( _T ( "unlimitedUndo" ), true );
		layout = config->Read ( _T ( "layout" ), wxEmptyString );
		restoreLayout = config->Read ( _T ( "restoreLayout" ), true );
		showLocationPane = config->Read ( _T ( "showLocationPane" ), true );
		showInsertChildPane = config->Read ( _T ( "showInsertChildPane" ), true );
		showInsertSiblingPane = config->Read ( _T ( "showInsertSiblingPane" ), true );
		showInsertEntityPane = config->Read ( _T ( "showInsertEntityPane" ), true );
		expandInternalEntities = config->Read ( _T ( "expandInternalEntities" ), true );

		lastSymbol = config->Read( _T( "lastSymbol" ), _T ( "*" ) );

		xercescSSE2Warning = config->Read ( _T ( "xercescSSE2Warning" ), true );
	}
	else // config not found
	{
		properties.insertCloseTag =
		    properties.completion =
		        properties.currentLine =
		            properties.indentLines =
		                properties.protectHiddenElements =
		                    properties.toggleLineBackground =
		                        properties.deleteWholeTag =
		                            properties.highlightSyntax = true;
		properties.font = defaultFont;
		properties.wrap = properties.whitespaceVisible = false;
		properties.zoom = 0;
		properties.colorScheme = COLOR_SCHEME_DEFAULT;
		applicationDir = wxStandardPaths::Get().GetDataDir();
		ruleSetPreset = _ ( "Default style" );
		dictionaryPreset = _ ( "en_US" );
		filterPreset = _ ( "No filter" );
		xpathExpression = lastXslStylesheet = lastRelaxNGSchema = wxEmptyString;
		findRegex = true;
		findMatchCase = globalReplaceAllDocuments =
		                    showFullPathOnFrame = false;
		toolbarVisible = true;
		protectTags = false;
		visibilityState = SHOW_TAGS;
		framePosX = framePosY = frameWidth = frameHeight = 0;
		rememberOpenTabs = true;
		libxmlNetAccess = false;
		openTabsOnClose = wxEmptyString;
		notebookStyle = ID_NOTEBOOK_STYLE_VC8_COLOR;
		saveBom = unlimitedUndo = true;
		layout = wxEmptyString;
		restoreLayout = true;
		showLocationPane = true;
		showInsertChildPane = true;
		showInsertSiblingPane = true;
		showInsertEntityPane = true;
		expandInternalEntities = true;
		properties.validateAsYouType = true;

		commandSync = false;
		commandOutput = ID_COMMAND_OUTPUT_IGNORE;
		commandString = wxEmptyString;
		
		exportStylesheet = exportFolder = wxEmptyString;
		exportQuiet = exportMp3Album = exportSuppressOptional = exportHtml =
            exportEpub = exportRtf = exportDoc = exportFullDaisy = true;

		lastSymbol = _T( "*" );
		xercescSSE2Warning = true;
	}

	largeFileProperties.completion = false;
	largeFileProperties.fold = false;
	largeFileProperties.whitespaceVisible = false;
	largeFileProperties.wrap = false;
	largeFileProperties.indentLines = false;
	largeFileProperties.protectHiddenElements = false;
	largeFileProperties.toggleLineBackground = false;
	largeFileProperties.insertCloseTag = false;
	largeFileProperties.deleteWholeTag = false;
	largeFileProperties.highlightSyntax = false;
	largeFileProperties.validateAsYouType = false;
	largeFileProperties.number = properties.number;
	largeFileProperties.currentLine = properties.currentLine;
	largeFileProperties.font = properties.font;
	largeFileProperties.zoom = 0;
	largeFileProperties.colorScheme = COLOR_SCHEME_NONE;

	updatePaths();
	loadBitmaps();

	// Initialize libxml
	WrapLibxml::Init ( catalogPath );

	// Initialize Xerces-C++
	WrapXerces::Init ( libxmlNetAccess );

	if ( XMLPlatformUtils::fgSSE2ok
		&& xercescSSE2Warning
		&& wxTheApp->argc == 1 )
	{
		xercescSSE2Warning = wxMessageBox (
			_ ("SSE2 is enabled in Xerces-C++ library. Xerces-C++ didn't "\
			   "use them in a thread-safe way. It may cause program crashes "\
			   "(segmentation faults).\n\n"\
			   "If it happens, please try compiling Xerces-C++ with SSE2 "\
			   "disabled.\n\n"\
			   "OK:\tShow this warning next time\n"\
			   "Cancel:\tDisable the warning\n"),
			_ ("SSE2 problem in Xerces-C++"),
			wxOK | wxCANCEL | wxICON_WARNING
		) == wxOK;
	}

	size_t findFlags = 0;
	findFlags |= wxFR_DOWN;

	if ( findMatchCase )
		findFlags |= wxFR_MATCHCASE;

	findData.SetFlags ( findFlags );

	// initialise document count for tab labels
	documentCount = 1;

	SetIcon ( wxICON ( appicon ) );

	CreateStatusBar();
	wxStatusBar *status = GetStatusBar();
	int widths[] = { -24, -6, -6, -6, -8 };
	status->SetFieldsCount ( 5 );
	status->SetStatusWidths ( 5, widths );

	if ( !frameWidth ||
	        !frameHeight ||
	        frameWidth < 0 ||
	        frameHeight < 0 ||
	        framePosX < 0 ||
	        framePosY < 0 )
	{
#ifdef __WXMSW__
		Maximize();
#else
		SetSize ( 50, 50, 640, 480 );
#endif
	}
	else
	{
		SetSize ( framePosX, framePosY, frameWidth, frameHeight );
	}

	stylePosition = aboutPosition = wxDefaultPosition;
	styleSize = wxSize ( 720, 540 );

	showTopBars ( toolbarVisible );

	long style = wxAUI_NB_TOP |
	             wxAUI_NB_TAB_SPLIT |
	             wxAUI_NB_TAB_MOVE |
	             wxAUI_NB_SCROLL_BUTTONS |
	             wxAUI_NB_WINDOWLIST_BUTTON |
	             wxAUI_NB_CLOSE_ON_ALL_TABS |
	             wxNO_BORDER;

	mainBook = new MyNotebook (
	    this,
	    ID_NOTEBOOK,
	    wxDefaultPosition,
	    wxDefaultSize,
	    style );

	manager.AddPane ( mainBook, wxAuiPaneInfo().CenterPane()
	                  .PaneBorder ( false ).Name ( _T ( "documentPane" ) ) );
	manager.GetPane ( mainBook ).dock_proportion = 10;

	// add insert child panes
	locationPanel = new LocationPanel ( this, ID_LOCATION_PANEL );
	insertChildPanel = new InsertPanel ( this, ID_INSERT_CHILD_PANEL,
	                                     INSERT_PANEL_TYPE_CHILD );
	insertSiblingPanel = new InsertPanel ( this, ID_INSERT_SIBLING_PANEL,
	                                       INSERT_PANEL_TYPE_SIBLING );
	insertEntityPanel = new InsertPanel ( this, ID_INSERT_ENTITY_PANEL,
	                                      INSERT_PANEL_TYPE_ENTITY );

#ifdef __WXMSW__
	manager.AddPane ( ( wxWindow * ) locationPanel, wxRIGHT, _ ( "Current Element" ) );
	manager.AddPane ( ( wxWindow * ) insertChildPanel, wxRIGHT, _ ( "Insert Element" ) );
	manager.AddPane ( ( wxWindow * ) insertSiblingPanel, wxRIGHT, _ ( "Insert Sibling" ) );
	manager.AddPane ( ( wxWindow * ) insertEntityPanel, wxRIGHT, _ ( "Insert Entity" ) );
#else
	manager.AddPane ( ( wxWindow * ) insertEntityPanel, wxRIGHT, _ ( "Insert Entity" ) );
	manager.AddPane ( ( wxWindow * ) insertSiblingPanel, wxRIGHT, _ ( "Insert Sibling" ) );
	manager.AddPane ( ( wxWindow * ) insertChildPanel, wxRIGHT, _ ( "Insert Element" ) );
	manager.AddPane ( ( wxWindow * ) locationPanel, wxRIGHT, _ ( "Current Element" ) );
#endif

	manager.GetPane ( locationPanel ).Name ( _T ( "locationPane" ) ).Show (
	    ( restoreLayout ) ? showLocationPane : true ).DestroyOnClose ( false ).PinButton ( true );
	manager.GetPane ( locationPanel ).dock_proportion = 1;

	manager.GetPane ( insertChildPanel ).Name ( _T ( "insertChildPane" ) ).Show (
	    ( restoreLayout ) ? showInsertChildPane : true ).DestroyOnClose ( false ).PinButton ( true );
	manager.GetPane ( insertChildPanel ).dock_proportion = 1;

	manager.GetPane ( insertSiblingPanel ).Name ( _T ( "insertSiblingPane" ) ).Show (
	    ( restoreLayout ) ? showInsertSiblingPane : true ).DestroyOnClose ( false ).PinButton ( true );
	manager.GetPane ( insertSiblingPanel ).dock_proportion = 1;

	manager.GetPane ( insertEntityPanel ).Name ( _T ( "insertEntityPane" ) ).Show (
	    ( restoreLayout ) ? showInsertEntityPane : true ).DestroyOnClose ( false ).PinButton ( true );
	manager.GetPane ( insertEntityPanel ).dock_proportion = 1;

	// add (hidden) message pane
	htmlReport = new MyHtmlPane (
	    this,
	    ID_VALIDATION_PANE,
	    wxDefaultPosition,
	    wxSize ( -1, 48 ) );
#ifndef __WXMSW__
	const int sizeArray[] =
	{
		8, 9, 10, 11, 12, 13, 14
	};
	htmlReport->SetFonts ( wxEmptyString, wxEmptyString, sizeArray );
#endif
	htmlReport->SetBorders ( 0 );
	manager.AddPane ( htmlReport, wxAuiPaneInfo().Movable().Bottom()
	                  .Hide().Name ( _T ( "messagePane" ) )
	                  .DestroyOnClose ( false ).Layer ( 1 ) );
	manager.GetPane ( htmlReport ).dock_proportion = 1;

#ifdef NEWFINDREPLACE
	findReplacePanel = new FindReplacePanel (
	    this,
	    ID_FIND_REPLACE_PANEL,
	    &findData,
	    true,
	    findRegex );

	manager.AddPane (
	    ( wxWindow * ) findReplacePanel,
	    wxAuiPaneInfo().Bottom().Hide().Caption ( wxEmptyString ).
	    DestroyOnClose ( false ).Layer ( 2 ) );
#endif

	commandPanel = new CommandPanel (
	    this,
	    wxID_ANY,
	    commandString, // tbd
	    commandSync, // tbd
	    commandOutput // tbd
	);
	manager.AddPane (
	    ( wxWindow * ) commandPanel,
	    wxAuiPaneInfo().Bottom().Hide().Caption ( _T ( "Command" ) ).DestroyOnClose ( false ).Layer ( 3 ) );

	validatePaths();

	// handle command line and, on Windows, MS Word integration
	handleCommandLineFlag = ( wxTheApp->argc > 1 ) ? true : false;

#ifdef __WXMSW__
	DragAcceptFiles ( true ); // currently Windows only
#endif

	manager.Update();

	/*
	  defaultLayout = manager.SavePerspective();

	  // restore layout if req'd
	  if (restoreLayout && !layout.empty())
	  {
	    if (!manager.LoadPerspective(layout, true))
	      manager.LoadPerspective(defaultLayout, true);
	  }
	*/
}

MyFrame::~MyFrame()
{
	ThreadReaper::get().clear();

	std::vector<wxString>::iterator it;
	for ( it = tempFileVector.begin(); it != tempFileVector.end(); ++it )
		wxRemoveFile ( *it );

	layout = manager.SavePerspective();
	if ( !config )
		return;
	history.Save ( *config );
	config->Write ( _T ( "insertCloseTag" ), properties.insertCloseTag );
	config->Write ( _T ( "completion" ), properties.completion );
	config->Write ( _T ( "number" ), properties.number );
	config->Write ( _T ( "fold" ), properties.fold );
	config->Write ( _T ( "currentLine" ), properties.currentLine );
	config->Write ( _T ( "whitespaceVisible" ), properties.whitespaceVisible );
	config->Write ( _T ( "wrap" ), properties.wrap );
	config->Write ( _T ( "indentLines" ), properties.indentLines );
	config->Write ( _T ( "zoom" ), properties.zoom );
	config->Write ( _T ( "colorScheme" ), properties.colorScheme );
	config->Write ( _T ( "protectHiddenElements" ), properties.protectHiddenElements );
	config->Write ( _T ( "toggleLineBackground" ), properties.toggleLineBackground );
	config->Write ( _T ( "deleteWholeTag" ), properties.deleteWholeTag );
	config->Write ( _T ( "validateAsYouType" ), properties.validateAsYouType );
	config->Write ( _T ( "font" ), properties.font );
	config->Write ( _T ( "highlightSyntax" ), properties.highlightSyntax );
	config->Write ( _T ( "applicationDir" ), applicationDir );
	config->Write ( _T ( "ruleSetPreset" ), ruleSetPreset );
	config->Write ( _T ( "dictionaryPreset" ), dictionaryPreset );
	config->Write ( _T ( "filterPreset" ), filterPreset );
	config->Write ( _T ( "xpathExpression" ), xpathExpression );
	config->Write ( _T ( "findReplaceFind" ), findData.GetFindString() );
	config->Write ( _T ( "findReplaceReplace" ), findData.GetReplaceString() );
	config->Write ( _T ( "globalReplaceAllDocuments" ), globalReplaceAllDocuments );
	config->Write ( _T ( "showFullPathOnFrame" ), showFullPathOnFrame );
	config->Write ( _T ( "toolbarVisible" ), toolbarVisible );
	config->Write ( _T ( "protectTags" ), protectTags );
	config->Write ( _T ( "visibilityState" ), visibilityState );
 	config->Write ( _T ( "showLocationPane" ), manager.GetPane ( locationPanel ).IsShown() );
	config->Write ( _T ( "showInsertChildPane" ), manager.GetPane ( insertChildPanel ).IsShown() );
	config->Write ( _T ( "showInsertSiblingPane" ), manager.GetPane ( insertSiblingPanel ).IsShown() );
	config->Write ( _T ( "showInsertEntityPane" ), manager.GetPane ( insertEntityPanel ).IsShown() );
	config->Write ( _T ( "expandInternalEntities" ), expandInternalEntities );
	config->Write ( _T ( "findRegex" ), findReplacePanel->getRegex() );
	config->Write ( _T ( "findMatchCase" ), ( findData.GetFlags() ) & wxFR_MATCHCASE );
	config->Write ( _T ( "commandSync" ), commandPanel->getSync() );
	config->Write ( _T ( "commandOutput" ), commandPanel->getOutput() );
	config->Write ( _T ( "commandString" ), commandPanel->getCommand() );
	config->Write ( _T ( "restoreLayout" ), restoreLayout );
	config->Write ( _T ( "lastXslStylesheet" ), lastXslStylesheet );
	config->Write ( _T ( "lastRelaxNGSchema" ), lastRelaxNGSchema );
	config->Write ( _T ( "exportStylesheet" ), exportStylesheet );
	config->Write ( _T ( "exportFolder" ), exportFolder );
	config->Write ( _T ( "exportQuiet" ), exportQuiet );
	config->Write ( _T ( "exportMp3Album" ), exportMp3Album );
	config->Write ( _T ( "exportSuppressOptional" ), exportSuppressOptional );
	config->Write ( _T ( "exportHtml" ), exportHtml );
	config->Write ( _T ( "exportEpub" ), exportEpub );
	config->Write ( _T ( "exportRtf" ), exportRtf );
	config->Write ( _T ( "exportDoc" ), exportDoc );
	config->Write ( _T ( "exportFullDaisy" ), exportFullDaisy );

	GetPosition ( &framePosX, &framePosY );
	config->Write ( _T ( "framePosX" ), framePosX );
	config->Write ( _T ( "framePosY" ), framePosY );
	GetSize ( &frameWidth, &frameHeight );
	config->Write ( _T ( "frameWidth" ), frameWidth );
	config->Write ( _T ( "frameHeight" ), frameHeight );

	config->Write ( _T ( "rememberOpenTabs" ), rememberOpenTabs );
	config->Write ( _T ( "openTabsOnClose" ), openTabsOnClose );
	config->Write ( _T ( "libxmlNetAccess" ), libxmlNetAccess );

	config->Write ( _T ( "singleInstanceCheck" ), singleInstanceCheck );
	config->Write ( _T ( "lang" ), lang );
	config->Write ( _T ( "notebookStyle" ), notebookStyle );
	config->Write ( _T ( "saveBom" ), saveBom );
	config->Write ( _T ( "unlimitedUndo" ), unlimitedUndo );

	config->Write ( _T ( "lastSymbol" ), lastSymbol );
	config->Write ( _T ( "xercescSSE2Warning" ), xercescSSE2Warning );

	manager.UnInit();
	wxTheClipboard->Flush();
}

void MyFrame::showTopBars ( bool b )
{
	if ( !menuBar )
	{
		SetToolBar ( NULL );
		menuBar = getMenuBar();
		SetMenuBar ( menuBar );
	}
	if ( b )
	{
		if ( !toolBar )
			toolBar = getToolBar();
		SetToolBar ( toolBar );
	}
	else
	{
		SetToolBar ( NULL );
		delete toolBar;
		toolBar = NULL;
	}
}

void MyFrame::handleCommandLine()
{
	bool wordFlag, styleFlag;
	wordFlag = styleFlag = false;
	wxChar c;

	int argc = wxTheApp->argc;
	wxChar **argv = wxTheApp->argv;

	while ( ( --argc > 0 && ( *++argv ) [0] == L'-' ) != 0 )
	{
		wxString wideVersion ( ABOUT_VERSION );
		std::string version = ( const char * ) wideVersion.mb_str ( wxConvUTF8 );
		const wxChar *s = argv[0];
		while ( ( c = *++s ) != 0 )
		{
			switch ( c )
			{
				case 'w':
					wordFlag = true;
					break;
				case 's':
					styleFlag = true;
					break;
				case '-':
					if ( *++s == 'v' )
					{
						std::cout << version.c_str() << std::endl;
					}
					else
					{
						std::cout << "Usage: xmlcopyeditor [--version --help -ws] [<file>] [<file2>]" << std::endl
						    << "Options -w (import Microsoft Word document) and -s (open Spelling and style check) are provided for integration with Microsoft Office and only available on Windows" << std::endl;
					}
					exit ( 0 );
				default:
					messagePane ( _ ( "Unknown command line switch (expecting 'w', 's', --version or --help)" ),
					              CONST_STOP );
					return;
			}
		}
	}

	if ( argc <= 0 )
	{
		messagePane ( _ ( "Command line processing incomplete: no file specified" ),
		              CONST_STOP );
		return;
	}

	wxString fileName;

	// no flags specified or not Windows
#ifdef __WXMSW__
	if ( !styleFlag && !wordFlag )
#endif
	{
		for ( ; argc > 0; --argc, ++argv )
		{
			fileName = wxString ( *argv, wxConvLocal );
			fileName = PathResolver::run ( fileName );
			if ( isOpen ( fileName ) )
				continue;
			else if ( !openFile ( fileName ) )
				break;
		}
		return;
	}

	// options only available on Windows
	fileName = wxString ( *argv, wxConvLocal );

	// fetch as many parameters as possible
	for ( ;; )
	{
		if ( --argc <= 0 )
			break;
		++argv;
		ruleSetPreset = wxString ( *argv, wxConvLocal );

		if ( --argc <= 0 )
			break;
		++argv;
		filterPreset = wxString ( *argv, wxConvLocal );

		if ( --argc <= 0 )
			break;
		++argv;
		applicationDir = wxString ( *argv, wxConvLocal );
		updatePaths();

		break;
	}
	if ( wordFlag )
		importMSWord ( fileName );
	else
		openFile ( fileName );

	if ( styleFlag && !ruleSetPreset.empty() && !filterPreset.empty() )
	{
		wxCommandEvent e;
		OnSpelling ( e );
	}
}

bool MyFrame::isOpen ( const wxString& fileName )
{
	return ( openFileSet.find ( fileName ) != openFileSet.end() );
}

bool MyFrame::activateTab ( const wxString& fileName )
{
	int pageCount = mainBook->GetPageCount();
	XmlDoc *currentDoc;
	for ( int i = 0; i < pageCount; ++i )
	{
		currentDoc = ( XmlDoc * ) mainBook->GetPage ( i );
		if ( !currentDoc )
			break;
		if ( currentDoc->getFullFileName() == fileName )
		{
			mainBook->SetSelection ( i );
			return true;
		}
	}
	return false;
}

void MyFrame::OnAbout ( wxCommandEvent& WXUNUSED ( event ) )
{
	wxString description;
	description = ABOUT_DESCRIPTION;
	description.Append ( ABOUT_CONTRIBUTORS );
	description.Append ( _T("\n\nFramework version: ") );
	description.Append ( wxVERSION_STRING );
	description.Append ( _T("\n") );

	wxAboutDialogInfo info;
	info.SetName ( _ ( "XML Copy Editor" ) );
	info.SetWebSite ( _T ( "http://xml-copy-editor.sourceforge.net" ) );
	info.SetVersion ( ABOUT_VERSION );
	info.SetCopyright ( ABOUT_COPYRIGHT );
	info.AddDeveloper ( _T ( "Gerald Schmidt (development) <gnschmidt at users dot sourceforge dot net>" ) );
	info.AddDeveloper ( _T ( "Matt Smigielski (testing) <alectrus at users dot sourceforge dot net>" ) );
	info.AddDeveloper ( _T ( "Justin Dearing (development) <j-pimp at users dot sourceforge dot net>" ) );
	info.AddDeveloper ( _T ( "Kev James (development) <kmjames at users dot sourceforge dot net>" ) );
	info.AddDeveloper ( _T ( "Anh Trinh (development) <ant271 at users dot sourceforge dot net>" ) );
	info.AddDeveloper ( _T ( "Zane U. Ji (development) <zaneuji at users dot sourceforge dot net>" ) );
	info.AddTranslator ( _T ( "Viliam Búr (Slovak) <viliam at bur dot sk>" ) );
	info.AddTranslator ( _T ( "David Håsäther (Swedish) <hasather at gmail dot com>" ) );
	info.AddTranslator ( _T ( "François Badier (French) <frabad at gmail dot com>" ) );
	info.AddTranslator ( _T ( "Thomas Wenzel (German) <thowen at users dot sourceforge.net>" ) );
	info.AddTranslator ( _T ( "SHiNE CsyFeK (Chinese Simplified) <csyfek at gmail dot com>" ) );
	info.AddTranslator ( _T ( "HSU PICHAN, YANG SHUFUN, CHENG PAULIAN, CHUANG KUO-PING, Marcus Bingenheimer (Chinese Traditional)" ) );
	info.AddTranslator ( _T ( "Serhij Dubyk (Ukrainian) <dubyk at library dot lviv dot ua>" ) );
	info.AddTranslator ( _T ( "Antonio Angelo (Italian) <aangelo at users dot sourceforge dot net>" ) );
	info.AddTranslator ( _T ( "Siarhei Kuchuk (Russian) <Cuchuk dot Sergey at gmail dot com>" ) );
	info.AddTranslator ( _T ( "Marcos Pérez González (Spanish) <marcos_pg at yahoo dot com>" ) );
	info.AddTranslator ( _T ( "Rob Elemans (Dutch) <relemans at gmail dot com>" ) );
	info.AddTranslator ( _T ( "Robert Falcó Miramontes <rfalco at acett dot org>" ) );
	info.AddTranslator ( _T ( "Khoem Sokhem (Khmer) <sokhem at open dot org dot kh>" ) );
	info.AddTranslator ( _T ( "Roger Sperberg (Khmer) <rsperberg at gmail dot com>" ) );
	info.SetLicense ( ABOUT_LICENSE );
	info.SetDescription ( description );
	wxAboutBox ( info );
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->SetFocus();
}

void MyFrame::OnCheckWellformedness ( wxCommandEvent& event )
{
    statusProgress ( wxEmptyString );
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	std::string utf8Buffer;
	getRawText ( doc, utf8Buffer );
	if ( utf8Buffer.empty() )
		return;

	doc->clearErrorIndicators();
	statusProgress ( _ ( "Parse in progress..." ) );

	// check for well-formedness
	WrapExpat we ( "UTF-8" );
	if ( !we.parse ( utf8Buffer ) )
	{
		statusProgress ( wxEmptyString );
		messagePane ( we.getLastError(), CONST_WARNING );
		std::pair<int, int> posPair = we.getErrorPosition();
		-- ( posPair.first );
		int cursorPos =
		    doc->PositionFromLine ( posPair.first );
		doc->SetSelection ( cursorPos, cursorPos );

		doc->setErrorIndicator ( posPair.first, posPair.second );
		return;
	}

	statusProgress ( wxEmptyString );
	documentOk ( _ ( "well-formed" ) );
}

void MyFrame::OnPageClosing ( wxAuiNotebookEvent& event ) //wxNotebookEvent& event)//wxFlatNotebookEvent& event)
{
	deletePageVetoed = false;

	if ( insertChildPanel && insertSiblingPanel && locationPanel )
	{
		insertChildPanel->update ( NULL, wxEmptyString );
		insertSiblingPanel->update ( NULL, wxEmptyString );
		locationPanel->update();
		manager.Update();
	}

	XmlDoc *doc;
	doc = ( XmlDoc * ) mainBook->GetPage ( event.GetSelection() );
	if ( !doc )
		return;

	statusProgress ( wxEmptyString );
	closeMessagePane();

	if ( doc->GetModify() ) //CanUndo())
	{
		int selection;
		wxString fileName;
		selection = mainBook->GetSelection();
		if ( selection != -1 )
			fileName = doc->getShortFileName();

		int answer = wxMessageBox (
		                 _ ( "Do you want to save the changes to " ) + fileName + _T ( "?" ),
		                 _ ( "XML Copy Editor" ),
		                 wxYES_NO | wxCANCEL | wxICON_QUESTION,
		                 this );

		if ( answer == wxCANCEL )
		{
			event.Veto();
			deletePageVetoed = true;
			return;
		}
		else if ( answer == wxYES )
		{
			wxCommandEvent event;
			OnSave ( event );
		}
	}
	statusProgress ( wxEmptyString );

	openFileSet.erase ( doc->getFullFileName() );
	event.Skip();
}

void MyFrame::OnClose ( wxCommandEvent& WXUNUSED ( event ) )
{
	closeActiveDocument();
}

void MyFrame::OnCloseAll ( wxCommandEvent& WXUNUSED ( event ) )
{
	if ( !mainBook )
		return;
	openTabsOnClose = wxEmptyString;

	// retain tab order
	if ( rememberOpenTabs && !openFileSet.empty() )
	{
		XmlDoc *doc;
		wxString fullPath;
		size_t maxTabs = mainBook->GetPageCount();
		for ( size_t i = 0; i < maxTabs; ++i )
		{
			doc = ( XmlDoc * ) mainBook->GetPage ( i );
			if ( doc )
			{
				fullPath = doc->getFullFileName();
				if ( !fullPath.empty() )
				{
					openTabsOnClose.Append ( fullPath );
					openTabsOnClose.Append ( _T ( "|" ) );
				}
			}
		}
	}

	while ( closeActiveDocument() )
		;
}

void MyFrame::OnCloseMessagePane ( wxCommandEvent& WXUNUSED ( event ) )
{
	closeMessagePane();
}

void MyFrame::OnCloseFindReplacePane ( wxCommandEvent& WXUNUSED ( event ) )
{
	closeFindReplacePane();
}

void MyFrame::OnCloseCommandPane ( wxCommandEvent& WXUNUSED ( event ) )
{
	closeCommandPane();
}

void MyFrame::closeMessagePane()
{
	if ( !htmlReport )
		return;
	manager.GetPane ( htmlReport ).Hide();
	manager.Update();

	XmlDoc *doc = getActiveDocument();
	if ( doc )
		doc->SetFocus();
}

void MyFrame::closeFindReplacePane()
{
	if ( manager.GetPane ( findReplacePanel ).IsShown() )
		manager.GetPane ( findReplacePanel ).Hide();
	manager.Update();

	XmlDoc *doc = getActiveDocument();
	if ( doc != NULL )
		doc->SetFocus();
}

void MyFrame::closeCommandPane()
{
	if ( manager.GetPane ( commandPanel ).IsShown() )
		manager.GetPane ( commandPanel ).Hide();
	manager.Update();

	XmlDoc *doc = getActiveDocument();
	if ( doc != NULL )
		doc->SetFocus();
}

bool MyFrame::panelHasFocus()
{
	XmlDoc *doc = getActiveDocument();
	return ( !doc || ( FindFocus() != ( wxWindow * ) doc ) );
}

void MyFrame::OnCut ( wxCommandEvent& event )
{
	if ( panelHasFocus() )
	{
		event.Skip();
		return;
	}

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	if ( protectTags )
		doc->adjustSelection();

	doc->Cut();
	doc->setValidationRequired ( true );
}

void MyFrame::OnCopy ( wxCommandEvent& event )
{
	if ( panelHasFocus() )
	{
		event.Skip();
		return;
	}

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->Copy();
}

void MyFrame::OnPaste ( wxCommandEvent& event )
{
	if ( panelHasFocus() )
	{
		event.Skip();
		return;
	}
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	// this has to be handled here to override Scintilla's default Ctrl+V support
	if ( protectTags )
	{
		if ( !wxTheClipboard->Open() || !wxTheClipboard->IsSupported ( wxDF_TEXT ) )
			return;
		wxTextDataObject data;
		wxTheClipboard->GetData ( data );
		wxString buffer = data.GetText();
		wxTheClipboard->Close();
		xmliseWideTextNode ( buffer );
		doc->adjustCursor();
		doc->AddText ( buffer );
	}
	else
		doc->Paste();
}

void MyFrame::OnIdle ( wxIdleEvent& event )
{
	wxStatusBar *status = GetStatusBar();
	if ( !status )
		return;

    /*
    // IPC handling: take one file from fileQueue at a time
    if ( !fileQueue.empty() )
    {
      openFile ( * ( fileQueue.begin() ) );
      fileQueue.erase( fileQueue.begin() );  
    }
    */

	// update attributes hidden field even if no document loaded
	wxString currentHiddenStatus = status->GetStatusText ( STATUS_HIDDEN );
	if ( visibilityState == HIDE_ATTRIBUTES )
	{
		if ( currentHiddenStatus != _ ( "Attributes hidden" ) )
			status->SetStatusText (
			    _ ( "Attributes hidden" ),
			    STATUS_HIDDEN );
	}
	else if ( visibilityState == HIDE_TAGS )
	{
		if ( currentHiddenStatus != _ ( "Tags hidden" ) )
			status->SetStatusText (
			    _ ( "Tags hidden" ),
			    STATUS_HIDDEN );
	}
	else
	{
		if ( !currentHiddenStatus.empty() )
			status->SetStatusText ( wxEmptyString, STATUS_HIDDEN );
	}

	// update protected field even if no document loaded
	wxString currentProtectedStatus = status->GetStatusText ( STATUS_PROTECTED );
	if ( protectTags )
	{
		if ( currentProtectedStatus != _ ( "Tags locked" ) )
			status->SetStatusText (
			    _ ( "Tags locked" ),
			    STATUS_PROTECTED );
	}
	else
	{
		if ( !currentProtectedStatus.empty() )
			status->SetStatusText ( wxEmptyString, STATUS_PROTECTED );
	}

	// check if document loaded
	wxString frameTitle = GetTitle();

	XmlDoc *doc = getActiveDocument();
	if ( doc == NULL )
	{
		if ( lastDoc != NULL )
		{
			lastDoc = NULL;
			status->SetStatusText ( wxEmptyString, STATUS_MODIFIED );
			status->SetStatusText ( wxEmptyString, STATUS_POSITION );
			locationPanel->update ( NULL, wxEmptyString );
			insertChildPanel->update ( NULL, wxEmptyString );
			insertSiblingPanel->update ( NULL, wxEmptyString );
			insertEntityPanel->update ( NULL, wxEmptyString );
			wxString minimal = _ ( "XML Copy Editor" );
			if ( frameTitle != minimal )
				SetTitle ( minimal );

			closeFindReplacePane();

			event.Skip();
			manager.Update();
		}
		return;
	}

	if ( restoreFocusToNotebook )
	{
		doc->SetFocus();
		restoreFocusToNotebook = false;
	}

	// update modified field
	if ( !mainBook )
		return;
	int index = mainBook->GetSelection();

	wxString currentModifiedStatus = status->GetStatusText ( STATUS_MODIFIED );
	wxString currentTabLabel = mainBook->GetPageText ( index );
	if ( doc->GetModify() )
	{
		if ( currentModifiedStatus != _ ( "Modified" ) )
		{
			status->SetStatusText ( _ ( "Modified" ), STATUS_MODIFIED );

			if ( ! ( currentTabLabel.Mid ( 0, 1 ) == _T ( "*" ) ) )
			{
				currentTabLabel.Prepend ( _T ( "*" ) );
				mainBook->SetPageText ( index, currentTabLabel );
			}
		}
	}
	else
	{
		if ( !currentModifiedStatus.empty() )
		{
			status->SetStatusText ( _T ( "" ), STATUS_MODIFIED );

			if ( currentTabLabel.Mid ( 0, 1 ) == _T ( "*" ) )
			{
				currentTabLabel.Remove ( 0, 1 );
				mainBook->SetPageText ( index, currentTabLabel );
			}
		}
	}

	// update coordinates field
	int current = doc->GetCurrentPos();
	if ( current != lastPos )
	{
		wxString coordinates;
		coordinates.Printf (
		    _ ( "Ln %i Col %i" ),
			doc->LineFromPosition ( current ) + 1,
			doc->GetColumn ( current ) + 1 );
		GetStatusBar()->SetStatusText ( coordinates, STATUS_POSITION );
	}

	// update parent element field
	wxString parent, grandparent;
	if ( current == lastPos && doc == lastDoc )
		return;

	lastPos = current;
	lastDoc = doc;

	wxString docTitle;
	if ( doc->getFullFileName().empty() || !showFullPathOnFrame )
		docTitle = doc->getShortFileName();
	else
		docTitle = doc->getFullFileName();

	docTitle += _T ( " - " );
	docTitle += _ ( "XML Copy Editor" );

	if ( frameTitle != docTitle )
		SetTitle ( docTitle );

	// don't try to find parent if pane is not shown
	if ( !manager.GetPane ( insertChildPanel ).IsShown() && !properties.validateAsYouType )
		return;

	int parentCloseAngleBracket = -1;
	if ( !doc->canInsertAt ( current ) )
		parent = grandparent = wxEmptyString;
	else
	{
		parentCloseAngleBracket = doc->getParentCloseAngleBracket ( current );
		parent = doc->getLastElementName ( parentCloseAngleBracket );
	}

	if ( !parent.empty() && properties.validateAsYouType && doc->getValidationRequired() )
	{
		// tbd: limit to parent element
		doc->backgroundValidate();
	}


	if ( parent == lastParent )
		return;
	lastParent = parent;

	bool mustUpdate = false;
	if ( locationPanel && insertChildPanel && insertEntityPanel )
	{
		locationPanel->update ( doc, parent );
		insertChildPanel->update ( doc, parent );
		insertEntityPanel->update ( doc );
		mustUpdate = true;
	}

	if ( parent.empty() )
	{
		if ( insertSiblingPanel )
			insertSiblingPanel->update ( doc, wxEmptyString );
		if ( mustUpdate )
			manager.Update();
		return;
	}

	if ( !manager.GetPane ( insertSiblingPanel ).IsShown() )
	{
		if ( mustUpdate )
			manager.Update();
		return;
	}

	// try to fetch grandparent if necessary/possible
	if ( !parent.empty() && parentCloseAngleBracket != -1 )
	{
		int grandParentCloseAngleBracket;
		grandParentCloseAngleBracket =
		    doc->getParentCloseAngleBracket (
		        doc->getTagStartPos ( parentCloseAngleBracket ) );
		grandparent = doc->getLastElementName ( grandParentCloseAngleBracket );

		if ( insertSiblingPanel )
			insertSiblingPanel->update ( doc, parent, grandparent );
		if ( grandparent != lastGrandparent )
		{
			mustUpdate = true;
			lastGrandparent = grandparent;
		}

	}
	if ( mustUpdate )
		manager.Update();
}

void MyFrame::OnInsertChild ( wxCommandEvent& event )
{
	if ( !insertChildPanel )
		return;

	wxAuiPaneInfo info = manager.GetPane ( insertChildPanel );
	if ( !info.IsOk() )
	{
		return;
	}
	if ( !info.IsShown() )
	{
		manager.GetPane ( insertChildPanel ).Show ( true );
		manager.Update();
	}
	insertChildPanel->setEditFocus();
}

void MyFrame::OnInsertSibling ( wxCommandEvent& event )
{
	if ( !insertSiblingPanel )
		return;

	wxAuiPaneInfo info = manager.GetPane ( insertSiblingPanel );
	if ( !info.IsOk() )
	{
		return;
	}

	if ( !info.IsShown() )
	{
		manager.GetPane ( insertSiblingPanel ).Show ( true );
		manager.Update();
	}
	insertSiblingPanel->setEditFocus();
}

void MyFrame::OnInsertEntity ( wxCommandEvent& event )
{
	if ( !insertEntityPanel )
		return;

	wxAuiPaneInfo info = manager.GetPane ( insertEntityPanel );
	if ( !info.IsOk() )
	{
		return;
	}

	if ( !info.IsShown() )
	{
		manager.GetPane ( insertEntityPanel ).Show ( true );
		manager.Update();
	}
	insertEntityPanel->setEditFocus();
}

void MyFrame::OnInsertSymbol ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxSymbolPickerDialog dlg ( lastSymbol, wxEmptyString, properties.font, this );
	if ( dlg.ShowModal() == wxID_OK )
	{
		if ( dlg.HasSelection() )
		{
			lastSymbol = dlg.GetSymbol();
			doc->AddText ( lastSymbol );
		}
	}
}

void MyFrame::OnInsertTwin ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxString parent = doc->getParent();
	if ( !doc->insertSibling ( parent, parent ) )
	{
		wxString msg;
		msg.Printf ( _T ( "Cannot insert twin '%s'" ), parent.c_str() );
		messagePane ( msg, CONST_STOP );
	}
	doc->setValidationRequired ( true );
	doc->SetFocus();
}

void MyFrame::OnPasteNewDocument ( wxCommandEvent& event )
{
	if ( !wxTheClipboard->Open() )
	{
		messagePane ( _ ( "Cannot open clipboard" ), CONST_STOP );
		return;
	}
	if ( !wxTheClipboard->IsSupported ( wxDF_TEXT ) )
	{
		messagePane ( _ ( "Cannot paste as new document: no text on clipboard" ), CONST_STOP );
		return;
	}
	wxTextDataObject data;
	wxTheClipboard->GetData ( data );

	wxString buffer = data.GetText();
	xmliseWideTextNode ( buffer );

	buffer.Prepend ( _T ( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>" ) );
	buffer.Append ( _T ( "</root>\n" ) );

	newDocument ( buffer );
	wxTheClipboard->Close();
}

void MyFrame::OnDialogFind ( wxFindDialogEvent& event )
{
	findAgain ( event.GetFindString(), event.GetFlags() );
}

void MyFrame::OnDialogReplace ( wxFindDialogEvent& event )
{
	statusProgress ( wxEmptyString );
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	int start = doc->GetTargetStart();
	int end = doc->GetTargetEnd();
	if ( start != end )
	{
		if ( findReplacePanel->getRegex() )
		{
			start += doc->ReplaceTargetRE ( event.GetReplaceString() );
		}
		else
		{
			start += doc->ReplaceTarget ( event.GetReplaceString() );
		}
		// Move to the next position
		doc->SetSelection ( start, start );
	}
	OnDialogFind ( event );
}

void MyFrame::OnDialogReplaceAll ( wxFindDialogEvent& event )
{
	statusProgress ( wxEmptyString );
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	int flags = 0;
	if ( event.GetFlags() & wxFR_WHOLEWORD )
		flags |= wxSTC_FIND_WHOLEWORD;
	if ( event.GetFlags() & wxFR_MATCHCASE )
		flags |= wxSTC_FIND_MATCHCASE;
	if ( findReplacePanel->getRegex() )
		flags |= wxSTC_FIND_REGEXP;

	doc->SetTargetStart ( 0 );
	doc->SetTargetEnd ( doc->GetLength() );
	doc->SetSearchFlags ( flags );

	doc->BeginUndoAction();

	int newLocation, replacementCount, regexWidth;
	newLocation = replacementCount = regexWidth = 0;

	while ( ( newLocation = doc->SearchInTarget ( event.GetFindString() ) ) != -1 )
	{
		if ( findReplacePanel->getRegex() )
		{
			regexWidth = doc->ReplaceTargetRE ( event.GetReplaceString() );
			doc->SetTargetStart ( newLocation + regexWidth );
		}
		else
		{
			doc->ReplaceTarget ( event.GetReplaceString() );
			doc->SetTargetStart ( newLocation + event.GetReplaceString().size() );
		}
		doc->SetTargetEnd ( doc->GetLength() );
		++replacementCount;
	}

	doc->EndUndoAction();

	wxString msg;
	msg.Printf (
	    wxPLURAL ( "%i replacement made", "%i replacements made", replacementCount ),
	    replacementCount );
	statusProgress ( msg );
}

void MyFrame::OnPrintSetup ( wxCommandEvent &WXUNUSED ( event ) )
{
	if ( !htmlPrinting.get() )
		return;
	htmlPrinting->PageSetup();
}

void MyFrame::OnPrintPreview ( wxCommandEvent &WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( !htmlPrinting.get() || ( doc = getActiveDocument() ) == NULL )
		return;
	wxString shortFileName, header;
	shortFileName = doc->getShortFileName();
	if ( !shortFileName.empty() )
		header = shortFileName + _T ( " " );
	header += _T ( "(@PAGENUM@/@PAGESCNT@)<hr>" );

	htmlPrinting->SetHeader (
	    header,
	    wxPAGE_ALL );
	statusProgress ( _ ( "Preparing Print Preview..." ) );
	wxString htmlBuffer = getHtmlBuffer();
	statusProgress ( wxEmptyString );
	if ( ! ( htmlPrinting->PreviewText ( htmlBuffer ) ) )
	{}
}

void MyFrame::OnPrint ( wxCommandEvent &WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( !htmlPrinting.get() || ( doc = getActiveDocument() ) == NULL )
		return;
	wxString shortFileName, header;
	shortFileName = doc->getShortFileName();
	if ( !shortFileName.empty() )
		header = shortFileName + _T ( " " );
	header += _T ( "(@PAGENUM@/@PAGESCNT@)<hr>" );

	htmlPrinting->SetHeader (
	    header,
	    wxPAGE_ALL );
	statusProgress ( _ ( "Preparing to print..." ) );
	wxString htmlBuffer = getHtmlBuffer();
	statusProgress ( wxEmptyString );
	if ( ! ( htmlPrinting->PrintText ( htmlBuffer ) ) )
	{}
}

wxString MyFrame::getHtmlBuffer()
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return _T ( "<html><head></head><body></body></html>" );
	wxString buffer, htmlBuffer;
	buffer = doc->GetText();
	size_t size = buffer.size();
	htmlBuffer.reserve ( size * 2 );

	htmlBuffer = _T ( "<html><body><p>" );
	bool startOfLine = true;
	for ( size_t i = 0; i < size; ++i )
	{
		wchar_t c = buffer[i];
		switch ( c )
		{
			case L' ':
				htmlBuffer += ( startOfLine ) ? _T ( "&nbsp;" ) : _T ( " " );
				break;
			case L'\t':
				htmlBuffer += _T ( "&nbsp;&nbsp;" );
				break;
			case L'<':
				htmlBuffer += _T ( "&lt;" );
				startOfLine = false;
				break;
			case L'>':
				htmlBuffer += _T ( "&gt;" );
				startOfLine = false;
				break;
			case L'\n':
				htmlBuffer += _T ( "<br>" );
				startOfLine = true;
				break;
			case L'&':
				htmlBuffer + _T ( "&amp;" );
				startOfLine = false;
				break;
			default:
				htmlBuffer += c;
				startOfLine = false;
				break;
		}
	}
	htmlBuffer += _T ( "</p></body></html>" );
	return htmlBuffer;
}

void MyFrame::OnFind ( wxCommandEvent& WXUNUSED ( event ) )
{
#ifdef NEWFINDREPLACE
	manager.GetPane ( findReplacePanel ).Caption ( _ ( "Find" ) );
	bool visible = manager.GetPane ( findReplacePanel ).IsShown();
	if ( !visible )
	{
		manager.GetPane ( findReplacePanel ).Show();
	}
	manager.Update();
	findReplacePanel->refresh();
	findReplacePanel->setReplaceVisible ( false );
	findReplacePanel->focusOnFind();
	return;
#endif

	if ( findDialog.get() )
	{
		findDialog = std::auto_ptr<wxFindReplaceDialog> ( 0 );
	}
	findDialog = ( std::auto_ptr<wxFindReplaceDialog> ( new wxFindReplaceDialog (
	                   this,
	                   &findData,
	                   _ ( "Find" ) ) ) );
	findDialog->Show();
}

void MyFrame::OnImportMSWord ( wxCommandEvent& event )
{
#ifndef __WXMSW__
	messagePane ( _ ( "This functionality requires Microsoft Windows" ) );
	return;
#endif

	std::auto_ptr<wxFileDialog> fd ( new wxFileDialog (
	                                     this,
	                                     _ ( "Import Microsoft Word Document" ),
	                                     mLastDir,
	                                     _T ( "" ),
	                                     _T ( "Microsoft Word (*.doc)|*.doc" ),
#if wxCHECK_VERSION(2,9,0)
	                                     wxFD_OPEN | wxFD_FILE_MUST_EXIST
#else
	                                     wxOPEN | wxFILE_MUST_EXIST
#endif
	                                     ) );
	if ( fd->ShowModal() == wxID_CANCEL )
		return;

	mLastDir = fd->GetDirectory();

	wxString path = fd->GetPath();

	if ( path == _T ( "" ) )
		return;

	importMSWord ( path );
}

void MyFrame::OnExport ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );
	closeMessagePane();

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

    wxString testDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "daisy" );
    bool downloadLink = !wxDirExists ( testDir );

    std::auto_ptr<ExportDialog> ed ( new ExportDialog (
        this,
        exportStylesheet,
        exportFolder,
        exportQuiet,
        exportSuppressOptional,
        exportHtml,
        exportEpub,
        exportRtf,
        exportDoc,
        exportFullDaisy,
        exportMp3Album,
        downloadLink ) );
    int ret = ed->ShowModal();
    
    if ( ret != wxID_OK )
        return;

    exportStylesheet = ed->getUrlString();
    exportFolder = ed->getFolderString();
    exportQuiet = ed->getQuiet();
    exportMp3Album = ed->getMp3Album();
    exportSuppressOptional = ed->getSuppressOptional();
    exportHtml = ed->getHtml();
    exportEpub = ed->getEpub();
    exportRtf = ed->getRtf();
    exportDoc = ed->getDoc();
    exportFullDaisy = ed->getFullDaisy();

	std::string rawBufferUtf8;
	getRawText ( doc, rawBufferUtf8 );
	if ( !XmlEncodingHandler::setUtf8 ( rawBufferUtf8 ) )
	{
		encodingMessage();
		return;
	}

	WrapTempFileName tempFileName ( doc->getFullFileName() );

	ofstream rawBufferStream ( tempFileName.name().c_str() );
	if ( !rawBufferStream )
		return;
	rawBufferStream << rawBufferUtf8;
	rawBufferStream.close();

    wxString tempFile= tempFileName.wideName();
    
    WrapDaisy wd ( this, daisyDir, doc->getFullFileName() );
    if ( !wd.run (
        tempFile,
        exportStylesheet,
        exportFolder,
        exportQuiet,
        exportSuppressOptional,
        exportEpub,
        exportRtf,
        exportDoc,
        exportFullDaisy,
        exportMp3Album ) )
    {
        messagePane ( _ ("[b]DAISY export stopped[/b]: ") + wd.getLastError(), CONST_STOP );
        return;
    }
	messagePane ( _ ( "DAISY export completed. Output files are stored in: [b]" ) + exportFolder + _T ( "[/b]." ), CONST_INFO );
}

void MyFrame::importMSWord ( const wxString& path )
{
#ifndef __WXMSW__
	messagePane ( _ ( "This functionality requires Microsoft Windows" ) );
	return;
#endif

	WrapTempFileName tempFileName ( _T ( "" ) ), swapFileName ( _T ( "" ) );
	wxString completeSwapFileName = swapFileName.wideName() + _T ( ".doc" );
	if ( !wxCopyFile ( path, completeSwapFileName, true ) )
	{
		wxString message;
		message.Printf ( _ ( "Cannot open [b]%s[/b] for import" ), path.c_str() );
		messagePane ( message, CONST_STOP );
		return;
	}

	wxString cmd = binDir +
	               _T ( "doc2xml.exe \"" ) +
	               completeSwapFileName + _T ( "\" \"" ) +
	               tempFileName.wideName() + _T ( "\"" );

	statusProgress ( _ ( "Import in progress..." ) );
	int result = wxExecute ( cmd, wxEXEC_SYNC );

	wxRemoveFile ( completeSwapFileName ); // necessary because .doc extension added

	statusProgress ( wxEmptyString );
	wxString message;
	wxString versionMessage (
	    _ ( "(lossless conversion requires version 2003 or later)" ) );

	switch ( result )
	{
		case 0:
			break;
		case 1:
			messagePane ( _ ( "Cannot start Microsoft Word" ), CONST_STOP );
			return;
		case 2:
			messagePane (
			    _ ( "A more recent version of Microsoft Word is required" ), CONST_STOP );
			return;
		case 3:
			message.Printf ( _T ( "Microsoft Word cannot open [b]%s[/b]" ), path.c_str() );
			messagePane ( message + path, CONST_STOP );
			return;
		case 4:
			message.Printf ( _ ( "Microsoft Word cannot save [b]%s[/b] as XML" ), path.c_str() );
			messagePane ( message, CONST_STOP );
			return;
		case 5:
			messagePane (
			    _ ( "Microsoft Word cannot save this document as WordprocessingML " ) +
			    versionMessage,
			    CONST_INFO );
			break;
		default:
			break;
	}

	statusProgress ( _ ( "Opening imported file..." ) );
	std::string buffer;

	wxString displayBuffer;

	if ( result != 5 ) // Word 2003 or later
	{
		std::auto_ptr<WrapLibxml> prettyPrinter ( new WrapLibxml ( libxmlNetAccess ) );
		prettyPrinter->parse ( tempFileName.wideName(), true );
		buffer = prettyPrinter->getOutput();
		displayBuffer = wxString ( buffer.c_str(), wxConvUTF8, buffer.size() );
	}
	else // earlier versions
	{
		if ( !ReadFile::run ( tempFileName.name(), buffer ) )
		{
			statusProgress ( wxEmptyString );
			messagePane ( _ ( "Cannot open imported file" ), CONST_STOP );
			return;
		}
		displayBuffer = wxString ( buffer.c_str(), wxConvUTF8, buffer.size() );
		displayBuffer.Remove ( 0, 1 ); // remove byte order mark
		xmliseWideTextNode ( displayBuffer );

		displayBuffer.Prepend (
		    _T ( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>" ) );
		displayBuffer.Append ( _T ( "</root>\n" ) );
	}

	newDocument ( displayBuffer, tempFileName.wideName() );
	statusProgress ( wxEmptyString );
}

void MyFrame::OnExportMSWord ( wxCommandEvent& event )
{
#ifndef __WXMSW__
	messagePane ( _ ( "This functionality requires Microsoft Windows" ) );
	return;
#endif
	statusProgress ( wxEmptyString );

	// fetch document contents
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	WrapTempFileName wtfn ( _T ( "" ) );
	wxString sourceFileName = doc->getFullFileName();

	if ( sourceFileName.empty() )
	{
		sourceFileName = wtfn.wideName();
		std::fstream ofs ( wtfn.name().c_str() );
		if ( !ofs )
			return;

		std::string utf8Buffer;
		getRawText ( doc, utf8Buffer );
		ofs << utf8Buffer;
		ofs.close();
	}
	else if ( doc->GetModify() ) //CanUndo())
	{
		modifiedMessage();
		return;
	}

	std::auto_ptr<wxFileDialog> fd ( new wxFileDialog (
	                                     this,
	                                     _ ( "Export Microsoft Word Document" ),
	                                     _T ( "" ),
	                                     _T ( "" ),
	                                     _T ( "Microsoft Word (*.doc)|*.doc" ),
#if wxCHECK_VERSION(2,9,0)
	                                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT));
#else
	                                     wxSAVE | wxOVERWRITE_PROMPT ) );
#endif
	fd->ShowModal();

	wxString path = fd->GetPath();

	if ( path == _T ( "" ) )
		return;

	wxString cmd = binDir +
	               _T ( "xml2doc.exe -v \"" ) +
	               sourceFileName + _T ( "\" \"" ) +
	               path + _T ( "\"" );

	statusProgress ( _ ( "Export in progress..." ) );
	int result = wxExecute ( cmd, wxEXEC_SYNC );
	statusProgress ( wxEmptyString );
	wxString message;
	switch ( result )
	{
		case 1:
			messagePane ( _ ( "Cannot start Microsoft Word" ), CONST_STOP );
			return;
		case 2:
			messagePane (
			    _ ( "A more recent version of Microsoft Word is required" ), CONST_STOP );
			return;
		case 3:
			message.Printf ( _ ( "Microsoft Word cannot save %s" ), path.c_str() );
			messagePane ( message, CONST_STOP );
			return;
		case 0:
			break;
		default:
			break;
	}
}

void MyFrame::OnBrowser ( wxCommandEvent& WXUNUSED ( event ) )
{
	statusProgress ( wxEmptyString );

	// fetch document contents
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxString sourceFileName = doc->getFullFileName();
	WrapTempFileName wtfn ( sourceFileName, _T ( ".html" ) );

	if ( sourceFileName.empty() || doc->GetModify() )
	{
		sourceFileName = wtfn.wideName();

		std::ofstream ofs ( ( const char * ) wtfn.name().c_str() );
		if ( !ofs )
		{
			messagePane ( _ ( "Cannot save temporary file" ), CONST_STOP );
			return;
		}

		std::string utf8Buffer;
		getRawText ( doc, utf8Buffer );
		ofs << utf8Buffer;
		ofs.close();

		// keep files until application closes
		tempFileVector.push_back ( sourceFileName );
		tempFileVector.push_back ( wtfn.originalWideName() );
		wtfn.setKeepFiles ( true );
	}

	wxLaunchDefaultBrowser ( sourceFileName, wxBROWSER_NEW_WINDOW );
}

void MyFrame::OnHelp ( wxCommandEvent& event )
{
#ifdef __WXMSW__
	wxString cmd = _T ( "hh.exe \"" ) + helpDir + _T ( "xmlcopyeditor.chm\"" );
	wxExecute ( cmd );
#else
	wxString helpFileName = helpDir + _T ( "xmlcopyeditor.hhp" );
	helpController->AddBook ( wxFileName ( helpFileName ) );
	helpController->DisplayContents();
#endif
}

void MyFrame::OnSplitTab ( wxCommandEvent& event )
{
/*
	int id = event.GetId();
	XmlDoc *doc = getActiveDocument();
	if ( !doc )
		return;
	wxString fileName = doc->getFullFileName();

	// mainBook->GetSelection() is currently unreliable, so fetch by title

	    int pageCount = mainBook->GetPageCount();
	    XmlDoc *currentDoc;
	    int currentSelection = -1;
	    for ( int i = 0; i < pageCount; ++i )
	    {
	        currentDoc = ( XmlDoc * ) mainBook->GetPage ( i );
	        if ( !currentDoc )
	            break;
	        if ( currentDoc->getFullFileName() == fileName )
	        {
	            currentSelection = i;
	        }
	    }
	    if ( currentSelection == -1 )
	        return;
	*/
	int currentSelection, direction;
	currentSelection = mainBook->GetSelection();
	direction = wxAUI_NB_RIGHT;
/*
	switch ( id )
	{
			    ID_SPLIT_TAB_TOP:
			        direction = wxAUI_NB_TOP;
			        break;
			    ID_SPLIT_TAB_RIGHT:
			        direction = wxAUI_NB_RIGHT;
			        break;
			    ID_SPLIT_TAB_BOTTOM:
			        direction = wxAUI_NB_BOTTOM;
			        break;
			    ID_SPLIT_TAB_LEFT:
			        direction = wxAUI_NB_LEFT;
			        break;
		default:
			direction = wxAUI_NB_RIGHT;
			break;
	}
*/
	mainBook->Split ( currentSelection, direction );
}

void MyFrame::OnColorScheme ( wxCommandEvent& event )
{
	int id = event.GetId();
	switch ( id )
	{
		case ID_COLOR_SCHEME_DEFAULT:
			properties.colorScheme = COLOR_SCHEME_DEFAULT;
			break;
		case ID_COLOR_SCHEME_DEFAULT_BACKGROUND:
			properties.colorScheme = COLOR_SCHEME_DEFAULT_BACKGROUND;
			break;
		case ID_COLOR_SCHEME_REDUCED_GLARE:
			properties.colorScheme = COLOR_SCHEME_REDUCED_GLARE;
			break;
		case ID_COLOR_SCHEME_NONE:
			properties.colorScheme = COLOR_SCHEME_NONE;
			break;
		default:
			return;
	}
	colorSchemeMenu->Check ( id, true );

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	properties.zoom = doc->GetZoom(); // ensure temp changes to font size are kept

	applyEditorProperties ( false );
	doc->SetFocus();
}

void MyFrame::OnFontSmaller ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->ZoomOut();
	properties.zoom = doc->GetZoom();
	applyEditorProperties ( true );
	doc->SetFocus();
}

void MyFrame::OnFontMedium ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->SetZoom ( 0 );
	properties.zoom = doc->GetZoom();
	applyEditorProperties ( true );
	doc->SetFocus();
}

void MyFrame::OnFontLarger ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->ZoomIn();
	properties.zoom = doc->GetZoom();
	applyEditorProperties ( true );
	doc->SetFocus();
}

void MyFrame::OnOptions ( wxCommandEvent& WXUNUSED ( event ) )
{
	// ensure font size does not change after
	XmlDoc *doc = getActiveDocument();
	if ( doc )
	{
		properties.zoom = doc->GetZoom();
	}

	wxString title
#ifdef __WXMSW__
	( _ ( "Options" ) );
#else
	( _ ( "Preferences" ) );
#endif
	std::auto_ptr<MyPropertySheet> mpsd ( new MyPropertySheet (
	                                          this,
	                                          properties,
	                                          applicationDir,
	                                          rememberOpenTabs,
	                                          libxmlNetAccess,
	                                          singleInstanceCheck,
	                                          saveBom,
	                                          unlimitedUndo,
	                                          restoreLayout,
	                                          expandInternalEntities,
	                                          showFullPathOnFrame,
	                                          lang,
	                                          wxGetApp().getAvailableTranslations(),
	                                          wxID_ANY,
	                                          title ) );
	if ( mpsd->ShowModal() == wxID_OK )
	{
		WrapXerces::enableNetwork ( libxmlNetAccess );
		applyEditorProperties();
		updatePaths();
	}
	if ( doc )
		doc->SetFocus();
}

void MyFrame::OnHistoryFile ( wxCommandEvent& event )
{
	wxString f ( history.GetHistoryFile ( event.GetId() - wxID_FILE1 ) );
	if ( !f.empty() )
		openFile ( f );
}

void MyFrame::OnGoto ( wxCommandEvent& WXUNUSED ( event ) )
{
	statusProgress ( wxEmptyString );

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxTextEntryDialog *dlg = new wxTextEntryDialog (
	    this,
	    _ ( "Enter line number:" ),
	    _ ( "Go To" ) );
	int ret = dlg->ShowModal();
	if ( ret == wxID_CANCEL )
		return;
	wxString val = dlg->GetValue();
	long line;
	if ( !val.ToLong ( &line ) || line < 1 )
	{
		wxString msg;
		msg.Printf ( _ ( "'%s' is not a valid line number" ), val.c_str() );
		messagePane ( msg, CONST_STOP );
		return;
	}
	--line;
	doc->GotoLine ( ( int ) line );
	doc->SetFocus();
}

void MyFrame::OnFindAgain ( wxCommandEvent& event )
{
	//findAgain(findData.GetFindString(), findData.GetFlags());
	findReplacePanel->OnFindNext ( event );
}

void MyFrame::OnCommand ( wxCommandEvent& WXUNUSED ( event ) )
{
	bool visible = manager.GetPane ( commandPanel ).IsShown();
	if ( !visible )
	{
		manager.GetPane ( commandPanel ).Show();
	}
	manager.Update();
	commandPanel->focusOnCommand();
}

void MyFrame::OnFindReplace ( wxCommandEvent& WXUNUSED ( event ) )
{
#ifdef NEWFINDREPLACE
	manager.GetPane ( findReplacePanel ).Caption ( _ ( "Replace" ) );
	bool visible = manager.GetPane ( findReplacePanel ).IsShown();
	if ( !visible )
	{
		manager.GetPane ( findReplacePanel ).Show();
	}
	manager.Update();
	findReplacePanel->refresh();
	findReplacePanel->setReplaceVisible ( true );
	findReplacePanel->focusOnFind();
	return;
#endif


	if ( findDialog.get() )
	{
		findDialog = std::auto_ptr<wxFindReplaceDialog> ( 0 );
	}
	findDialog = std::auto_ptr<wxFindReplaceDialog> ( new wxFindReplaceDialog (
	                 this,
	                 &findData,
	                 _ ( "Find and Replace" ),
	                 wxFR_REPLACEDIALOG ) );
	findDialog->Show();
}

void MyFrame::OnGlobalReplace ( wxCommandEvent& event )
{
	if ( getActiveDocument() == NULL )
		return;

	size_t flags = findData.GetFlags();
	std::auto_ptr<GlobalReplaceDialog> grd ( new GlobalReplaceDialog (
	            this,
	            findData.GetFindString(),
	            findData.GetReplaceString(),
	            flags & wxFR_MATCHCASE,
	            globalReplaceAllDocuments,
	            findRegex ) );
	int res = grd->ShowModal();

	flags = 0;
	flags |= wxFR_DOWN;
	if ( grd->getMatchCase() )
		flags |= wxFR_MATCHCASE;
	findRegex = grd->getRegex();
	globalReplaceAllDocuments = grd->getAllDocuments();

	findData.SetFindString ( grd->getFindString() );
	findData.SetReplaceString ( grd->getReplaceString() );
	findData.SetFlags ( flags );
	findReplacePanel->setRegex ( findRegex );
	findReplacePanel->setMatchCase ( flags & wxFR_MATCHCASE );
	findReplacePanel->refresh();

	if ( res != wxID_OK )
	{
		return;
	}

	int globalMatchCount, pageCount;
	globalMatchCount = 0;
	pageCount = mainBook->GetPageCount();
	XmlDoc *currentDoc = getActiveDocument();
	if ( !currentDoc )
		return;

	for ( int i = 0; i < pageCount; ++i )
	{
		std::string bufferUtf8;
		if ( !globalReplaceAllDocuments )
		{
			getRawText ( currentDoc, bufferUtf8 );
		}
		else
		{
			currentDoc = ( XmlDoc * ) mainBook->GetPage ( i );
			if ( !currentDoc )
				return;
			getRawText ( currentDoc, bufferUtf8 );
		}

		size_t flags = findData.GetFlags();
		if ( !findRegex )
		{
			std::string findUtf8, replaceUtf8;
			findUtf8 =findData.GetFindString().mb_str ( wxConvUTF8 );
			replaceUtf8 = findData.GetReplaceString().mb_str ( wxConvUTF8 );
			globalMatchCount += Replace::run (
			                        bufferUtf8,
			                        findUtf8,
			                        replaceUtf8,
			                        flags & wxFR_MATCHCASE );
			currentDoc->SetTextRaw ( bufferUtf8.c_str() );
			currentDoc->setValidationRequired ( true );
		}
		else
		{
			try
			{
				std::auto_ptr<WrapRegex> wr ( new WrapRegex (
				                                  ( const char * ) findData.GetFindString().mb_str ( wxConvUTF8 ),
				                                  flags & wxFR_MATCHCASE,
				                                  ( const char * ) findData.GetReplaceString().mb_str ( wxConvUTF8 ) ) );

				int matchCount;
				std::string outputBuffer = wr->replaceGlobal ( bufferUtf8, &matchCount );
				globalMatchCount += matchCount;
				currentDoc->SetTextRaw ( outputBuffer.c_str() );
				currentDoc->setValidationRequired ( true );
			}
			catch ( std::exception& e )
			{
				wxString wideError = wxString ( e.what(), wxConvUTF8, strlen ( e.what() ) );
				messagePane ( _ ( "Cannot replace: " ) + wideError, CONST_STOP );
				return;
			}
		}
		if ( !globalReplaceAllDocuments )
			break;
	}
	wxString msg;

	msg.Printf (
	    wxPLURAL ( "%i replacement made", "%i replacements made", globalMatchCount ),
	    globalMatchCount );

	statusProgress ( msg );
}

void MyFrame::OnToggleComment ( wxCommandEvent& event )
{
	XmlDoc *doc = getActiveDocument();
	if ( doc == NULL )
		return;

	wxBusyCursor cursor;
	doc->toggleComment();
}

void MyFrame::OnFrameClose ( wxCloseEvent& event )
{
	wxCommandEvent e;
	OnCloseAll ( e );
	if ( mainBook->GetPageCount() )
	{
		event.Veto();
		return;
	}
	event.Skip();
}

void MyFrame::OnNew ( wxCommandEvent& WXUNUSED ( event ) )
{
	wxString defaultSelection, typeSelection, templateFile;
	defaultSelection = _ ( "XML document (*.xml)" );
	if ( wxDirExists ( templateDir ) )
	{
		wxArrayString templateArray;
		wxString templateMask, name, extension, entry;
		wxFileName fn;
		templateMask = templateDir + wxFileName::GetPathSeparator() + _T ( "*.*" );
		templateFile = wxFindFirstFile ( templateMask, wxFILE );
		while ( !templateFile.empty() )
		{
			templateFile.Replace ( _T("_"), _T(" ") );
			fn.Assign ( templateFile );
			name = fn.GetName();
			extension = fn.GetExt();

			entry.Printf ( _T ( "%s (*.%s)" ), name.c_str(), extension.c_str() );
			templateArray.Add ( entry );

			templateFile = wxFindNextFile();
		}
		templateArray.Sort();
		templateArray.Insert ( defaultSelection, 0 );

		wxSingleChoiceDialog scd (
		    this, _ ( "Choose a document type:" ), _ ( "New Document" ), templateArray );
		if ( scd.ShowModal() == wxID_CANCEL )
		{
			XmlDoc *doc = getActiveDocument();
			if ( doc )
				doc->SetFocus();
			return;
		}
		typeSelection = scd.GetStringSelection();
	}

	if ( typeSelection == defaultSelection )
	{
		wxString emptyString ( _T ( "" ) );
		newDocument ( emptyString );
		return;
	}

	typeSelection.Replace ( _T ( " (*" ), wxEmptyString );
	typeSelection.Replace ( _T ( ")" ), wxEmptyString );
	typeSelection.Replace ( _T ( " " ), _T ( "_" ) );
	templateFile = templateDir + typeSelection;
	std::string templateFileLocal, buffer;
	templateFileLocal = templateFile.mb_str ( wxConvLocal );
	ReadFile::run ( templateFileLocal, buffer );
	wxString documentContents = wxString ( buffer.c_str(), wxConvUTF8, buffer.size() );

	newDocument ( buffer, templateFile );
}

void MyFrame::newDocument ( const wxString& s, const wxString& path, bool canSave )
{
	std::string bufferUtf8 = ( const char * ) s.mb_str ( wxConvUTF8 );
	newDocument ( bufferUtf8, path, canSave );
}

void MyFrame::newDocument ( const std::string& s, const wxString& path, bool canSave )
{
	XmlDoc *doc;

	wxString documentLabel;
	documentLabel.Printf ( _ ( "Document%i" ), documentCount++ );

	wxString auxPath = getAuxPath ( path );

	{
		wxWindowUpdateLocker noupdate (this);

		doc = ( s.empty() ) ?
			  new XmlDoc (
				  mainBook,
				  properties,
				  &protectTags,
				  visibilityState,
				  FILE_TYPE_XML,
				  wxID_ANY,
				  NULL, 0 // new: NULL pointer leads to default document
			  )
			  : new XmlDoc (
				  mainBook,
				  properties,
				  &protectTags,
				  visibilityState,
				  FILE_TYPE_XML,
				  wxID_ANY,
				  s.c_str(), // modified
				  s.size(), // new
				  path,
				  auxPath );
		mainBook->AddPage ( ( wxWindow * ) doc, documentLabel );
	}

	mainBook->Layout();

	if ( properties.completion )
		doc->updatePromptMaps();
	doc->setShortFileName ( documentLabel );
	doc->SetFocus();
	manager.Update();
	if ( properties.validateAsYouType )
		doc->backgroundValidate();
}

void MyFrame::OnOpen ( wxCommandEvent& event )
{
	bool largeFile;
	largeFile = ( event.GetId() == ID_OPEN_LARGE_FILE );

	wxString file = event.GetString();
	if ( !file.empty() )
	{
		openFile ( file );
		return;
	}

	wxString defaultFile, defaultDir;
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) != NULL )
	{
		defaultFile = doc->getFullFileName();
		if ( !defaultFile.empty() )
		{
			wxFileName fn ( defaultFile );
			defaultDir = fn.GetPath();
		}
	}
	else
		defaultDir = mLastDir;

	wxFileDialog fd (
	    this,
	    ( largeFile ) ? _ ( "Open Large Document" ) : _ ( "Open" ),
	    defaultDir,
	    wxEmptyString,
	    FILE_FILTER,
#if wxCHECK_VERSION(2,9,0)
	    wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST
#else
	    wxOPEN | wxMULTIPLE | wxFILE_MUST_EXIST
#endif
	    );


	if ( fd.ShowModal() == wxID_CANCEL )
		return;

	mLastDir = fd.GetDirectory();

	wxArrayString paths;
	fd.GetPaths ( paths );
	size_t count = paths.Count();
	if ( !count )
		return;
	for ( size_t i = 0; i < count; ++i )
		if ( !openFile ( paths[i], largeFile ) )
			break;
}

bool MyFrame::openFile ( const wxString &file, bool largeFile )
{
	wxFileName fn = WrapLibxml::URLToFileName ( file );
	fn.Normalize();

	wxString fileName = fn.GetFullPath();
	if ( !fn.IsFileReadable() )
	{
		wxString message;
		message.Printf ( _ ( "Cannot open %s." ), fileName.c_str() );
		messagePane ( message, CONST_STOP );
		return false;
	}

	if ( openFileSet.count ( fileName ) )
	{
		wxString message;
		message.Printf ( _ ( "%s is already open" ), fileName.c_str() );
		statusProgress ( message );
		activateTab ( fileName );
		return false;
	}

	wxString wideError;
	pair<int, int> posPair;
	XmlDoc *doc;

	int type = getFileType ( fileName );
	wxString auxPath = getAuxPath ( fileName );

	char *docBuffer = NULL;
	size_t docBufferLen = 0;
	bool fileEmpty = false;

	statusProgress ( _T ( "Opening file..." ) );
	BinaryFile binaryfile ( fileName );
	if ( !binaryfile.getData() )
	{
		wxString message;
		message.Printf ( _ ( "Cannot open %s" ), fileName.c_str() );
		messagePane ( message, CONST_STOP );
		statusProgress ( wxEmptyString );
		return false;
	}
	/*
	//wxMemoryMappedFile memorymap (
	  fileName,
	  true, // readOnly
	  true // fread
	);
	*/

	/*
	  catch (wxMemoryMappedFileEmptyException&)
	  {
	    fileEmpty = true;
	  }
	*/

	bool isUtf8 = false;

	if ( !fileEmpty )
	{
		docBuffer = ( char * ) binaryfile.getData();//memorymap->GetStream();
		docBufferLen = binaryfile.getDataLen();//memorymap->GetMapSize();
	}
	else
	{
		docBuffer = NULL;
		docBufferLen = 0;
		isUtf8 = true;
	}

	statusProgress ( wxEmptyString );

	wxCharBuffer iconvBuffer;
	size_t iconvBufferLen = 0;

	char *finalBuffer;
	size_t finalBufferLen;

	std::string encoding;
	if ( docBufferLen >= 4 && // UTF-32 BE
			( unsigned char ) docBuffer[0] == 0x00 &&
			( unsigned char ) docBuffer[1] == 0x00 &&
			( unsigned char ) docBuffer[2] == 0xFE &&
			( unsigned char ) docBuffer[3] == 0xFF )
	{
		docBuffer += 4;
		docBufferLen -= 4;
		encoding = "UTF-32BE";
	}
	else if ( docBufferLen >= 4 && // UTF-32 LE
			( unsigned char ) docBuffer[0] == 0xFF &&
			( unsigned char ) docBuffer[1] == 0xFE &&
			( unsigned char ) docBuffer[2] == 0x00 &&
			( unsigned char ) docBuffer[3] == 0x00 )
	{
		docBuffer += 4;
		docBufferLen -= 4;
		encoding = "UTF-32LE";
	}
	else if ( docBufferLen >= 2 && //UTF-16 BE
			( unsigned char ) docBuffer[0] == 0xFE &&
			( unsigned char ) docBuffer[1] == 0xFF )
	{
		docBuffer += 2;
		docBufferLen -= 2;
		encoding = "UTF-16BE";
	}
	else if ( docBufferLen >= 2 && //UTF-16 LE
			( unsigned char ) docBuffer[0] == 0xFF &&
			( unsigned char ) docBuffer[1] == 0xFE )
	{
		docBuffer += 2;
		docBufferLen -= 2;
		encoding = "UTF-16LE";
	}
	else if ( docBufferLen >= 3 && //UTF-8
			( unsigned char ) docBuffer[0] == 0xEF &&
			( unsigned char ) docBuffer[1] == 0xBB &&
			( unsigned char ) docBuffer[2] == 0xBF )
	{
		docBuffer += 3;
		docBufferLen -= 3;
		encoding = "UTF-8";
	}

	if ( encoding.empty() )
	{
		XmlEncodingSpy es;
		es.parse ( docBuffer, docBufferLen );
		encoding = es.getEncoding();
		if ( encoding.empty() )  // Expat couldn't parse file (e.g. UTF-32)
			encoding = getApproximateEncoding ( docBuffer, docBufferLen );
	}

	// convert buffer if not UTF-8
	if ( encoding == "UTF-8" ||
		encoding == "utf-8" ||
		encoding == "US-ASCII" ||
		encoding == "us-ascii" || // US-ASCII is a subset of UTF-8
		docBufferLen == 0 )
	{
		finalBuffer = docBuffer;
		finalBufferLen = docBufferLen;
		isUtf8 = true;
	}
	else
	{
		wxString wideEncoding = wxString (
		                            encoding.c_str(),
		                            wxConvLocal,
		                            encoding.size() );
		iconv_t cd = iconv_open ( "UTF-8", encoding.c_str() );
		if ( cd == ( iconv_t )-1 )
		{
			wxString message;
			message.Printf ( _ ( "Cannot open %s: unknown encoding %s" ),
			                 fileName.c_str(),
			                 wideEncoding.c_str() );
			messagePane ( message, CONST_STOP );
			return false;
		};

		int iconvLenMultiplier = 4; // worst case scenario
		if ( encoding == "ISO-8859-1" ||
		        encoding == "UTF-16" ||
		        encoding == "UTF-16BE" ||
		        encoding == "UTF-16LE" )
		{
			iconvLenMultiplier = 2;
		}
		else if ( encoding == "UTF-32" ||
		          encoding == "UTF-32BE" ||
		          encoding == "UTF-32LE" )
		{
			iconvLenMultiplier = 1;
		}

		size_t nconv;
		char *buffer;
		size_t iconvBufferLeft, docBufferLeft;
		iconvBufferLen = iconvBufferLeft = docBufferLen * iconvLenMultiplier + 1;
		docBufferLeft = docBufferLen;
		if ( ( ( ( size_t ) -1 ) - 1 ) / iconvLenMultiplier < docBufferLen
			|| !iconvBuffer.extend ( iconvBufferLen ) )
		{
			wxString message;
			message.Printf ( _ ( "Cannot open %s: out of memory" ),
					fileName.c_str() );
			messagePane ( message, CONST_STOP );
			statusProgress ( wxEmptyString );
			return false;
		}
		finalBuffer = buffer = iconvBuffer.data(); // buffer will be incremented by iconv
		nconv = reinterpret_cast < universal_iconv & > ( iconv ) (
		            cd,
		            &docBuffer,
		            &docBufferLeft,
		            &buffer,
		            &iconvBufferLeft );

		*buffer = '\0';

		iconv_close ( cd );

		if ( nconv == ( size_t )-1 )
		{
			wxString message;
			message.Printf ( _ ( "Cannot open %s: conversion from encoding %s failed" ),
			                 fileName.c_str(),
			                 wideEncoding.c_str() );
			messagePane ( message, CONST_STOP );
			return false;
		}
		finalBufferLen = iconvBufferLen - iconvBufferLeft;
	}

	statusProgress ( _ ( "Creating document view..." ) );
	{
		wxWindowUpdateLocker noupdate ( this );

		doc = new XmlDoc (
			mainBook,
			( largeFile ) ? largeFileProperties: properties,
			&protectTags,
			visibilityState,
			( !binaryfile.getDataLen() ) ? FILE_TYPE_XML : type,
			wxID_ANY,
			finalBuffer,
			finalBufferLen,
			fileName,
			auxPath );
#ifdef __WXMSW__
		doc->SetUndoCollection ( false );
		doc->SetUndoCollection ( true );
#endif

		doc->setFullFileName ( fileName );
		openFileSet.insert ( fileName );
		history.AddFileToHistory ( fileName );
		updateFileMenu();

		mainBook->AddPage ( ( wxWindow * ) doc, fn.GetFullName(), _T ( "" ) );
	}
	statusProgress ( wxEmptyString );

	mainBook->Layout();

	doc->setLastModified ( fn.GetModificationTime() );
	doc->SetFocus();

	if ( type != FILE_TYPE_XML || !binaryfile.getDataLen() )
	{
		return true;
	}

	// NOW parse the document, but don't create a UTF-8 copy
	statusProgress ( _T ( "Parsing document..." ) );
	std::auto_ptr<WrapExpat> we ( new WrapExpat() );

	// omit XML declaration
	if ( !isUtf8 && finalBufferLen > 5 &&
	        finalBuffer[0] == '<' &&
	        finalBuffer[1] == '?' &&
	        finalBuffer[2] == 'x' &&
	        finalBuffer[3] == 'm' &&
	        finalBuffer[4] == 'l' )
	{
		for ( ; *finalBuffer && finalBufferLen; finalBuffer++ && finalBufferLen-- )
		{
			if ( *finalBuffer == '>' )
			{
				finalBuffer++;
				finalBufferLen--;
				break;
			}
		}
	}

	bool optimisedParseSuccess = false;
	if ( finalBuffer )
	{
		optimisedParseSuccess = we->parse ( finalBuffer, finalBufferLen );
		statusProgress ( wxEmptyString );
	}

	// NOW update prompt maps if necessary
	if ( !largeFile && ( properties.completion || properties.validateAsYouType ) )
	{
		statusProgress ( _T ( "Compiling autocompletion lists..." ) );
		doc->updatePromptMaps ( finalBuffer, finalBufferLen );
		statusProgress ( wxEmptyString );
	}

	if ( !largeFile && ( properties.validateAsYouType && doc->getGrammarFound() ) )
	{
		statusProgress ( _T ( "Validating document..." ) );
		//doc->backgroundValidate ( finalBuffer, doc->getFullFileName().mb_str(wxConvUTF8), finalBufferLen );
		doc->backgroundValidate();
		statusProgress ( wxEmptyString );
	}

	if ( !optimisedParseSuccess )
	{
		posPair = we->getErrorPosition();
		-- ( posPair.first );
		messagePane ( we->getLastError(), CONST_WARNING );

		int newPosition = doc->PositionFromLine ( posPair.first );
		doc->SetSelection ( newPosition, newPosition );
		doc->SetFocus();
		doc->setErrorIndicator ( posPair.first, posPair.second );
	}
	else
	{
		closeMessagePane();
	}
	return true;
}

std::string MyFrame::getApproximateEncoding ( char *docBuffer,
        size_t docBufferLen )
{
	std::string line;
	char *it;
	size_t i;

	// grab first line
	for (
	    i = 0, it = docBuffer;
	    i < docBufferLen && *it != '\n' && i < BUFSIZ;
	    i++, it++ )
	{
		if ( *it )
			line += *it;
	}

	std::pair<int, int> limits = XmlEncodingHandler::getEncodingValueLimits ( line );

	if ( limits.first == -1 || limits.second == -1 )
		return "";

	return line.substr ( limits.first, limits.second );
}

void MyFrame::OnToggleFold ( wxCommandEvent& WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->toggleFold();
}

void MyFrame::OnFoldAll ( wxCommandEvent& WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->foldAll();
	doc->SetFocus();
}

void MyFrame::OnUnfoldAll ( wxCommandEvent& WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->unfoldAll();
	doc->SetFocus();
}


void MyFrame::OnQuit ( wxCommandEvent& WXUNUSED ( event ) )
{
	Close ( true );
}

void MyFrame::OnUndo ( wxCommandEvent& WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->Undo();
	doc->setValidationRequired ( true );
	doc->SetFocus();
}

void MyFrame::OnRedo ( wxCommandEvent& WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->Redo();
	doc->setValidationRequired ( true );
	doc->SetFocus();
}

void MyFrame::OnRevert ( wxCommandEvent& WXUNUSED ( event ) )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	while ( doc->GetModify() )
	{
		if ( !doc->CanUndo() )
			return;
		doc->Undo();
		doc->setValidationRequired ( true );
	}
	doc->SetFocus();
}

void MyFrame::OnSpelling ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	statusProgress ( wxEmptyString );
	closeMessagePane();

#ifdef __WXMSW__
	doc->SetUndoCollection ( false );
	doc->SetUndoCollection ( true );
#endif

	int id, type;
	id = event.GetId();
	type = (id == ID_STYLE) ? ID_TYPE_STYLE : ID_TYPE_SPELL;
		

	std::string rawBufferUtf8;
	getRawText ( doc, rawBufferUtf8 );
	bool success = true; // always true for now: well-formedness not req'd 

	auto_ptr<StyleDialog> sd ( new StyleDialog (
	                               this,
	                               wxICON ( appicon ),
	                               rawBufferUtf8,
	                               doc->getShortFileName(),
	                               ruleSetDir,
	                               filterDir,
				                   ( type == ID_TYPE_SPELL ) ? dictionaryPreset : ruleSetPreset,
	                               filterPreset,
 #ifdef __WXMSW__
                                   aspellDataPath,
                                   aspellDictPath,
 #endif
					               type,
	                               ( success ) ? false : true,
	                               stylePosition,
	                               styleSize ) );

	if ( sd->ShowModal() == wxID_OK )
	{
		std::string bufferUtf8 = sd->getEditedString();
		if ( bufferUtf8.empty() )
			messagePane ( _ ( "Edited document empty" ), CONST_STOP );
		else
			doc->SetTextRaw ( bufferUtf8.c_str() );
	}
	
	// update presets if report has been created (even if followed by cancel)
	if (type == ID_TYPE_STYLE)
	{
		ruleSetPreset = sd->getRuleSetPreset();
		filterPreset = sd->getFilterPreset();
	}
	else
		dictionaryPreset = sd->getRuleSetPreset();
		
#ifdef __WXMSW__
	stylePosition = sd->getPosition();
	styleSize = sd->getSize();
#endif
}

void MyFrame::OnPreviousDocument ( wxCommandEvent& WXUNUSED ( event ) )
{
	if ( !getActiveDocument() )
		return;

	statusProgress ( wxEmptyString );
	closeMessagePane();

	int currentSelection = mainBook->GetSelection();
	if ( currentSelection < 1 )
		return;
	mainBook->SetSelection ( --currentSelection );
	XmlDoc *doc = getActiveDocument();
	if ( doc )
		doc->SetFocus();
}

void MyFrame::OnNextDocument ( wxCommandEvent& WXUNUSED ( event ) )
{
	if ( !getActiveDocument() )
		return;

	statusProgress ( wxEmptyString );
	closeMessagePane();

	int currentSelection = mainBook->GetSelection();
	int maxSelection = mainBook->GetPageCount();
	if ( currentSelection >= ( maxSelection - 1 ) )
		return;
	mainBook->SetSelection ( ++currentSelection );
	XmlDoc *doc = getActiveDocument();
	if ( doc )
		doc->SetFocus();
}

void MyFrame::OnSave ( wxCommandEvent& event )
{
	save();
}

void MyFrame::save()
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxString fileName;
	if ( ( fileName = doc->getFullFileName() ) == _T ( "" ) )
	{
		wxCommandEvent event;
		OnSaveAs ( event );
		return;
	}

	if ( !saveFile ( doc, fileName, true ) )
	{} // handle messages in saveFile
}

void MyFrame::OnReload ( wxCommandEvent& event )
{
	reloadTab();
}

void MyFrame::reloadTab()
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	wxString fileName = doc->getFullFileName();

	if ( closeActiveDocument() )
		openFile ( fileName );
}

void MyFrame::OnSaveAs ( wxCommandEvent& event )
{
	saveAs();
}

void MyFrame::saveAs()
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxString defaultFile, defaultDir;
	defaultFile = doc->getFullFileName();
	if ( !defaultFile.empty() )
	{
		wxFileName fn ( defaultFile );
		defaultDir = fn.GetPath();
	}
	auto_ptr<wxFileDialog> fd ( new wxFileDialog (
	                                this,
	                                _ ( "Save As" ),
	                                defaultDir,
	                                defaultFile,
	                                FILE_FILTER,
#if wxCHECK_VERSION(2,9,0)
	                                wxFD_SAVE | wxFD_OVERWRITE_PROMPT ) );
#else
	                                wxSAVE | wxOVERWRITE_PROMPT ) );
#endif
	if ( fd->ShowModal() == wxID_CANCEL )
		return;

	wxString path = fd->GetPath();

	if (
	    openFileSet.count ( path ) &&
	    path != doc->getFullFileName() )
	{
		wxString message;
		message.Printf ( _ ( "%s is already open" ), path.c_str() );
		messagePane ( message, CONST_STOP );
		return;
	}

	wxString name = fd->GetFilename();
	wxString directory;
	wxFileName::SplitPath ( path, &directory, NULL, NULL );
	if ( path == _T ( "" ) )
		return;

	if ( !saveFile ( doc, path, false ) )
		return;

	// if already named, remove from set of open files
	openFileSet.erase ( doc->getFullFileName() );

	doc->setFullFileName ( path );

	history.AddFileToHistory ( path ); // update history
	updateFileMenu();

	int selection = mainBook->GetSelection();
	if ( selection != -1 )
		mainBook->SetPageText ( selection, name );

	wxString title = showFullPathOnFrame ? path : name;
	title += _T ( " - " );
	title += _ ( "XML Copy Editor" );
	SetTitle ( title );
}

void MyFrame::OnUpdateCloseAll ( wxUpdateUIEvent& event )
{
	event.Enable ( mainBook->GetPageCount() > 1 );
}

void MyFrame::OnUpdateReload ( wxUpdateUIEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
	{
		event.Enable ( false );
		return;
	}
	event.Enable ( !doc->getFullFileName().empty() );
}

void MyFrame::OnUpdateCutCopy ( wxUpdateUIEvent& event )
{
	event.Enable ( getActiveDocument() != NULL );
}

void MyFrame::OnUpdateLocationPaneVisible ( wxUpdateUIEvent& event )
{
	if ( !viewMenu )
		return;
	wxAuiPaneInfo info = manager.GetPane ( locationPanel );
	event.Check ( info.IsShown() );
}

void MyFrame::OnUpdateSavedOnly ( wxUpdateUIEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
	{
		event.Enable ( false );
		return;
	}
	event.Enable ( !doc->getFullFileName().empty() );
}

void MyFrame::OnUpdateDocRange ( wxUpdateUIEvent& event )
{
	event.Enable ( getActiveDocument() != NULL );
}

void MyFrame::OnUpdateReplaceRange ( wxUpdateUIEvent& event )
{
	event.Enable ( getActiveDocument() != NULL );
}

void MyFrame::OnUpdateFindAgain ( wxUpdateUIEvent& event )
{
	if ( !getActiveDocument() || findData.GetFindString().empty() )
	{
		event.Enable ( false );
		return;
	}
	event.Enable ( true );
}

void MyFrame::OnUpdateUndo ( wxUpdateUIEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
	{
		event.Enable ( false );
		return;
	}
#ifdef __WXMSW__
	event.Enable((doc->CanUndo()) ? true : false);
#else
	event.Enable ( ( doc->GetModify() ) ? true : false );
#endif
}

void MyFrame::OnUpdateRedo ( wxUpdateUIEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
	{
		event.Enable ( false );
		return;
	}
	event.Enable ( doc->CanRedo() );
}

void MyFrame::OnUpdatePaste ( wxUpdateUIEvent& event )
{
	event.Enable ( getActiveDocument() != NULL );
}

void MyFrame::OnUpdateToggleComment ( wxUpdateUIEvent& event )
{
	XmlDoc *doc = getActiveDocument();
	if ( !doc )
	{
		event.Enable ( false );
		return;
	}

	int from = doc->GetSelectionStart();
	int to = doc->GetSelectionEnd();
	event.Enable ( from != to || doc->getType() == FILE_TYPE_XML );
}

void MyFrame::OnUpdatePreviousDocument ( wxUpdateUIEvent& event )
{
	if ( !getActiveDocument() )
	{
		event.Enable ( false );
		return;
	}
	int currentDocument = mainBook->GetSelection();
	event.Enable ( ( currentDocument < 1 ) ? false : true );
}

void MyFrame::OnUpdateNextDocument ( wxUpdateUIEvent& event )
{
	if ( !getActiveDocument() )
	{
		event.Enable ( false );
		return;
	}
	int currentDocument = mainBook->GetSelection();
	int maxDocument = mainBook->GetPageCount();
	event.Enable ( ( currentDocument >= ( maxDocument - 1 ) ) ? false : true );
}

void MyFrame::OnUpdateCloseMessagePane ( wxUpdateUIEvent& event )
{
	wxAuiPaneInfo &info = manager.GetPane ( htmlReport );
	event.Enable ( info.IsShown() );
}

void MyFrame::OnUpdateCloseFindReplacePane ( wxUpdateUIEvent& event )
{
	wxAuiPaneInfo &info = manager.GetPane ( findReplacePanel );
	event.Enable ( info.IsShown() );
}

void MyFrame::OnUpdateCloseCommandPane ( wxUpdateUIEvent& event )
{
	wxAuiPaneInfo &info = manager.GetPane ( commandPanel );
	event.Enable ( info.IsShown() );
}

void MyFrame::OnValidateDTD ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );

	// fetch document contents
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;


	doc->clearErrorIndicators();
	statusProgress ( _ ( "DTD Validation in progress..." ) );

	auto_ptr<WrapLibxml> wl ( new WrapLibxml ( libxmlNetAccess ) );
	wxString fname = doc->getFullFileName();
	if ( !wl->validate ( doc->myGetTextRaw(), fname ) )
	{
		wxString wideError = wl->getLastError();
		statusProgress ( wxEmptyString );
		messagePane ( wideError, CONST_WARNING );

		std::pair<int, int> posPair = wl->getErrorPosition();
		-- ( posPair.first );
		int cursorPos =
		    doc->PositionFromLine ( posPair.first );
		doc->SetSelection ( cursorPos, cursorPos );

		// shallow validate all
		doc->backgroundValidate(); // has to come first as it deletes all indicators
		doc->setErrorIndicator ( posPair.first, posPair.second );

		return;
	}
	statusProgress ( wxEmptyString );
	documentOk ( _ ( "valid" ) );
}

void MyFrame::OnValidateRelaxNG ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxString fileName = doc->getFullFileName();

	wxString defaultFile, defaultDir;
	defaultFile = doc->getFullFileName();
	if ( !defaultFile.empty() )
	{
		wxFileName fn ( defaultFile );
		defaultDir = fn.GetPath();
	}
	AssociateDialog ad (
	    this,
	    _ ( "Select RELAX NG grammar" ),
	    _ ( "Choose a file:" ),
	    _ ( "RELAX NG grammar" ),
	    _T ( "*.*" ),
	    lastRelaxNGSchema,
	    &mLastDir );
	if ( ad.ShowModal() != wxID_OK )
		return;

	wxString path = lastRelaxNGSchema = ad.getUrl();

	if ( path.empty() )
	{
		statusProgress ( wxEmptyString );
		return;
	}

	validateRelaxNG ( doc, path, fileName );
}

void MyFrame::validateRelaxNG (
    XmlDoc *doc,
    const wxString& schemaName,
    wxString& fileName ) // not const: may change if empty/document modified
{
	statusProgress ( wxEmptyString );

	if ( !doc )
		return;


	doc->clearErrorIndicators();
	statusProgress ( _ ( "RELAX NG validation in progress..." ) );

	auto_ptr<WrapLibxml> wl ( new WrapLibxml ( libxmlNetAccess ) );

	if ( !wl->validateRelaxNG ( schemaName, doc->myGetTextRaw(), fileName ) )
	{
		wxString wideError = wl->getLastError();
		statusProgress ( wxEmptyString );

		std::pair<int, int> posPair = wl->getErrorPosition();
		-- ( posPair.first );

		int cursorPos =
		    doc->PositionFromLine ( posPair.first );
		doc->SetSelection ( cursorPos, cursorPos );
		doc->setErrorIndicator ( posPair.first, posPair.second );
		messagePane ( wideError, CONST_WARNING );
		doc->SetFocus();
		return;
	}

	statusProgress ( wxEmptyString );
	documentOk ( _ ( "valid" ) );
	doc->SetFocus();
}

void MyFrame::OnValidatePreset ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxString fileName = doc->getFullFileName();

	int id = event.GetId();
	wxString schemaFileName = validationPresetMap[id];
	if ( schemaFileName.empty() )
		return;
	validateRelaxNG ( doc, schemaFileName, fileName );
}

void MyFrame::OnValidateSchema ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	// branch: if no XML Schema found, use LibXML DTD parser instead
	// so the catalog is read - switch when Xerces-C implements
	// XMLCatalogResolver
#if 0
	{
		std::string rawBuffer;
		getRawText ( doc, rawBuffer );
		XmlSchemaLocator xsl ( "UTF-8" );
		xsl.parse ( rawBuffer.c_str() );
		if ( ( xsl.getSchemaLocation() ) . empty() )
		{
			OnValidateDTD ( event );
			return;
		}
	}
#endif

	statusProgress ( _ ( "Validation in progress..." ) );
	doc->clearErrorIndicators();

	wxString fileName = doc->getFullFileName();
	std::string utf8Buffer = doc->myGetTextRaw();
	std::auto_ptr<WrapXerces> validator ( new WrapXerces() );
	int severity;
	wxString message;
	if ( validator->validateMemory ( utf8Buffer.c_str(), utf8Buffer.size(),
			fileName ) )
	{
		message.Printf ( _ ( "%s is valid" ),
		                 doc->getShortFileName().c_str() );
		if ( validator->getLastError().empty() )
			severity = CONST_INFO;
		else
		{
			severity = CONST_WARNING;
			message << _T ( "[br][br]" );
		}
	}
	else
	{
		severity = CONST_STOP;
	}
	statusProgress ( wxEmptyString );
	message << validator->getLastError();
	messagePane ( message, severity );

	if ( severity != CONST_INFO )
	{
		std::pair<int, int> posPair = validator->getErrorPosition();
		int cursorPos =
		    doc->PositionFromLine ( posPair.first - 1 );
		doc->SetSelection ( cursorPos, cursorPos );
		doc->setErrorIndicator ( posPair.first - 1, 0 );
	}
}

void MyFrame::OnCreateSchema ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );

	XmlDoc *doc = getActiveDocument();
	if ( doc == NULL )
		return;

	std::string rawBufferUtf8;
	getRawText ( doc, rawBufferUtf8 );

	const static wxString types[] = { _ ( "W3C Schema" ), _ ( "DTD" ) };
	const static wxString message = _ ( "Please choose a shema type");
	wxSingleChoiceDialog dlg ( this, message, _ ( "Schema type" ),
			( int ) sizeof ( types ) / sizeof ( types[0] ), types );
	int ret = dlg.ShowModal();
	if ( ret == wxID_CANCEL ) return;

	Grammar::GrammarType type = ( dlg.GetSelection() == 0 ) ?
			Grammar::SchemaGrammarType : Grammar::DTDGrammarType;
	XmlSchemaGenerator gen;
	const wxString &schema = gen.generate(type, doc->getFullFileName(),
	    rawBufferUtf8.c_str(), rawBufferUtf8.size(), _T ( "UTF-8" ) );
	if (schema.IsEmpty()) {
		messagePane ( gen.getLastError(), CONST_WARNING );
		return;
	}
	newDocument ( schema );
}

void MyFrame::OnDtd2Schema ( wxCommandEvent& event )
{
	closeMessagePane();

#if wxCHECK_VERSION(2,9,0)
	long style = wxFD_OPEN | wxFD_FILE_MUST_EXIST;
#else
	long style = wxOPEN | wxFILE_MUST_EXIST;
#endif
	wxFileDialog fd ( this, _ ( "Please select a DTD file" ), wxEmptyString,
	    wxEmptyString, _T ( "DTD files (*.dtd)|*.dtd|All files (*.*)|*.*" ),
	    style );
	if (fd.ShowModal() != wxID_OK)
		return;

	statusProgress ( _ ( "Converting..." ) );

	Dtd2Schema dtd2xsd;
	const wxString &schema = dtd2xsd.convert ( fd.GetPath() );
	const wxString &error = dtd2xsd.getErrors();
	if ( !error.empty() )
		messagePane ( error, CONST_STOP );
	if ( !schema.empty() )
	{
		statusProgress ( _ ( "Creating document view..." ) );
		newDocument ( schema );
	}

	statusProgress ( wxEmptyString );
}

void MyFrame::OnXPath ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );
	closeMessagePane();

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	auto_ptr<wxTextEntryDialog> dlg ( new wxTextEntryDialog (
	                                      this,
	                                      _ ( "Enter XPath:" ),
	                                      _ ( "Evaluate XPath" ),
	                                      xpathExpression ) );

	int ret = dlg->ShowModal();
	if ( ret == wxID_CANCEL )
		return;
	xpathExpression = dlg->GetValue();

	// fetch document contents
	std::string utf8Buffer;
	getRawText ( doc, utf8Buffer );

	auto_ptr<WrapLibxml> wl ( new WrapLibxml ( libxmlNetAccess ) );
	bool success = wl->xpath ( xpathExpression, utf8Buffer,
			doc->getFullFileName() );

	if ( !success )
	{
		wxString wideError = wl->getLastError();
		if ( !wideError.empty() )
			wideError.Prepend ( _T ( ": " ) );
		wideError.Prepend ( _ ( "Cannot evaluate XPath" ) );

		messagePane ( wideError, CONST_WARNING );
		return;
	}

	statusProgress ( wxEmptyString );
	std::string buffer = wl->getOutput();

	if ( buffer.empty() )
	{
		messagePane ( _ ( "No matching nodes found" ), CONST_WARNING );
		return;
	}
	newDocument ( buffer );
	statusProgress ( wxEmptyString );
}

void MyFrame::OnXslt ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );
	closeMessagePane();

	// fetch document contents
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	std::string rawBufferUtf8 = doc->myGetTextRaw();

	wxString path;
	int id = event.GetId();
	if ( id == ID_XSLT )
	{
		XslLocator xl ( "UTF-8" );
		xl.parse ( rawBufferUtf8 );
		std::string location = xl.getXslLocation();

		path = wxString ( location.c_str(), wxConvUTF8, location.size() );

		path = PathResolver::run ( path, doc->getFullFileName() );

		if ( !wxFileExists ( path ) )
		{
			if ( !path.empty() )
			{
				wxString message;
				message.Printf ( _ ( "Cannot open stylesheet %s" ), path.c_str() );
				messagePane ( message, CONST_WARNING );
			}

			wxString defaultFile, defaultDir;
			defaultFile = doc->getFullFileName();
			if ( !defaultFile.empty() )
			{
				wxFileName fn ( defaultFile );
				defaultDir = fn.GetPath();
			}
			AssociateDialog ad (
			    this,
			    _ ( "Select stylesheet" ),
			    _ ( "Choose a file:" ),
			    _ ( "XSLT stylesheet" ),
			    _T ( "*.xsl;*.xslt" ),
			    lastXslStylesheet,
			    &mLastDir );
			if ( ad.ShowModal() != wxID_OK )
				return;

			path = lastXslStylesheet = ad.getUrl();


			if ( path.empty() ) // Cancel selected
			{
				statusProgress ( wxEmptyString );
				return;
			}
		}
	}
	else
	{
		wxString sep;
		sep.Append ( wxFileName::GetPathSeparator() );
		switch ( id )
		{
			case ID_XSLT_TEI_FO:
				path = applicationDir + sep + _T ( "tei" ) + sep + _T ( "fo" ) + sep +
				       _T ( "tei.xsl" );
				break;
			case ID_XSLT_TEI_HTML:
				path = applicationDir + sep + _T ( "tei" ) + sep + _T ( "html" ) + sep +
				       _T ( "tei.xsl" );
				break;
			case ID_XSLT_TEI_XHTML:
				path = applicationDir + sep + _T ( "tei" ) + sep + _T ( "xhtml" ) + sep +
				       _T ( "tei.xsl" );
				break;
			case ID_XSLT_TEI_LATEX:
				path = applicationDir + sep + _T ( "tei" ) + sep + _T ( "latex" ) + sep +
				       _T ( "tei.xsl" );
				break;
			case ID_XSLT_DOCBOOK_FO:
				path = applicationDir + sep + _T ( "docbook" ) + sep + _T ( "fo" ) + sep +
				       _T ( "docbook.xsl" );
				break;
			case ID_XSLT_DOCBOOK_HTML:
				path = applicationDir + sep + _T ( "docbook" ) + sep + _T ( "html" ) + sep +
				       _T ( "docbook.xsl" );
				break;
			case ID_XSLT_DOCBOOK_XHTML:
				path = applicationDir + sep + _T ( "docbook" ) + sep + _T ( "xhtml" ) + sep +
				       _T ( "docbook.xsl" );
				break;
			default:
				break;
		}
	}
	statusProgress ( _ ( "XSL transformation in progress..." ) );

	auto_ptr<WrapLibxml> wl ( new WrapLibxml ( libxmlNetAccess ) );
	wxString fileName = doc->getFullFileName();
	if ( !wl->xslt ( path, rawBufferUtf8, fileName ) )
	{
		wxString wideError = wl->getLastError();
		wideError.Prepend ( _ ( "Cannot transform: " ) );
		statusProgress ( wxEmptyString );
		messagePane ( wideError, CONST_WARNING );
		return;
	}
	std::string buffer = wl->getOutput();
	if ( buffer.empty() )
	{
		messagePane ( _ ( "Output document empty" ), CONST_WARNING );
		return;
	}
	statusProgress ( wxEmptyString );
	newDocument ( buffer );
}

void MyFrame::OnPrettyPrint ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );
	closeMessagePane();

	// fetch document contents
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	int line = doc->LineFromPosition ( doc->GetCurrentPos() );

	std::string rawBufferUtf8;
	getRawText ( doc, rawBufferUtf8 );

	std::string encoding = XmlEncodingHandler::get ( rawBufferUtf8 );

	statusProgress ( _ ( "Pretty-printing in progress..." ) );

	wxString fileName = doc->getFullFileName();
	auto_ptr<WrapLibxml> wl ( new WrapLibxml ( libxmlNetAccess ) );
	for ( int i = 0; i < 2; i++ ) // perform two iterations
	{

		if ( !wl->parse ( rawBufferUtf8, fileName, true ) )
		{
			wxString wideError = wl->getLastError();
			wideError.Prepend ( _ ( "Cannot pretty-print: " ) );
			statusProgress ( wxEmptyString );
			messagePane ( wideError, CONST_WARNING );
			return;
		}
		rawBufferUtf8 = wl->getOutput();
	}

	statusProgress ( wxEmptyString );
	if ( rawBufferUtf8.empty() )
		messagePane (
		    _ ( "Pretty-print unsuccessful: output document empty" ),
		    CONST_STOP );
	else
	{
		if ( encoding != "UTF-8" && !encoding.empty() )
		{
			XmlEncodingHandler::set ( rawBufferUtf8, encoding );
		}
		doc->SetTextRaw ( rawBufferUtf8.c_str() );
		statusProgress ( wxEmptyString );
	}

	doc->setValidationRequired ( true );
	doc->GotoLine ( line );
	doc->SetFocus();
}

void MyFrame::OnEncoding ( wxCommandEvent& event )
{
	statusProgress ( wxEmptyString );
	closeMessagePane();

	// fetch document contents
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	const static wxString encodings[] = {
		_T ( "UTF-8" ), _T ( "UTF-16" ), _T ( "UTF-16LE" ),
		_T ( "UTF-16BE" ), _T ( "ISO-8859-1" ), _T ( "US-ASCII" )
	};
	wxSingleChoiceDialog scd (
	    this, _ ( "Choose an encoding:" ), _ ( "Encoding" ),
	    sizeof ( encodings ) / sizeof ( encodings[0] ), encodings );
	if ( scd.ShowModal() == wxID_CANCEL )
		return;

	int res;
	wxMemoryBuffer output;
	std::auto_ptr<WrapLibxml> wl ( new WrapLibxml ( libxmlNetAccess ) );
	res = wl->saveEncoding ( doc->myGetTextRaw(), doc->getFullFileName(),
			wxEmptyString, &output, scd.GetStringSelection() );
	if ( res == -1 )
	{
		wxString wideError = wl->getLastError();
		wideError.Prepend ( _ ( "Cannot set encoding: " ) );
		messagePane ( wideError, CONST_STOP );
		return;
	}

	std::auto_ptr<XmlUtf8Reader> xur ( new XmlUtf8Reader (
	                                       false,
	                                       expandInternalEntities,
	                                       output.GetDataLen() ) );
	if ( !xur->parse ( ( const char * ) output.GetData(), output.GetDataLen() ) )
	{
		messagePane ( _ ( "Cannot set encoding (cannot parse temporary file)" ),
		              CONST_STOP );
		return;
	}

	doc->SetTextRaw ( xur->getBuffer().c_str() );
	doc->setValidationRequired ( true );
	doc->SetFocus();
}

void MyFrame::OnHome ( wxCommandEvent& event )
{
	wxLaunchDefaultBrowser ( _T ( "http://xml-copy-editor.sourceforge.net" ) );
}

void MyFrame::OnDownloadSource ( wxCommandEvent& event )
{
	wxLaunchDefaultBrowser ( _T ( "http://sourceforge.net/p/xml-copy-editor/code/" ) );
}

void MyFrame::OnToolbarVisible ( wxCommandEvent& event )
{
	if ( !viewMenu )
		return;
	toolbarVisible = ( toolbarVisible ) ? false : true;
	viewMenu->Check ( ID_TOOLBAR_VISIBLE, toolbarVisible );
	showTopBars ( toolbarVisible );
	manager.Update();
}

void MyFrame::OnWrapWords ( wxCommandEvent& event )
{
	if ( !viewMenu )
		return;

	bool wrapWords;
	wrapWords = ( properties.wrap ) ? false : true;

	viewMenu->Check ( ID_WRAP_WORDS, wrapWords );
	properties.wrap = wrapWords;

	// update all documents
	int pageCount = mainBook->GetPageCount();
	XmlDoc *currentDoc;
	for ( int i = 0; i < pageCount; ++i )
	{
		currentDoc = ( XmlDoc * ) mainBook->GetPage ( i );
		if ( !currentDoc )
			break;
		currentDoc->SetWrapMode ( wrapWords );
	}
}

void MyFrame::OnLocationPaneVisible ( wxCommandEvent& event )
{
	wxAuiPaneInfo info = manager.GetPane ( locationPanel );
	bool visible = ( info.IsShown() ) ? false : true;
	manager.GetPane ( locationPanel ).Show ( visible );
	manager.Update();

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	doc->SetFocus();
}

void MyFrame::OnProtectTags ( wxCommandEvent& event )
{
	if ( !xmlMenu )
		return;
	protectTags = ( protectTags ) ? false : true;
	if ( xmlMenu )
		xmlMenu->Check ( ID_PROTECT_TAGS, protectTags );
	if ( toolBar )
		toolBar->ToggleTool ( ID_PROTECT_TAGS, protectTags );

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	if ( protectTags )
		doc->adjustCursor(); // apply to all open docs?

	doc->SetFocus();
}

void MyFrame::OnVisibilityState ( wxCommandEvent& event )
{

	int id;
	id = event.GetId();
	switch ( id )
	{
		case ID_SHOW_TAGS:
			visibilityState = SHOW_TAGS;
			//GetStatusBar()->SetStatusText(wxEmptyString, STATUS_PARENT);
			break;
		case ID_HIDE_ATTRIBUTES:
			visibilityState = HIDE_ATTRIBUTES;
			//GetStatusBar()->SetStatusText(wxEmptyString, STATUS_PARENT);
			break;
		case ID_HIDE_TAGS:
			visibilityState = HIDE_TAGS;
			break;
		default:
			visibilityState = SHOW_TAGS;
			break;
	}
	if ( viewMenu )
		viewMenu->Check ( id, true );

	// iterate over all open documents
	int pageCount = mainBook->GetPageCount();
	XmlDoc *currentDoc;
	for ( int i = 0; i < pageCount; ++i )
	{
		currentDoc = ( XmlDoc * ) mainBook->GetPage ( i );
		if ( !currentDoc )
			break;
		currentDoc->applyVisibilityState ( visibilityState );
	}

	if ( visibilityState == HIDE_ATTRIBUTES || visibilityState == HIDE_TAGS )
	{
		if ( properties.protectHiddenElements && !protectTags )
		{
			wxCommandEvent e;
			OnProtectTags ( e );
		}
	}

	// fetch current document
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	// set focus for current document
	doc->SetFocus();
}

void MyFrame::OnFeedback ( wxCommandEvent& event )
{
	wxString forumUrl =
	    _T ( "https://sourceforge.net/forum/forum.php?forum_id=475215" );
	wxLaunchDefaultBrowser ( forumUrl );
}

void MyFrame::findAgain ( wxString s, int flags )
{
	findReplacePanel->flagNotFound ( false );

	if ( s.empty() )
		return;

	statusProgress ( wxEmptyString );
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	// update regex parameter to keep global replace in sync
	findRegex = findReplacePanel->getRegex();

	int newLocation;
	int myFlags = 0;
	if ( flags & wxFR_WHOLEWORD )
		myFlags |= wxSTC_FIND_WHOLEWORD;
	if ( flags & wxFR_MATCHCASE )
		myFlags |= wxSTC_FIND_MATCHCASE;
	if ( findReplacePanel->getRegex() )
		myFlags |= wxSTC_FIND_REGEXP;

	//doc->SetYCaretPolicy(wxSTC_CARET_SLOP | wxSTC_CARET_STRICT, 10);

	if ( flags & wxFR_DOWN ) // find next
	{
		doc->SetTargetStart ( findReplacePanel->getIncrementalFind()
				? doc->GetSelectionStart()  : doc->GetSelectionEnd() );
		doc->SetTargetEnd ( doc->GetLength() );
		doc->SetSearchFlags ( myFlags );
		newLocation = doc->SearchInTarget ( s );

		// try once more from top
		if ( newLocation == -1 )
		{
			doc->SetTargetStart ( 0 );
			doc->SetTargetEnd ( doc->GetLength() );
			newLocation = doc->SearchInTarget ( s );
		}
	}
	else // find previous
	{
		doc->SetCurrentPos (
		    ( doc->GetSelectionStart() ) ? doc->GetSelectionStart() - 1 : 0 );
		doc->SearchAnchor();
		newLocation = doc->SearchPrev ( myFlags, s );
	}

	//doc->SetYCaretPolicy(wxSTC_CARET_SLOP, 0);


	if ( newLocation == -1 )
	{
		findReplacePanel->flagNotFound ( true );
		wxString err;
		err.Printf ( _ ( "Cannot find '%s'" ), s.c_str() );
		doc->SetSelectionEnd ( doc->GetSelectionStart() );

		statusProgress ( err );

		// must clear target to prevent replace affecting whole document
		doc->SetTargetStart ( 0 );
		doc->SetTargetEnd ( 0 );

		return;
	}

	doc->SetSelection ( doc->GetTargetStart(), doc->GetTargetEnd() );
	doc->EnsureCaretVisible();
}

bool MyFrame::closeActiveDocument()
{
	statusProgress ( wxEmptyString );
	closeMessagePane();

	int selection = mainBook->GetSelection();
	if ( selection == -1 || !mainBook->GetPageCount() ) // GetPageCount needed for wxAuiNotebook
		return false;

	locationPanel->update ( NULL, wxEmptyString );
	insertChildPanel->update ( NULL, wxEmptyString );
	insertSiblingPanel->update ( NULL, wxEmptyString );

	    // workaround -- wxAuiNotebook: send virtual close event? DeletePage doesn't generate one
	    wxAuiNotebookEvent e;
	    e.SetSelection ( selection );
	    OnPageClosing ( e );
	    if ( deletePageVetoed )
	        return false;

	    mainBook->DeletePage ( selection ); // check this is still correct
	    return true;

	// apparently fixed betw. 2.8.0 and 2.8.6, so from v. 1.1.0.3, BUT this is once more:
	// 1.1.0.5: reverted to above workaround - no confirmation dialog?
	// watch memory usage for DeletePage call
	//mainBook->DeletePage ( selection );
	//return ( !deletePageVetoed );
}

bool MyFrame::saveFile ( XmlDoc *doc, wxString& fileName, bool checkLastModified )
{
	if ( !doc )
		return false;

	statusProgress ( wxEmptyString );

	if ( checkLastModified )
	{
		wxFileName fn ( fileName );
		if ( fn.IsOk() )
		{
			wxDateTime myLastModified = fn.GetModificationTime();
			if ( !myLastModified.IsEqualTo ( doc->getLastModified() ) )
			{
				int choice = wxMessageBox (
				                 _ ( "File has been modified by another application.\nDo you want to proceed?" ),
				                 _ ( "Confirmation" ),
				                 wxICON_QUESTION | wxYES_NO | wxCANCEL );
				if ( choice != wxYES )
					return false;
			}
		}
	}

	int bytes = 0;
	std::string utf8Buffer, encoding, fileNameLocal;
	wxString wideEncoding;
	bool isXml = true;
	try
	{
		getRawText ( doc, utf8Buffer );
		XmlEncodingSpy es ( "UTF-8" );
		es.parse ( utf8Buffer );
		encoding = es.getEncoding();
		wideEncoding = wxString ( encoding.c_str(), wxConvUTF8 );

		fileNameLocal = fileName.mb_str ( wxConvLocal );

		closeMessagePane();
		bool success;
		success = true;
		if ( getFileType ( fileName ) != FILE_TYPE_XML )
		{
			isXml = false;
		}

		// raw file conditions
		if ( doc->getType() == FILE_TYPE_BINARY )
		{
			success = saveRawUtf8 ( fileNameLocal, utf8Buffer, true, isXml );
			if ( success )
				bytes = utf8Buffer.size();
			else
			{
				wxString message;
				message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
				messagePane ( message, CONST_STOP );
				return false;
			}

		}
		else if ( !isXml && encoding.empty() )
		{
			success = saveRawUtf8 ( fileNameLocal, utf8Buffer, true, isXml );
			if ( success )
				bytes = utf8Buffer.size();
			else
			{
				wxString message;
				message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
				messagePane ( message, CONST_STOP );
				return false;
			}
		}
		else if ( encoding == "UTF-8" )
		{
			WrapExpat we ( "UTF-8" );

			if ( !we.parse ( utf8Buffer ) )
			{
				//if ( we->isEncodingError() )
				//	;
				messagePane ( we.getLastError(), CONST_WARNING );
			}
			success = saveRawUtf8 ( fileNameLocal, utf8Buffer, true, isXml );
			if ( success )
				bytes = utf8Buffer.size();
			else
			{
				wxString message;
				message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
				messagePane ( message, CONST_STOP );
				return false;
			}
		}
		else
		{
			// IF Unicode, use iconv to convert buffer
			if ( encoding == "UTF-16" || encoding == "UTF-16LE" ||
			        encoding == "UTF-16BE" || encoding == "UTF-32" || encoding == "UTF-32LE" ||
			        encoding == "UTF-32BE" )
			{
				iconv_t cd = iconv_open ( encoding.c_str(), "UTF-8" );
				if ( cd == ( iconv_t )-1 )
				{
					success = saveRawUtf8 ( fileNameLocal, utf8Buffer, false, isXml );
					if ( success )
					{
						bytes = utf8Buffer.size();
						wxString message;
						message.Printf (
						    _ ( "%s saved in default encoding UTF-8: unknown encoding %s" ),
						    fileName.c_str(),
						    wideEncoding.c_str() );
						messagePane ( message, CONST_WARNING );
					}
					else
					{
						wxString message;
						message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
						messagePane ( message, CONST_STOP );
						return false;
					}
				}
				else
				{
					size_t utf8BufferLen = utf8Buffer.size();

					size_t iconvBufferLen, iconvBufferLeft, utf8BufferLeft;
					int iconvLenMultiplier = 4; // worst case scenario
					if ( encoding == "UTF-16" ||
					        encoding == "UTF-16BE" ||
					        encoding == "UTF-16LE" )
					{
						iconvLenMultiplier = 2;
					}
					else if ( encoding == "UTF-32" ||
					          encoding == "UTF-32BE" ||
					          encoding == "UTF-32LE" )
					{
						iconvLenMultiplier = 4;
					}

					iconvBufferLen = iconvBufferLeft =
					                     utf8BufferLen * iconvLenMultiplier + 4; // worst case scenario

					char *finalBuffer;
					char *iconvBuffer;

					utf8BufferLeft = utf8BufferLen;
					iconvBuffer = new char[iconvBufferLen];
					finalBuffer = iconvBuffer; // iconvBuffer will be incremented by iconv
					size_t nconv;

					char *utf8BufferPtr = ( char * ) utf8Buffer.c_str();

					nconv = reinterpret_cast < universal_iconv & > ( iconv ) (
					            cd,
					            &utf8BufferPtr,
					            &utf8BufferLeft,
					            &iconvBuffer,
					            &iconvBufferLeft );

					iconv_close ( cd );

					if ( nconv == ( size_t )-1 ) // conversion failed
					{
						delete[] finalBuffer;
						success = saveRawUtf8 ( fileNameLocal, utf8Buffer, false, isXml );
						if ( success )
						{
							bytes = utf8Buffer.size();
							wxString message;
							message.Printf (
							    _ ( "%s saved in default encoding UTF-8: conversion to %s failed" ),
							    fileName.c_str(),
							    wideEncoding.c_str() );
							messagePane ( message, CONST_WARNING );
						}
						else
						{
							wxString message;
							message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
							messagePane ( message, CONST_STOP );
							return false;
						}
					}
					else
					{
						size_t finalBufferLen = iconvBufferLen - iconvBufferLeft;

						std::ofstream ofs ( fileNameLocal.c_str(), std::ios::out | std::ios::binary );
						if ( !ofs )
						{
							delete[] finalBuffer;
							wxString message;
							message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
							messagePane ( message, CONST_STOP );
							return false;
						}

						// iconv adds boms for UTF-16 & UTF-32 automatically

						ofs.write ( finalBuffer, finalBufferLen );
						ofs.close();
						delete[] finalBuffer;
						bytes = finalBufferLen;
					}
				}
			}
			else // all other encodings handled by Libxml
			{
				auto_ptr<WrapLibxml> wl ( new WrapLibxml ( libxmlNetAccess ) );
				int result = wl->saveEncoding ( utf8Buffer,
						doc->getFullFileName(), fileName, NULL, wideEncoding );
				if ( result == -1 )
				{
					success = saveRawUtf8 ( fileNameLocal, utf8Buffer, false, isXml );
					if ( success )
					{
						wxString wideError = wl->getLastError();
						bytes = utf8Buffer.size();
						wxString msg;
						if ( wideError.empty() )
							wideError = _ ( "unknown error" );

						msg.Printf ( _ ( "Cannot save document in %s: %s (saved in default encoding UTF-8)" ),
						             wideEncoding.c_str(), wideError.c_str() );
						messagePane ( msg, CONST_INFO );
					}
					else
					{
						wxString message;
						message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
						messagePane ( message, CONST_STOP );
						return false;
					}
				}
				else
					bytes = result;
			}
		}
	} // try
	catch ( std::bad_alloc& )
	{
		if ( encoding != "UTF-8" )
		{
			int answer = wxMessageBox (
			                 _ ( "Out of memory: attempt to save in default encoding UTF-8?" ),
			                 _ ( "Out of memory" ),
			                 wxYES_NO | wxCANCEL | wxICON_QUESTION,
			                 this );
			if ( answer == wxCANCEL || answer == wxNO )
				return false;

			bool success = saveRawUtf8 ( fileNameLocal, utf8Buffer, true, isXml );
			if ( success )
			{
				bytes = utf8Buffer.size();
				wxString message;
				message.Printf (
				    _ ( "%s saved in default encoding UTF-8" ),
				    fileName.c_str() );
				messagePane ( message, CONST_INFO );
			}
			else
			{
				wxString message;
				message.Printf ( _ ( "Cannot save %s" ), fileName.c_str() );
				messagePane ( message, CONST_STOP );
				return false;
			}
		}
	}

	doc->SetFocus();
	doc->SetSavePoint();

	if ( properties.validateAsYouType && isXml )
	{
		doc->clearErrorIndicators();
		//doc->backgroundValidate ( utf8Buffer.c_str(), doc->getFullFileName().mb_str(wxConvUTF8), utf8Buffer.size() );
		doc->backgroundValidate();
	}

	if ( !unlimitedUndo )
		doc->EmptyUndoBuffer();
	wxFileName fn ( fileName );
	if ( fn.IsOk() )
		doc->setLastModified ( fn.GetModificationTime() );
	openFileSet.insert ( fileName );
	displaySavedStatus ( bytes );
	return true;
}

bool MyFrame::saveRawUtf8 (
    const std::string& fileNameLocal,
    std::string& bufferUtf8,
    bool ignoreEncoding,
    bool isXml )
{
	ofstream ofs ( fileNameLocal.c_str(), std::ios::out | std::ios::binary );
	if ( !ofs )
		return false;

	if ( !ignoreEncoding && isXml )
		XmlEncodingHandler::setUtf8 ( bufferUtf8, true );

	if ( saveBom && isXml )
	{
		static const char bom[4] = "\xEF\xBB\xBF";
		ofs.write ( bom, 3 );
	}
	ofs.write ( bufferUtf8.c_str(), bufferUtf8.size() );
	ofs.close();
	return true;
}

void MyFrame::displaySavedStatus ( int bytes )
{
	wxString unit;
	float result = 0;
	if ( bytes > 1000000 )
	{
		result = bytes / 1000000;
		unit = _ ( "MB" );
	}
	else if ( bytes > 1000 )
	{
		result = bytes / 1000;
		unit = _ ( "kB" );
	}
	else if ( bytes >= 0 )
	{
		result = bytes;
		unit = wxPLURAL ( "byte", "bytes", bytes );
	}
	else
		return;

	wxString msg;

	msg.Printf (
	    _ ( "%g %s saved" ),
	    result,
	    unit.c_str() );
	statusProgress ( msg );
}

bool MyFrame::getHandleCommandLineFlag()
{
	return handleCommandLineFlag;
}

wxMenuBar *MyFrame::getMenuBar()
{
	fileMenu = new wxMenu; // use class-wide data member
	updateFileMenu ( false );

	// edit menu
	wxMenu *editMenu = new wxMenu;

	wxMenuItem *undoItem =
	    new wxMenuItem ( editMenu, wxID_UNDO, _ ( "&Undo\tCtrl+Z" ), _ ( "Undo" ) );
	undoItem->SetBitmap ( undo16Bitmap );

	wxMenuItem *redoItem =
	    new wxMenuItem ( editMenu, wxID_REDO, _ ( "&Redo\tCtrl+Y" ), _ ( "Redo" ) );
	redoItem->SetBitmap ( redo16Bitmap );

	wxMenuItem *cutItem =
	    new wxMenuItem ( editMenu, wxID_CUT, _ ( "&Cut\tCtrl+X" ), _ ( "Cut" ) );
	cutItem->SetBitmap ( cutBitmap );

	wxMenuItem *copyItem =
	    new wxMenuItem ( editMenu, wxID_COPY, _ ( "C&opy\tCtrl+C" ), _ ( "Copy" ) );
	copyItem->SetBitmap ( copyBitmap );

	wxMenuItem *pasteItem =
	    new wxMenuItem ( editMenu, wxID_PASTE, _ ( "&Paste\tCtrl+V" ), _ ( "Paste" ) );
	pasteItem->SetBitmap ( pasteBitmap );

	wxMenuItem *pasteNewDocumentItem =
	    new wxMenuItem (
	    editMenu,
	    ID_PASTE_NEW_DOCUMENT,
	    _ ( "P&aste As New Document" ),
	    _ ( "Paste As New Document" ) );
	pasteNewDocumentItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *findItem =
	    new wxMenuItem ( editMenu, ID_FIND, _ ( "&Find...\tCtrl+F" ), _ ( "Find..." ) );
	findItem->SetBitmap ( searchBitmap );

	wxMenuItem *findAgainItem =
	    new wxMenuItem ( editMenu, ID_FIND_AGAIN, _ ( "F&ind Again\tF3" ), _ ( "Find Again" ) );
	findAgainItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *replaceItem =
	    new wxMenuItem ( editMenu, ID_REPLACE, _ ( "&Replace...\tCtrl+R" ), _ ( "Replace..." ) );
	replaceItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *globalReplaceItem =
	    new wxMenuItem (
	    editMenu,
	    ID_GLOBAL_REPLACE,
	    _ ( "&Global Replace...\tCtrl+Shift+R" ),
	    _ ( "Global Replace..." ) );
	globalReplaceItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *gotoItem =
	    new wxMenuItem ( editMenu, ID_GOTO, _ ( "G&o To...\tCtrl+G" ), _ ( "Go To..." ) );
	gotoItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *commentItem =
	    new wxMenuItem ( editMenu, ID_TOGGLE_COMMENT, _ ( "&Toggle Comment\tCtrl+/" ), _ ( "Toggle Comment" ) );
	commentItem->SetBitmap ( wxNullBitmap );

	editMenu->Append ( undoItem );
	editMenu->Append ( redoItem );
	editMenu->AppendSeparator();
	editMenu->Append ( cutItem );
	editMenu->Append ( copyItem );
	editMenu->Append ( pasteItem );
	editMenu->Append ( pasteNewDocumentItem );
	editMenu->AppendSeparator();
	editMenu->Append ( findItem );
	editMenu->Append ( findAgainItem );
	editMenu->Append ( replaceItem );
	editMenu->Append ( globalReplaceItem );
	editMenu->Append ( gotoItem );
	editMenu->AppendSeparator();
	editMenu->Append ( commentItem );

#ifndef __WXMSW__
	wxMenuItem *preferencesItem =
	    new wxMenuItem ( editMenu, ID_OPTIONS, _ ( "Pr&eferences..." ), _ ( "Preferences..." ) );
	editMenu->AppendSeparator();
	editMenu->Append ( preferencesItem );
#endif

	// font size menu
	wxMenu *fontSizeMenu = new wxMenu;
	fontSizeMenu->Append (
	    ID_FONT_LARGER, _ ( "Increase\tCtrl+U" ), _ ( "Increase" ) );
	fontSizeMenu->Append (
	    ID_FONT_SMALLER, _ ( "Decrease\tCtrl+D" ), _ ( "Decrease" ) );
	fontSizeMenu->AppendSeparator();
	fontSizeMenu->Append ( ID_FONT_NORMAL, _ ( "Normal\tCtrl+0" ), _ ( "Normal" ) );

	// color scheme menu
	colorSchemeMenu = new wxMenu;
	colorSchemeMenu->AppendRadioItem (
	    ID_COLOR_SCHEME_DEFAULT, _ ( "&Default" ), _ ( "Default" ) );
	colorSchemeMenu->AppendRadioItem (
	    ID_COLOR_SCHEME_REDUCED_GLARE,
	    _ ( "&Blue background, white text" ),
	    _ ( "Blue background, white text" ) );
	colorSchemeMenu->AppendRadioItem (
	    ID_COLOR_SCHEME_DEFAULT_BACKGROUND,
	    _ ( "&Light" ),
	    _ ( "Light" ) );
	colorSchemeMenu->AppendRadioItem (
	    ID_COLOR_SCHEME_NONE,
	    _ ( "&None" ),
	    _ ( "None" ) );

	switch ( properties.colorScheme )
	{
		case COLOR_SCHEME_DEFAULT:
			colorSchemeMenu->Check ( ID_COLOR_SCHEME_DEFAULT, true );
			break;
		case COLOR_SCHEME_DEFAULT_BACKGROUND:
			colorSchemeMenu->Check ( ID_COLOR_SCHEME_DEFAULT_BACKGROUND, true );
			break;
		case COLOR_SCHEME_REDUCED_GLARE:
			colorSchemeMenu->Check ( ID_COLOR_SCHEME_REDUCED_GLARE, true );
			break;
		case COLOR_SCHEME_NONE:
			colorSchemeMenu->Check ( ID_COLOR_SCHEME_NONE, true );
			break;
		default:
			break;
	}

	 /* WAIT FOR AUI LIBRARY TO SUPPORT THIS - currently always splits left
	wxMenu *splitTabMenu = new wxMenu;
	splitTabMenu->Append ( ID_SPLIT_TAB_TOP, _ ( "&Top" ), _ ( "Top" ));
	splitTabMenu->Append ( ID_SPLIT_TAB_RIGHT, _ ( "&Right" ), _ ( "Right" ));
	splitTabMenu->Append ( ID_SPLIT_TAB_BOTTOM, _ ( "&Bottom" ), _ ( "Bottom" ));
	splitTabMenu->Append ( ID_SPLIT_TAB_LEFT, _ ( "&Left" ), _ ( "Left" ));
	 */

	viewMenu = new wxMenu; // use class-wide data member
	viewMenu->Append ( ID_PREVIOUS_DOCUMENT, _ ( "&Previous Document\tCtrl+PgUp" ), _ ( "Previous Document" ) );
	viewMenu->Append ( ID_NEXT_DOCUMENT, _ ( "&Next Document\tCtrl+PgDn" ), _ ( "Next Document" ) );

	//viewMenu->Append ( wxID_ANY, _ ( "&Split Tab" ), splitTabMenu );

	viewMenu->Append ( ID_BROWSER, _ ( "&Browser\tCtrl+B" ), _ ( "Browser" ) );
	viewMenu->AppendSeparator();
	viewMenu->AppendRadioItem (
	    ID_SHOW_TAGS,
	    _ ( "&Show Tags and Attributes\tCtrl+T" ), _ ( "Show Tags and Attributes" ) );
	viewMenu->AppendRadioItem (
	    ID_HIDE_ATTRIBUTES,
	    _ ( "&Hide Attributes Only\tCtrl+Shift+A" ), _ ( "Hide Attributes Only" ) );
	viewMenu->AppendRadioItem (
	    ID_HIDE_TAGS,
	    _ ( "H&ide Tags and Attributes\tCtrl+Shift+T" ), _ ( "Hide Tags and Attributes" ) );

	switch ( visibilityState )
	{
		case SHOW_TAGS:
			viewMenu->Check ( ID_SHOW_TAGS, true );
			break;
		case HIDE_TAGS:
			viewMenu->Check ( ID_HIDE_TAGS, true );
			break;
		case HIDE_ATTRIBUTES:
			viewMenu->Check ( ID_HIDE_ATTRIBUTES, true );
			break;
		default:
			viewMenu->Check ( ID_SHOW_TAGS, true );
			break;
	}
	viewMenu->AppendSeparator();
	viewMenu->Append (
	    ID_TOGGLE_FOLD, _ ( "&Toggle Fold\tCtrl+Alt+T" ), _ ( "Toggle Fold" ) );
	viewMenu->Append (
	    ID_FOLD_ALL, _ ( "&Fold Tags\tCtrl+Shift+F" ), _ ( "Fold Tags" ) );
	viewMenu->Append (
	    ID_UNFOLD_ALL, _ ( "&Unfold Tags\tCtrl+Shift+U" ), _T ( "Unfold Tags" ) );
	viewMenu->AppendSeparator();
	viewMenu->AppendCheckItem (
	    ID_WRAP_WORDS, _ ( "&Wrap Words\tCtrl+W" ), _T ( "Wrap Words" ) );
	viewMenu->Check ( ID_WRAP_WORDS, properties.wrap );
	viewMenu->Append ( wxID_ANY, _ ( "&Color Scheme" ), colorSchemeMenu );
	viewMenu->Append ( wxID_ANY, _ ( "&Text Size" ), fontSizeMenu );
	viewMenu->AppendSeparator();

	viewMenu->AppendCheckItem (
	    ID_LOCATION_PANE_VISIBLE,
	    _ ( "S&how Current Element Pane" ),
	    _ ( "Show Current Element Pane" ) );
	viewMenu->Check ( ID_LOCATION_PANE_VISIBLE, false );
	viewMenu->AppendCheckItem (
	    ID_TOOLBAR_VISIBLE, _ ( "Sh&ow Toolbar" ), _ ( "Show Toolbar" ) );
	viewMenu->Check ( ID_TOOLBAR_VISIBLE, toolbarVisible );
	viewMenu->Append ( ID_CLOSE_MESSAGE_PANE,
	    _ ( "C&lose Message Pane\tAlt+C" ), _ ( "Close Message Pane" ) );
	viewMenu->Append ( ID_CLOSE_FIND_REPLACE_PANE,
	    _ ( "Close Find/&Replace Pane" ), _ ( "Close Find/Replace Pane" ) );
	viewMenu->Append ( ID_CLOSE_COMMAND_PANE,
	    _ ( "Close Co&mmand Pane" ), _ ( "Close Command Pane" ) );

	// insert menu
	wxMenu *insertMenu = new wxMenu;
	insertMenu->Append ( ID_INSERT_CHILD, _ ( "&Element...\tCtrl+I" ), _ ( "Element..." ) );
	insertMenu->Append ( ID_INSERT_SIBLING, _ ( "&Sibling...\tCtrl+Shift+I" ), _ ( "Sibling..." ) );
	insertMenu->Append ( ID_INSERT_ENTITY, _ ( "&Entity...\tCtrl+E" ), _ ( "Entity..." ) );
	insertMenu->AppendSeparator();
	insertMenu->Append ( ID_INSERT_TWIN, _ ( "&Twin\tCtrl+Enter" ), _ ( "Twin" ) );
	insertMenu->AppendSeparator();
	insertMenu->Append ( ID_INSERT_SYMBOL, _ ( "S&ymbol..." ), _ ( "Symbol..." ) );

	// validation menu
	wxMenu *validationMenu = new wxMenu;
	/*
	    validationMenu->Append ( ID_VALIDATE_DTD, _ ( "&DTD\tF4" ), _ ( "DTD" ) );
	*/
	validationMenu->Append (
	    ID_VALIDATE_W3C_SCHEMA, _ ( "&DTD/XML Schema\tF5" ), _ ( "DTD/XML Schema" ) );
	validationMenu->AppendSeparator();
	validationMenu->Append (
	    ID_VALIDATE_RELAX_NG, _ ( "&RELAX NG...\tF6" ), _ ( "RELAX NG..." ) );

	wxMenu *associateMenu = new wxMenu;
	associateMenu->Append ( ID_ASSOCIATE_DTD_PUBLIC, _ ( "&Public DTD..." ), _ ( "Public DTD..." ) );
	associateMenu->Append ( ID_ASSOCIATE_DTD_SYSTEM, _ ( "&System DTD..." ), _ ( "System DTD..." ) );
	associateMenu->Append ( ID_ASSOCIATE_W3C_SCHEMA, _ ( "&XML Schema..." ), _ ( "XML Schema..." ) );
	associateMenu->Append ( ID_ASSOCIATE_XSL, _ ( "XS&LT stylesheet..." ), _ ( "XSLT stylesheet..." ) );

	if ( wxDirExists ( rngDir ) )
	{
		wxString rngMask, rngFile, displayName, shortcutString;
		rngMask = rngDir + wxFileName::GetPathSeparator() + _T ( "*.rng" );
		rngFile = wxFindFirstFile ( rngMask, wxFILE );

		int id = ID_VALIDATE_PRESET1;

		if ( !rngFile.empty() )
		{
			validationPresetMap.insert ( make_pair ( id, rngFile ) );
			wxFileName::SplitPath ( rngFile, NULL, NULL, &displayName, NULL );
			displayName.Replace ( _T ( ".rng" ), _T ( "" ) );
			shortcutString.Printf ( _ ( "\tCtrl+%i" ), ( id - ID_VALIDATE_PRESET1 ) + 1 );

			validationMenu->Append ( id, displayName + shortcutString, displayName );

			for ( id = ID_VALIDATE_PRESET2; id <= ID_VALIDATE_PRESET9; ++id )
			{
				rngFile = wxFindNextFile();
				if ( rngFile.empty() )
					break;
				validationPresetMap.insert ( make_pair ( id, rngFile ) );
				wxFileName::SplitPath ( rngFile, NULL, NULL, &displayName, NULL );
				shortcutString.Printf ( _ ( "\tCtrl+%i" ), ( id - ID_VALIDATE_PRESET1 ) + 1 );
				displayName.Replace ( _T ( ".rng" ), _T ( "" ) );
				validationMenu->Append ( id, displayName + shortcutString, displayName );
			}
		}
	}

	// xsl menu
	/*
	wxMenu *xslMenu = new wxMenu;
	xslMenu->Append ( ID_XSLT, _ ( "&XSL Transform...\tF8" ),
	                  _ ( "XSL Transform..." ) );
	xslMenu->AppendSeparator();
	xslMenu->Append (
	    ID_XSLT_DOCBOOK_HTML,
	    _ ( "&DocBook to HTML\tAlt+1" ), _ ( "DocBook to HTML" ) );
	xslMenu->Append (
	    ID_XSLT_DOCBOOK_XHTML,
	    _ ( "&DocBook to XHTML\tAlt+2" ), _ ( "DocBook to XHTML" ) );
	xslMenu->Append (
	    ID_XSLT_DOCBOOK_FO,
	    _ ( "D&ocBook to XSL-FO\tAlt+3" ), _ ( "DocBook to XSL-FO" ) );
	xslMenu->Append (
	    ID_XSLT_TEI_HTML,
	    _ ( "&TEI to HTML\tAlt+4" ), _ ( "TEI to HTML" ) );
	xslMenu->Append (
	    ID_XSLT_TEI_LATEX,
	    _ ( "T&EI to LaTeX\tAlt+5" ), _ ( "TEI to LaTeX" ) );
	xslMenu->Append (
	    ID_XSLT_TEI_XHTML,
	    _ ( "TE&I to XHTML\tAlt+6" ), _ ( "TEI to XHTML" ) );
	xslMenu->Append (
	    ID_XSLT_TEI_FO,
	    _ ( "TEI to &XSL-FO\tAlt+7" ), _ ( "TEI to XSL-FO" ) );
	*/

	// xml menu
	xmlMenu = new wxMenu; // use class-wide data member
	xmlMenu->Append (
	    ID_CHECK_WELLFORMED,
	    _ ( "&Check Well-formedness\tF2" ), _ ( "Check Well-formedness" ) );
	xmlMenu->Append (
	    wxID_ANY,
	    _ ( "&Validate" ),
	    validationMenu );
	xmlMenu->Append ( ID_CREATE_SCHEMA, _ ( "Create &Schema...\tF10" ),
	    _ ( "Create schema..." ) );
	xmlMenu->Append ( ID_DTD_TO_SCHEMA, _ ( "DTD -> Schema..." ),
	    _ ( "DTD -> Schema..." ) );
	xmlMenu->AppendSeparator();
	xmlMenu->Append (
	    wxID_ANY,
	    _ ( "&Associate" ),
	    associateMenu );
	xmlMenu->AppendSeparator();
	xmlMenu->Append ( ID_XSLT, _ ( "&XSL Transform...\tF8" ),
	                  _ ( "XSL Transform..." ) );
	xmlMenu->Append (
	    ID_XPATH,
	    _ ( "&Evaluate XPath...\tF9" ),
	    _ ( "Evaluate XPath..." ) );

	xmlMenu->AppendSeparator();
	xmlMenu->Append (
	    ID_PRETTYPRINT,
	    _ ( "&Pretty-print\tF11" ), _ ( "Pretty-print" ) );
	xmlMenu->AppendSeparator();
	xmlMenu->AppendCheckItem (
	    ID_PROTECT_TAGS,
	    _ ( "&Lock Tags\tCtrl+L" ),
	    _ ( "Lock Tags" ) );
	xmlMenu->Check ( ID_PROTECT_TAGS, protectTags );
	xmlMenu->AppendSeparator();
	xmlMenu->Append (
	    ID_ENCODING,
	    _ ( "E&ncoding..." ), _ ( "Encoding..." ) );

	// tools menu
	wxMenu *toolsMenu = new wxMenu;

	wxMenuItem *spellingItem =
	    new wxMenuItem (
	    toolsMenu,
	    ID_SPELL,
	    _ ( "&Spelling...\tF7" ),
	    _ ( "Spelling..." ) );
	spellingItem->SetBitmap ( spelling16Bitmap );

	wxMenuItem *styleItem = 
	    new wxMenuItem (
	    toolsMenu,
	    ID_STYLE,
	    _ ( "&Style...\tShift+F7" ),
	    _ ( "Style..." ) );
	styleItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *wordCountItem =
	    new wxMenuItem (
	    toolsMenu,
	    ID_WORD_COUNT,
	    _ ( "&Word Count" ),
	    _ ( "Word Count" ) );
	wordCountItem->SetBitmap ( wxNullBitmap );


	wxMenuItem *commandItem =
	    new wxMenuItem (
	    toolsMenu,
	    ID_COMMAND,
	    _ ( "&Command\tCtrl+Alt+C" ),
	    _ ( "Command" ) );
	commandItem->SetBitmap ( wxNullBitmap );

	toolsMenu->Append ( spellingItem );
	toolsMenu->Append ( styleItem );
	toolsMenu->Append ( wordCountItem );
	toolsMenu->AppendSeparator();
	toolsMenu->Append ( commandItem );

#ifdef __WXMSW__
	toolsMenu->AppendSeparator();
	wxMenuItem *optionsItem =
	    new wxMenuItem (
	    toolsMenu,
	    ID_OPTIONS,
	    _ ( "&Options..." ),
	    _ ( "Options..." ) );
	optionsItem->SetBitmap ( wxNullBitmap );
	toolsMenu->Append ( optionsItem );
#endif

	// help menu
	wxMenu *helpMenu = new wxMenu;

	wxMenuItem *helpItem =
	    new wxMenuItem ( helpMenu, wxID_HELP,
	                     _ ( "&XML Copy Editor Help\tF1" ), _ ( "Help" ) );
	helpItem->SetBitmap ( helpBitmap );


	wxMenuItem *homeItem =
	    new wxMenuItem ( helpMenu, ID_HOME,
	                     _ ( "&Home Page" ), _ ( "Home Page" ) );
	homeItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *feedbackItem =
	    new wxMenuItem ( helpMenu, ID_FEEDBACK, _ ( "&Forum" ), _ ( "Forum" ) );
	feedbackItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *aboutItem =
	    new wxMenuItem ( helpMenu, wxID_ABOUT,
	                     _ ( "&About XML Copy Editor" ), _ ( "About" ) );
	aboutItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *downloadSourceItem =
	    new wxMenuItem ( helpMenu, ID_DOWNLOAD_SOURCE,
	                     _ ( "&Browse Source" ), _ ( "Browse Source" ) );
	downloadSourceItem->SetBitmap ( wxNullBitmap );
	helpMenu->Append ( helpItem );

	helpMenu->AppendSeparator();
	helpMenu->Append ( homeItem );
	helpMenu->Append ( feedbackItem );
	helpMenu->Append ( downloadSourceItem );
	helpMenu->AppendSeparator();
	helpMenu->Append ( aboutItem );

	wxMenuBar *menuBar = new wxMenuBar ( wxMB_DOCKABLE );
	menuBar->Append ( fileMenu, _ ( "&File" ) );
	menuBar->Append ( editMenu, _ ( "&Edit" ) );
	menuBar->Append ( viewMenu, _ ( "&View" ) );
	menuBar->Append ( insertMenu, _ ( "&Insert" ) );
	menuBar->Append ( xmlMenu, _ ( "&XML" ) );
	menuBar->Append ( toolsMenu, _ ( "&Tools" ) );
	menuBar->Append ( helpMenu, _ ( "&Help" ) );
	return menuBar;
}

void MyFrame::updateFileMenu ( bool deleteExisting )
{
	if ( deleteExisting )
	{
		wxMenuItemList list = fileMenu->GetMenuItems();
		size_t count = list.size();
		for ( size_t i = 0; i < count; ++i )
			fileMenu->Delete ( list[i] );
	}

	wxMenuItem *newItem =
	    new wxMenuItem ( fileMenu, wxID_NEW, _ ( "&New...\tCtrl+N" ), _ ( "New..." ) );
	newItem->SetBitmap ( new16Bitmap );
	wxMenuItem *openItem =
	    new wxMenuItem ( fileMenu, wxID_OPEN, _ ( "&Open...\tCtrl+O" ), _ ( "Open..." ) );
	openItem->SetBitmap ( open16Bitmap );
	wxMenuItem *openLargeFileItem =
	    new wxMenuItem ( fileMenu, ID_OPEN_LARGE_FILE,
	                     _ ( "O&pen Large Document...\tCtrl+Shift+O" ), _ ( "Open Large Document..." ) );
	openLargeFileItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *closeItem =
	    new wxMenuItem ( fileMenu, wxID_CLOSE, _ ( "&Close\tCtrl+F4" ), _ ( "Close" ) );
	closeItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *closeAllItem =
	    new wxMenuItem ( fileMenu, wxID_CLOSE_ALL, _ ( "C&lose All" ), _ ( "Close All" ) );
	closeAllItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *saveItem =
	    new wxMenuItem ( fileMenu, wxID_SAVE, _ ( "&Save\tCtrl+S" ), _ ( "Save" ) );
	saveItem->SetBitmap ( save16Bitmap );
	wxMenuItem *saveAsItem =
	    new wxMenuItem ( fileMenu, wxID_SAVEAS, _ ( "S&ave As...\tF12" ), _ ( "Save As..." ) );
	saveAsItem->SetBitmap ( wxNullBitmap );
#ifdef __WXMSW__
	wxMenuItem *exportItem =
        new wxMenuItem ( fileMenu, ID_EXPORT, _ ( "&DAISY Export..." ), _ ( "DAISY Export..." ) );
    exportItem->SetBitmap ( wxNullBitmap );
#endif
	wxMenuItem *reloadItem =
	    new wxMenuItem ( fileMenu, ID_RELOAD, _ ( "&Reload" ), _ ( "Reload" ) );
	reloadItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *revertItem =
	    new wxMenuItem ( fileMenu, wxID_REVERT, _ ( "&Revert" ), _ ( "Revert" ) );
	revertItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *printSetupItem =
	    new wxMenuItem ( fileMenu, ID_PRINT_SETUP, _ ( "Pa&ge Setup..." ), _ ( "Page Setup..." ) );
	printSetupItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *printPreviewItem =
	    new wxMenuItem ( fileMenu, ID_PRINT_PREVIEW, _ ( "Pr&int Preview..." ), _ ( "Print Preview..." ) );
	printPreviewItem->SetBitmap ( printPreviewBitmap );
	wxMenuItem *printItem =
	    new wxMenuItem ( fileMenu, ID_PRINT, _ ( "Pri&nt...\tCtrl+P" ), _ ( "Print..." ) );
	printItem->SetBitmap ( print16Bitmap );
	wxMenuItem *importMSWordItem =
	    new wxMenuItem (
	    fileMenu, ID_IMPORT_MSWORD, _ ( "I&mport Microsoft Word Document..." ) );
	importMSWordItem->SetBitmap ( wxNullBitmap );
	wxMenuItem *exportMSWordItem =
	    new wxMenuItem (
	    fileMenu, ID_EXPORT_MSWORD, _ ( "Expor&t Microsoft Word Document..." ) );
	exportMSWordItem->SetBitmap ( wxNullBitmap );

	wxMenuItem *exitItem =
	    new wxMenuItem ( fileMenu, wxID_EXIT, _ ( "E&xit" ), _ ( "Exit" ) );
	exitItem->SetBitmap ( wxNullBitmap );

	fileMenu->Append ( newItem );
	fileMenu->Append ( openItem );
	fileMenu->Append ( openLargeFileItem );
	fileMenu->AppendSeparator();
	fileMenu->Append ( closeItem );
	fileMenu->Append ( closeAllItem );
	fileMenu->Append ( saveItem );
	fileMenu->Append ( saveAsItem );
    fileMenu->AppendSeparator();
	fileMenu->Append ( reloadItem );
	fileMenu->Append ( revertItem );
	fileMenu->AppendSeparator();
	fileMenu->Append ( printSetupItem );
	fileMenu->Append ( printPreviewItem );
	fileMenu->Append ( printItem );
#ifdef __WXMSW__
	fileMenu->AppendSeparator();
	fileMenu->Append ( exportItem );
	fileMenu->Append ( importMSWordItem );
	fileMenu->Append ( exportMSWordItem );
#endif
	history.AddFilesToMenu ( fileMenu );

	fileMenu->AppendSeparator();
	fileMenu->Append ( exitItem );
}

wxToolBar *MyFrame::getToolBar()
{
	wxToolBar *toolBar = new wxToolBar (
	    this,
	    ID_TOOLBAR,
	    wxDefaultPosition,
	    wxDefaultSize,
	    wxTB_FLAT |
	    wxTB_HORIZONTAL |
	    wxTB_DOCKABLE );
	int w, h;
	w = saveBitmap.GetWidth(), h = saveBitmap.GetHeight();
	toolBar->SetToolBitmapSize ( wxSize ( w, h ) );

	toolBar->AddTool (
	    wxID_NEW,
	    _ ( "New" ),
	    newBitmap,
	    _ ( "New" ) );
	toolBar->AddTool (
	    wxID_OPEN,
	    _ ( "Open" ),
	    openBitmap,
	    _ ( "Open" ) );
	toolBar->AddTool (
	    wxID_SAVE,
	    _ ( "Save" ),
	    saveBitmap,
	    wxNullBitmap,
	    wxITEM_NORMAL,
	    _ ( "Save" ) );
	toolBar->AddTool (
	    ID_PRINT,
	    _ ( "Print" ),
	    printBitmap,
	    wxNullBitmap,
	    wxITEM_NORMAL,
	    _ ( "Print" ) );
	toolBar->AddTool (
	    ID_CHECK_WELLFORMED,
	    _ ( "Check Well-formedness" ),
	    checkWellformedBitmap,
	    wxNullBitmap,
	    wxITEM_NORMAL,
	    _ ( "Check Well-formedness" ) );
	toolBar->AddTool (
	    ID_VALIDATE_W3C_SCHEMA,
	    _ ( "Validate" ),
	    checkValidBitmap,
	    wxNullBitmap,
	    wxITEM_NORMAL,
	    _ ( "Validate" ) );
	toolBar->AddTool (
	    ID_BROWSER,
	    _ ( "Browser" ),
	    internetBitmap,
	    wxNullBitmap,
	    wxITEM_NORMAL,
	    _ ( "Browser" ) );
	toolBar->AddTool (
	    ID_SPELL,
	    _ ( "Spelling" ),
	    spellingBitmap,
	    wxNullBitmap,
	    wxITEM_NORMAL,
	    _ ( "Spelling" ) );

	toolBar->AddCheckTool (
	    ID_PROTECT_TAGS,
	    _ ( "Lock Tags" ),
	    hyperlinkBitmap,
	    wxNullBitmap,
	    _ ( "Lock Tags" ) );
	toolBar->ToggleTool (
	    ID_PROTECT_TAGS, protectTags );

	toolBar->Realize();
	return toolBar;
}

XmlDoc *MyFrame::getActiveDocument()
{
	if ( !mainBook->GetPageCount() )
		return NULL;
	return ( XmlDoc * ) mainBook->GetPage ( mainBook->GetSelection() );
}

void MyFrame::addSafeSeparator ( wxToolBar *toolBar )
{
	wxStaticText *staticControl = new wxStaticText (
	    toolBar,
	    wxID_ANY,
	    _T ( "  " ) );
	toolBar->AddControl ( staticControl );
}

void MyFrame::statusProgress ( const wxString& s )
{
	wxStatusBar *status = GetStatusBar();
	if ( !status )
		return;
	status->SetStatusText ( s, 0 );
}

void MyFrame::messagePane ( const wxString& s, int iconType, bool forcePane )
{
	statusProgress ( wxEmptyString );
	wxString paneTitle;
	switch ( iconType )
	{
		case ( CONST_INFO ) :
			paneTitle = _ ( "Information" );
			break;
		case ( CONST_WARNING ) :
			paneTitle = _ ( "Warning" );
			break;
		case ( CONST_STOP ) :
			paneTitle = _ ( "Stopped" );
			break;
		case ( CONST_QUESTION ) :
			paneTitle = _ ( "Question" );
			break;
		default:
			paneTitle = _ ( "Message" );
			break;
	}

	wxAuiPaneInfo &info = manager.GetPane ( htmlReport );
	if ( !info.IsShown() )
	{
		info.Show ( true );
	}

	info.Caption ( paneTitle );

	wxString htmlString = s;
	htmlString.Replace ( _T ( "&" ), _T ( "&amp;" ), true );
	htmlString.Replace ( _T ( "<" ), _T ( "&lt;" ), true );
	htmlString.Replace ( _T ( ">" ), _T ( "&gt;" ), true );

	// have to use <br> on Ubuntu
	htmlString.Replace ( _T("[br]"), _T("<br>"), true );
	htmlString.Replace ( _T("[br/]"), _T("<br/>"), true );
	htmlString.Replace ( _T("[b]"), _T("<b>"), true );
	htmlString.Replace ( _T("[/b]"), _T("</b>"), true );
	htmlString.Replace ( _T("[i]"), _T("<i>"), true );
	htmlString.Replace ( _T("[/i]"), _T("</i>"), true );

	wxString htmlBuffer;
	htmlBuffer += _T ( "<html><body><table><tr><td width=\"5%\"><img src=\"" );
	switch ( iconType )
	{
		case ( CONST_INFO ) :
			htmlBuffer += pngDir;
			htmlBuffer += _T ( "stock_dialog-info-32.png" );
			break;
		case ( CONST_WARNING ) :
						htmlBuffer += pngDir;
			htmlBuffer += _T ( "stock_dialog-warning-32.png" );
			break;
		case ( CONST_STOP ) :
						htmlBuffer += pngDir;
			htmlBuffer += _T ( "stock_dialog-stop-32.png" );
			break;
		case ( CONST_QUESTION ) :
						htmlBuffer += pngDir;
			htmlBuffer += _T ( "stock_dialog-question-32.png" );
			break;
		default:
			break;
	}
	htmlBuffer += _T ( "\"></td><td width=\"95%\">" );
	htmlBuffer += htmlString;
	htmlBuffer += _T ( "</td></tr></table></body></html>" );

	htmlReport->SetPage ( htmlBuffer );

	manager.Update();
}

void MyFrame::documentOk ( const wxString& status )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;
	wxString message;
	message.Printf ( _ ( "%s is %s" ),
	                 doc->getShortFileName().c_str(),
	                 status.c_str() );
	messagePane ( message, CONST_INFO );
}

void MyFrame::applyEditorProperties ( bool zoomOnly )
{
	XmlDoc *doc;
	size_t documentCount = mainBook->GetPageCount();
	for ( size_t i = 0; i < documentCount; i++ )
	{
		doc = ( XmlDoc * ) mainBook->GetPage ( i );
		if ( doc )
		{
			doc->applyProperties ( properties, zoomOnly );
			if ( !properties.validateAsYouType )
				doc->clearErrorIndicators();
		}
	}
}

void MyFrame::modifiedMessage()
{
	messagePane (
	    _ ( "Document has been modified: save or discard changes" ),
	    CONST_STOP );
}

void MyFrame::xmliseWideTextNode ( wxString& s )
{
	s.Replace ( _T ( "&" ), _T ( "&amp;" ), true );
	s.Replace ( _T ( "<" ), _T ( "&lt;" ), true );
	s.Replace ( _T ( ">" ), _T ( "&gt;" ), true );
}

int MyFrame::getFileType ( const wxString& fileName )
{
	wxString extension;
	wxFileName::SplitPath ( fileName, NULL/*Path*/, NULL/*Name*/, &extension );
	if ( extension.size() != 3 )
		return FILE_TYPE_XML;

	extension.MakeLower();

	if ( extension == _T ( "dtd" ) || extension == _T ( "ent" ) )
		return FILE_TYPE_DTD;
	else if ( extension == _T ( "css" ) )
		return FILE_TYPE_CSS;
	else if ( extension == _T ( "php" ) )
		return FILE_TYPE_PHP;
	else if ( extension == _T ( "exe" ) )
		return FILE_TYPE_BINARY;
	else if ( extension == _T ( "rnc" ) )
		return FILE_TYPE_RNC;
	return FILE_TYPE_XML;
}

long MyFrame::getNotebookStyleMask()
{
	/*
	if (notebookStyleMenu->IsChecked(ID_NOTEBOOK_STYLE_FLAT))
	  return wxFNB_FANCY_TABS | wxFNB_MOUSE_MIDDLE_CLOSES_TABS | wxFNB_X_ON_TAB;
	else if (notebookStyleMenu->IsChecked(ID_NOTEBOOK_STYLE_VC8))
	  return wxFNB_BACKGROUND_GRADIENT | wxFNB_VC8 | wxFNB_MOUSE_MIDDLE_CLOSES_TABS |
	    wxFNB_X_ON_TAB | wxFNB_DROPDOWN_TABS_LIST | wxFNB_NO_NAV_BUTTONS;
	else
	  return wxFNB_BACKGROUND_GRADIENT | wxFNB_VC8 | wxFNB_MOUSE_MIDDLE_CLOSES_TABS | wxFNB_COLORFUL_TABS |
	    wxFNB_X_ON_TAB | wxFNB_DROPDOWN_TABS_LIST | wxFNB_NO_NAV_BUTTONS;
	  //wxFNB_BACKGROUND_GRADIENT | wxFNB_VC8 | wxFNB_MOUSE_MIDDLE_CLOSES_TABS | wxFNB_COLORFUL_TABS;
	*/
	return 0;
}

bool MyFrame::isSpecialFileType ( const wxString& fileName )
{
	std::string fileNameLocal, fileNameLocalLC;
	fileNameLocal = fileName.mb_str ( wxConvLocal );
	fileNameLocalLC = CaseHandler::lowerCase ( fileNameLocal );

	return (
	           fileNameLocalLC.find ( ".dtd" ) != std::string::npos ||
	           fileNameLocalLC.find ( ".css" ) != std::string::npos ||
	           fileNameLocalLC.find ( ".php" ) != std::string::npos );
}

void MyFrame::encodingMessage()
{
	wxString msg = _ ( "Encoding should be one of " );
	msg += ENCODING_INFO;
	messagePane ( msg, CONST_STOP );
}

void MyFrame::updatePaths()
{
	ruleSetDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "rulesets" );
	filterDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "filters" );
	templateDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "templates" ) +
	              wxFileName::GetPathSeparator();
	binDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "bin" ) +
	         wxFileName::GetPathSeparator();
	helpDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "help" ) +
	          wxFileName::GetPathSeparator();
	rngDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "rng" ) +
	         wxFileName::GetPathSeparator();
	htmlDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "html" ) +
	          wxFileName::GetPathSeparator();
	pngDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "png" ) +
	         wxFileName::GetPathSeparator();
    daisyDir = applicationDir + wxFileName::GetPathSeparator() + _T ( "daisy" ) +
        wxFileName::GetPathSeparator();
	catalogPath =
	    applicationDir + wxFileName::GetPathSeparator() + _T ( "catalog" ) +
	    wxFileName::GetPathSeparator() + _T ( "catalog" );
	xslDtdPath =
	    applicationDir + wxFileName::GetPathSeparator() + _T ( "dtd" ) +
	    wxFileName::GetPathSeparator() + _T ( "xslt10.dtd" );
	rssDtdPath =
	    applicationDir + wxFileName::GetPathSeparator() + _T ( "dtd" ) +
	    wxFileName::GetPathSeparator() + _T ( "rss2.dtd" );
	xtmDtdPath =
	    applicationDir + wxFileName::GetPathSeparator() + _T ( "dtd" ) +
	    wxFileName::GetPathSeparator() + _T ( "xtm1.dtd" );
	lzxDtdPath =
	    applicationDir + wxFileName::GetPathSeparator() + _T ( "dtd" ) +
	    wxFileName::GetPathSeparator() + _T ( "lzx.dtd" );
	xliffDtdPath =
	    applicationDir + wxFileName::GetPathSeparator() + _T ( "dtd" ) +
	    wxFileName::GetPathSeparator() + _T ( "xliff.dtd" );
	aspellDataPath = applicationDir + wxFileName::GetPathSeparator() +
        _T ( "aspell" ) + wxFileName::GetPathSeparator() + _T ( "data" );
    aspellDictPath = applicationDir + wxFileName::GetPathSeparator() +
        _T ( "aspell" ) + wxFileName::GetPathSeparator() + _T ( "dict" );
}

void MyFrame::OnAssociate ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	wxString title, label, type, extension, path, defaulturl, defaultaux;
	wxString auxiliaryLabel;
	int id = event.GetId();
	switch ( id )
	{
		case ID_ASSOCIATE_DTD_PUBLIC:
			type = _ ( "Public DTD" );
			extension = _T ( "*.dtd" );
			defaulturl = lastDtdPublic;
			defaultaux = lastDtdPublicAux;
			break;
		case ID_ASSOCIATE_DTD_SYSTEM:
			type = _ ( "System DTD" );
			extension = _T ( "*.dtd" );
			defaulturl = lastDtdSystem;
			defaultaux = _T ( "" );
			break;
		case ID_ASSOCIATE_W3C_SCHEMA:
			type = _ ( "XML Schema" );
			extension = _T ( "*.xsd" );
			defaulturl = lastSchema;
			defaultaux = _T ( "" );
			break;
		case ID_ASSOCIATE_XSL:
			type = _ ( "XSLT stylesheet" );
			extension = _T ( "*.xsl;*.xslt" );
			defaulturl = lastXslStylesheet;
			defaultaux = _T ( "" );
			break;
		default:
			return;
	}

	std::string utf8Buffer;
	getRawText ( doc, utf8Buffer );
	std::string origEncoding = XmlEncodingHandler::get ( utf8Buffer );
	WrapExpat wellformedparser ( "UTF-8" );
	if ( !wellformedparser.parse ( utf8Buffer ) )
	{
		wxString message;
		message.Printf (
		    _ ( "Cannot associate %s: %s" ),
		    type.c_str(),
		    wellformedparser.getLastError().c_str() );
		messagePane ( message, CONST_STOP );
		return;
	}

	title.Printf ( _ ( "Associate %s" ), type.c_str() );
	label = _ ( "Choose a file:" );

	bool auxiliaryBox =
	    ( id == ID_ASSOCIATE_DTD_PUBLIC );
	//(id == ID_ASSOCIATE_W3C_SCHEMA_NS || id == ID_ASSOCIATE_DTD_PUBLIC);
	if ( auxiliaryBox )
	{
		auxiliaryLabel = _ ( "Choose a public identifier:" );
		/*
		      (id == ID_ASSOCIATE_DTD_PUBLIC) ? _("Choose a public identifier:") :
		        _("Choose a namespace:");
		*/
	}

	AssociateDialog ad (
	    this,
	    title,
	    label,
	    type,
	    extension,
	    defaulturl,
	    &mLastDir,
	    auxiliaryBox,
	    auxiliaryLabel,
	    defaultaux );
	if ( ad.ShowModal() != wxID_OK )
		return;

	path = ad.getUrl();

	wxString aux, schemaPathMemory;

	if ( auxiliaryBox )
	{
		aux = ad.getAux();
	}

	std::string modifiedBuffer;

	// remember choice
	switch ( id )
	{
		case ID_ASSOCIATE_W3C_SCHEMA:
			lastSchema = path;
			break;
		case ID_ASSOCIATE_DTD_PUBLIC:
			lastDtdPublic = path;
			lastDtdPublicAux = aux;
			break;
		case ID_ASSOCIATE_DTD_SYSTEM:
			lastDtdSystem = path;
			break;
		case ID_ASSOCIATE_XSL:
			lastXslStylesheet = path;
			break;
		default:
			break;
	}

	if ( id == ID_ASSOCIATE_W3C_SCHEMA )
	{
		XmlAssociateXsd parser ( path, "UTF-8" );
		if ( !parser.parse ( utf8Buffer ) )
			return;
		modifiedBuffer = parser.getBuffer();
	}
	else if ( id == ID_ASSOCIATE_DTD_SYSTEM || id == ID_ASSOCIATE_DTD_PUBLIC )
	{
		XmlAssociateDtd parser ( path, aux, "UTF-8" );
		if ( !parser.parse ( utf8Buffer ) )
			return;
		modifiedBuffer = parser.getBuffer();
	}
	else if ( id == ID_ASSOCIATE_XSL )
	{
		XmlAssociateXsl parser( path, "UTF-8" );
		if ( !parser.parse ( utf8Buffer ) )
			return;
		modifiedBuffer = parser.getBuffer();
	}
	else
		return;

	XmlEncodingHandler::set ( modifiedBuffer, origEncoding );
	doc->SetTextRaw ( modifiedBuffer.c_str() );
	doc->SetFocus();
}

void MyFrame::openRememberedTabs()
{
	wxStringTokenizer files ( openTabsOnClose, _T ( "|" ) );
	while ( files.HasMoreTokens() )
	{
		wxString file = files.GetNextToken();
		if ( file.IsEmpty() || !openFile ( file ) )
			continue; //break; // Ignore errors
	}

	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) != NULL )
		doc->SetFocus();
}

void MyFrame::getRawText ( XmlDoc *doc, std::string& buffer )
{
	if ( !doc )
	{
		buffer = "";
		return;
	}
	buffer = doc->myGetTextRaw();
}

void MyFrame::OnWordCount ( wxCommandEvent& event )
{
	XmlDoc *doc;
	if ( ( doc = getActiveDocument() ) == NULL )
		return;

	std::string rawBufferUtf8;
	getRawText ( doc, rawBufferUtf8 );

	XmlWordCount xwc ( "UTF-8" );
	wxString msg;
	if ( !xwc.parse ( rawBufferUtf8 ) )
	{
		statusProgress ( wxEmptyString );
		msg.Printf ( _ ( "Cannot count words: %s" ), xwc.getLastError().c_str() );
		messagePane ( msg, CONST_STOP );
		return;
	}

	int count = xwc.getWordCount();

	msg.Printf (
	    wxPLURAL ( "%s contains %i word", "%s contains %i words", count ),
	    doc->getShortFileName().c_str(), count );

	messagePane ( msg, CONST_INFO, true );
	doc->SetFocus();
}

void MyFrame::removeUtf8Bom ( std::string& buffer )
{
	if ( buffer.size() > 3 &&
	        ( unsigned char ) buffer[0] == 0xEF &&
	        ( unsigned char ) buffer[1] == 0xBB &&
	        ( unsigned char ) buffer[2] == 0xBF )
	{
		buffer.erase ( 0, 3 );
	}
}

void MyFrame::loadBitmaps()
{
#ifdef __WXMSW__
	// toolbar icons
	newBitmap = wxBITMAP ( stock_new );
	openBitmap = wxBITMAP ( stock_open );
	saveBitmap = wxBITMAP ( stock_save );
	printBitmap = wxBITMAP ( stock_print );
	spellingBitmap = wxBITMAP ( stock_spellcheck );
	internetBitmap = wxBITMAP ( stock_internet );
	hyperlinkBitmap = wxBITMAP ( stock_hyperlink );
	filtersBitmap = wxBITMAP ( stock_filters );
	checkWellformedBitmap = wxBITMAP ( stock_calc-accept );
	checkValidBitmap = wxBITMAP ( stock_calc-accept-green );

	// menu icons
	new16Bitmap = wxBITMAP ( stock_new_16 );
	open16Bitmap = wxBITMAP ( stock_open_16 );
	save16Bitmap = wxBITMAP ( stock_save_16 );
	printPreviewBitmap = wxBITMAP ( stock_print_preview_16 );
	print16Bitmap = wxBITMAP ( stock_print_16 );
	undo16Bitmap = wxBITMAP ( stock_undo_16 );
	redo16Bitmap = wxBITMAP ( stock_redo_16 );
	cutBitmap = wxBITMAP ( stock_cut_16 );
	copyBitmap = wxBITMAP ( stock_copy_16 );
	pasteBitmap = wxBITMAP ( stock_paste_16 );
	findBitmap = wxBITMAP ( stock_search_16 );
	spelling16Bitmap = wxBITMAP ( stock_spellcheck_16 );
	helpBitmap = wxBITMAP ( stock_help_16 );
#else
	// toolbar icons
	newBitmap = wxArtProvider::GetBitmap ( wxART_NEW, wxART_TOOLBAR );
	openBitmap = wxArtProvider::GetBitmap ( wxART_FILE_OPEN, wxART_TOOLBAR );
	saveBitmap = wxArtProvider::GetBitmap ( wxART_FILE_SAVE, wxART_TOOLBAR );
	printBitmap = wxArtProvider::GetBitmap ( wxART_PRINT, wxART_TOOLBAR );
	spellingBitmap = wxArtProvider::GetBitmap ( _T ( "gtk-spell-check" ), wxART_TOOLBAR );

	// no stock icons for the following
	internetBitmap.LoadFile ( pngDir + _T ( "stock_internet.png" ), wxBITMAP_TYPE_PNG );
	hyperlinkBitmap.LoadFile ( pngDir + _T ( "stock_hyperlink.png" ), wxBITMAP_TYPE_PNG );
	checkWellformedBitmap.LoadFile ( pngDir + _T ( "stock_calc-accept.png" ), wxBITMAP_TYPE_PNG );
	checkValidBitmap.LoadFile ( pngDir + _T ( "stock_calc-accept-green.png" ), wxBITMAP_TYPE_PNG );

	// menu icons
	new16Bitmap = wxNullBitmap;
	open16Bitmap = wxNullBitmap;
	save16Bitmap = wxNullBitmap;
	printPreviewBitmap = wxNullBitmap;
	print16Bitmap = wxNullBitmap;
	undo16Bitmap = wxNullBitmap;
	redo16Bitmap = wxNullBitmap;
	cutBitmap = wxNullBitmap;
	copyBitmap = wxNullBitmap;
	pasteBitmap = wxNullBitmap;
	findBitmap = wxNullBitmap;
	spelling16Bitmap = wxNullBitmap;
	helpBitmap = wxNullBitmap;
#endif
}

#ifdef __WXMSW__
void MyFrame::OnDropFiles ( wxDropFilesEvent& event )
{
	int no = event.GetNumberOfFiles();
	wxString *iterator = event.GetFiles();

	if ( !no || !iterator )
		return;

	for ( int i = 0; i < no; i++, iterator++ )
	{
		if ( !openFile ( *iterator ) )
			break;
	}
}
#endif

void MyFrame::OnPromptGenerated ( wxNotifyEvent &event )
{
	XmlDoc *doc = this->getActiveDocument();
	locationPanel->update ( doc, lastParent );
	insertChildPanel->update ( doc, lastParent, wxEmptyString, true );
	insertSiblingPanel->update ( doc, lastParent, wxEmptyString, true );
	insertEntityPanel->update ( doc, wxEmptyString, wxEmptyString, true );
}

wxString MyFrame::getAuxPath ( const wxString& fileName )
{
	if ( fileName.Find ( _T ( ".xsl" ) ) != wxNOT_FOUND ||
	        fileName.Find ( _T ( ".XSL" ) ) != wxNOT_FOUND )
		return xslDtdPath;
	else if ( fileName.Find ( _T ( ".rss" ) ) != wxNOT_FOUND ||
	          fileName.Find ( _T ( ".RSS" ) ) != wxNOT_FOUND )
		return rssDtdPath;
	else if ( fileName.Find ( _T ( ".xtm" ) ) != wxNOT_FOUND ||
	          fileName.Find ( _T ( ".xtmm" ) ) != wxNOT_FOUND ||
	          fileName.Find ( _T ( ".XTM" ) ) != wxNOT_FOUND ||
	          fileName.Find ( _T ( ".XTMM" ) ) != wxNOT_FOUND )
		return xtmDtdPath;
	else if ( fileName.Find ( _T ( ".lzx" ) ) != wxNOT_FOUND ||
	          fileName.Find ( _T ( ".LZX" ) ) != wxNOT_FOUND )
		return lzxDtdPath;
	else if ( fileName.Find ( _T ( ".xlf" ) ) != wxNOT_FOUND ||
	          fileName.Find ( _T ( ".XLF" ) ) != wxNOT_FOUND )
		return xliffDtdPath;
	return wxEmptyString;
}

void MyFrame::OnActivateApp ( wxActivateEvent& event )
{
	event.Skip();
	if ( !mainBook || !event.GetActive() )
		return;
	restoreFocusToNotebook = true;
}

void MyFrame::OnIconize ( wxIconizeEvent& event )
{
	event.Skip();
#if wxCHECK_VERSION(2,9,0)
	if (event.IsIconized())
#else
	if ( event.Iconized() )
#endif
		return;
	restoreFocusToNotebook = true;
}

void MyFrame::OnKeyPressed ( wxKeyEvent& event )
{
	event.Skip();
}

void MyFrame::setStrictScrolling ( bool b )
{
	XmlDoc *doc;
	doc = getActiveDocument();
	if ( !doc )
		return;
	doc->SetYCaretPolicy ( ( b ) ? ( wxSTC_CARET_STRICT | wxSTC_CARET_SLOP ) : wxSTC_CARET_EVEN,
	                       ( b ) ? 10 : 0 );
}

void MyFrame::addToFileQueue ( wxString& fileName )
{
     fileQueue.push_back ( fileName );
}

void MyFrame::validatePaths()
{
	int invalid = 0;
	wxString msg;

	// Warning: Don't put a space between 'CHECK' and '('
#define CHECK( check, path ) \
	if ( !( check ) ( path ) )\
	{\
		invalid++;\
		msg << wxTextFile::GetEOL() << path;\
	}
	CHECK ( wxDirExists, ruleSetDir );
	//CHECK ( wxDirExists, filterDir );
	CHECK ( wxDirExists, templateDir );
	//CHECK ( wxDirExists, binDir );
	CHECK ( wxDirExists, helpDir );
	CHECK ( wxDirExists, rngDir );
	//CHECK ( wxDirExists, htmlDir );
	CHECK ( wxDirExists, pngDir );
	//CHECK ( wxDirExists, daisyDir );
	CHECK ( wxFileExists, catalogPath );
	CHECK ( wxFileExists, xslDtdPath );
	CHECK ( wxFileExists, rssDtdPath );
	CHECK ( wxFileExists, xtmDtdPath );
	CHECK ( wxFileExists, lzxDtdPath );
	CHECK ( wxFileExists, xliffDtdPath );
#ifdef __WXMSW__
	CHECK ( wxDirExists, aspellDataPath );
	CHECK ( wxDirExists, aspellDictPath );
#endif // __WXMSW__
#undef CHECK

	if ( !invalid )
		return;

	msg = wxPLURAL ( "Invalid path: ", "Invalid paths: ", invalid ) + msg;
	msg << wxTextFile::GetEOL()
		<< wxTextFile::GetEOL()
#ifdef __WXMSW__
		<< _ ( "To change application directory, see Tools, Options..., General" );
#else
		<< _ ( "To change application directory, see Edit, Preferences..., General" );
#endif
	wxMessageBox ( msg, GetTitle(), wxOK | wxICON_ERROR, this );
}
