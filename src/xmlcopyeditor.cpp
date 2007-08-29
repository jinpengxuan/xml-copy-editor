#include <iostream>
#include <fstream>
#include <string>
#include <wx/aboutdlg.h>
#include "xmlcopyeditor.h"
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
#include "aboutdialog.h"
#include "pathresolver.h"
#include "locationpanel.h"
#include "insertpanel.h"
#include "xmlwordcount.h"
#include "mynotebook.h"
#include "getlinuxappdir.h"
#include "commandpanel.h"
#include "binaryfile.h"
#include <wx/aui/auibook.h>
#include <wx/richtext/richtextsymboldlg.h>
#include <wx/textctrl.h>
#include <iconv.h>

#define ngettext wxGetTranslation

#ifdef NEWFINDREPLACE
  #include "findreplacepanel.h"
#endif

#ifdef __WXMSW__
  #include <wx/msw/uxtheme.h>
#else
  #include "wrapxerces.h"
  #include "xpm/appicon.xpm"
#endif

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_ACTIVATE_APP(MyFrame::OnActivateApp)
  EVT_CLOSE(MyFrame::OnFrameClose)
  EVT_KEY_DOWN(MyFrame::OnKeyPressed)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_CLOSE, MyFrame::OnClose)
  EVT_MENU(wxID_CLOSE_ALL, MyFrame::OnCloseAll)
  EVT_MENU(wxID_CUT, MyFrame::OnCut)
  EVT_MENU(wxID_COPY, MyFrame::OnCopy)
	EVT_MENU(wxID_HELP, MyFrame::OnHelp)
  EVT_MENU(wxID_PASTE, MyFrame::OnPaste)
  EVT_MENU(ID_PASTE_NEW_DOCUMENT, MyFrame::OnPasteNewDocument)
  EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
  EVT_MENU(wxID_NEW, MyFrame::OnNew)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  EVT_MENU(wxID_SAVE, MyFrame::OnSave)
  EVT_MENU(wxID_SAVEAS, MyFrame::OnSaveAs)
  EVT_MENU(wxID_UNDO, MyFrame::OnUndo)
  EVT_MENU(wxID_REDO, MyFrame::OnRedo)
  EVT_MENU(wxID_REVERT, MyFrame::OnRevert)
  EVT_MENU(ID_INSERT_CHILD, MyFrame::OnInsertChild)
  EVT_MENU(ID_INSERT_SIBLING, MyFrame::OnInsertSibling)
  EVT_MENU(ID_INSERT_ENTITY, MyFrame::OnInsertEntity)
  EVT_MENU(ID_INSERT_TWIN, MyFrame::OnInsertTwin)
  EVT_MENU(ID_INSERT_SYMBOL, MyFrame::OnInsertSymbol)
  EVT_MENU(ID_TOGGLE_FOLD, MyFrame::OnToggleFold)
  EVT_MENU(ID_FOLD_ALL, MyFrame::OnFoldAll)
  EVT_MENU(ID_UNFOLD_ALL, MyFrame::OnUnfoldAll)
  EVT_MENU(ID_OPEN_LARGE_FILE, MyFrame::OnOpen)
  EVT_MENU(ID_PRINT_PREVIEW, MyFrame::OnPrintPreview)
  EVT_MENU(ID_PRINT_SETUP, MyFrame::OnPrintSetup)
  EVT_MENU(ID_PRINT, MyFrame::OnPrint)
  EVT_MENU(ID_WORD_COUNT, MyFrame::OnWordCount)
  EVT_MENU(ID_IMPORT_MSWORD, MyFrame::OnImportMSWord)
  EVT_MENU(ID_EXPORT_MSWORD, MyFrame::OnExportMSWord)
  EVT_MENU(ID_HIDE_PANE, MyFrame::OnClosePane)
  EVT_MENU(ID_COMMAND, MyFrame::OnCommand)
  EVT_MENU(ID_FIND, MyFrame::OnFind)
  EVT_MENU(ID_FIND_AGAIN, MyFrame::OnFindAgain)
  EVT_MENU(ID_GOTO, MyFrame::OnGoto)
  EVT_MENU(ID_FEEDBACK, MyFrame::OnFeedback)
  EVT_MENU(ID_PREVIOUS_DOCUMENT, MyFrame::OnPreviousDocument)
  EVT_MENU(ID_NEXT_DOCUMENT, MyFrame::OnNextDocument)
  EVT_MENU(ID_BROWSER, MyFrame::OnBrowser)
  EVT_MENU(ID_REPLACE, MyFrame::OnFindReplace)
  EVT_MENU(ID_GLOBAL_REPLACE, MyFrame::OnGlobalReplace)
  EVT_MENU(ID_CHECK_WELLFORMED, MyFrame::OnCheckWellformedness)
  EVT_MENU(ID_VALIDATE_DTD, MyFrame::OnValidateDTD)
  EVT_MENU(ID_VALIDATE_RELAX_NG, MyFrame::OnValidateRelaxNG)
  EVT_MENU(ID_VALIDATE_W3C_SCHEMA, MyFrame::OnValidateSchema)
  EVT_MENU(ID_XPATH, MyFrame::OnXPath)
  EVT_MENU_RANGE(ID_XSLT, ID_XSLT_WORDML_DOCBOOK, MyFrame::OnXslt)
  EVT_MENU(ID_PRETTYPRINT, MyFrame::OnPrettyPrint)
  EVT_MENU(ID_ENCODING, MyFrame::OnEncoding)
  EVT_MENU(ID_SPELL, MyFrame::OnSpelling)
  EVT_MENU(ID_FONT_SMALLER, MyFrame::OnFontSmaller)
  EVT_MENU(ID_FONT_NORMAL, MyFrame::OnFontMedium)
  EVT_MENU(ID_FONT_LARGER, MyFrame::OnFontLarger)
  EVT_MENU(ID_OPTIONS, MyFrame::OnOptions)
  EVT_MENU(ID_HOME, MyFrame::OnHome)
  EVT_MENU(ID_DOWNLOAD_SOURCE, MyFrame::OnDownloadSource)
  EVT_MENU(ID_TOOLBAR_VISIBLE, MyFrame::OnToolbarVisible)
  EVT_MENU(ID_LOCATION_PANE_VISIBLE, MyFrame::OnLocationPaneVisible)
  EVT_MENU(ID_PROTECT_TAGS, MyFrame::OnProtectTags)
  EVT_MENU(ID_WRAP_WORDS, MyFrame::OnWrapWords)
  EVT_MENU_RANGE(ID_SHOW_TAGS, ID_HIDE_TAGS, MyFrame::OnVisibilityState)
  EVT_MENU_RANGE(ID_ASSOCIATE_DTD_PUBLIC, ID_ASSOCIATE_XSL, MyFrame::OnAssociate)
  EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MyFrame::OnHistoryFile)
  EVT_MENU_RANGE(
    ID_VALIDATE_PRESET1, ID_VALIDATE_PRESET9, MyFrame::OnValidatePreset)
  EVT_MENU_RANGE(
    ID_COLOR_SCHEME_DEFAULT,
    ID_COLOR_SCHEME_NONE,
    MyFrame::OnColorScheme)
  EVT_UPDATE_UI_RANGE(ID_REPLACE, ID_GLOBAL_REPLACE, MyFrame::OnUpdateReplaceRange)
  EVT_FIND(wxID_ANY, MyFrame::OnDialogFind)
  EVT_FIND_NEXT(wxID_ANY, MyFrame::OnDialogFind)
  EVT_FIND_REPLACE(wxID_ANY, MyFrame::OnDialogReplace)
  EVT_FIND_REPLACE_ALL(wxID_ANY, MyFrame::OnDialogReplaceAll)
  EVT_ICONIZE(MyFrame::OnIconize)
  EVT_UPDATE_UI(ID_LOCATION_PANE_VISIBLE, MyFrame::OnUpdateLocationPaneVisible)
  EVT_UPDATE_UI(wxID_CLOSE, MyFrame::OnUpdateDocRange)
  EVT_UPDATE_UI(wxID_SAVEAS, MyFrame::OnUpdateDocRange)
  EVT_UPDATE_UI(wxID_CLOSE_ALL, MyFrame::OnUpdateCloseAll)
  EVT_UPDATE_UI(wxID_REVERT, MyFrame::OnUpdateSaveUndo)
  EVT_UPDATE_UI(wxID_SAVE, MyFrame::OnUpdateDocRange) // always allow save if doc present
  EVT_UPDATE_UI(wxID_UNDO, MyFrame::OnUpdateSaveUndo)
  EVT_UPDATE_UI(wxID_REDO, MyFrame::OnUpdateRedo)
  EVT_UPDATE_UI(wxID_PASTE, MyFrame::OnUpdatePaste)
  EVT_UPDATE_UI(wxID_CUT, MyFrame::OnUpdateCutCopy)
  EVT_UPDATE_UI(wxID_COPY, MyFrame::OnUpdateCutCopy)
  EVT_UPDATE_UI(ID_FIND_AGAIN, MyFrame::OnUpdateFindAgain)
  EVT_UPDATE_UI_RANGE(ID_FIND, ID_EXPORT_MSWORD, MyFrame::OnUpdateDocRange)
  EVT_UPDATE_UI(ID_PREVIOUS_DOCUMENT, MyFrame::OnUpdatePreviousDocument)
  EVT_UPDATE_UI(ID_NEXT_DOCUMENT, MyFrame::OnUpdateNextDocument)
  EVT_UPDATE_UI(ID_HIDE_PANE, MyFrame::OnUpdateClosePane)
  EVT_IDLE(MyFrame::OnIdle)
  EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MyFrame::OnPageClosing)
#ifdef __WXMSW__
  EVT_DROP_FILES(MyFrame::OnDropFiles)
#endif
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

MyApp::MyApp() : checker(NULL), server(NULL), connection(NULL),
#ifdef __WXMSW__
  config(new wxConfig(_T("SourceForge Project\\XML Copy Editor")))
#else
  config(new wxConfig(_T("xmlcopyeditor")))
#endif
{
  lang = 0;

#ifdef __WXGTK__
  int fdnull = open("/dev/null", O_WRONLY, 0);
  dup2(fdnull, STDERR_FILENO);
#endif
  myLocale.Init();
  int systemLocale = myLocale.GetSystemLanguage();
  switch (systemLocale)
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
    default:
      systemLocale = wxLANGUAGE_ENGLISH_US;
      break;
  }
  
  if (config.get())
  {
#ifdef __WXMSW__
    singleInstanceCheck = config->Read(_T("singleInstanceCheck"), true);
#else
    long longFalse = 0;
    singleInstanceCheck = config->Read(_T("singleInstanceCheck"), longFalse);
#endif
    lang = config->Read(_T("lang"), systemLocale);
  }
  else
  {
    lang = systemLocale;
#ifdef __WXMSW__
    singleInstanceCheck = true;
#else
    singleInstanceCheck = false;
#endif
  }

  myLocale.Init(lang, wxLOCALE_LOAD_DEFAULT);
  wxLocale::AddCatalogLookupPathPrefix(wxT("."));
  wxLocale::AddCatalogLookupPathPrefix(wxT(".."));
  
#ifdef __LINUX__
  wxLocale::AddCatalogLookupPathPrefix(GetLinuxAppDir::run());
#endif
  
  myLocale.AddCatalog(_T("messages"));

#ifdef __LINUX__
  {
    wxLogNull noLog;
    myLocale.AddCatalog(_T("fileutils"));
  }
#endif
}

MyApp::~MyApp()
{
  delete checker;
  delete server;
  delete connection;
}

bool MyApp::OnInit()
{

  wxString name, service, hostName;
  name.Printf(_T("xmlcopyeditor-%s"), wxGetUserId().c_str());
  service = IPC_SERVICE;
  hostName = _T("localhost");

  if (singleInstanceCheck)
  {
    checker = new wxSingleInstanceChecker(name);
    while (checker->IsAnotherRunning())
    {
      // attempt calling server
      client = new MyClient();
      connection = (MyClientConnection *)
         client->MakeConnection(hostName, service, IPC_TOPIC);
      if (!connection || !connection->StartAdvise(IPC_ADVISE_NAME))
        break;
      else
      {
        wxString argument;
        if (this->argc > 1)
        {
          for (int i = 1; i < this->argc; i++)
          {
            argument = (wxString)this->argv[i];
            argument = PathResolver::run(argument);
            connection->Poke(argument, _T("Data"));
          }
        }
        else
        {
          argument = (wxString)IPC_NO_FILE;
          connection->Poke(argument, _T("Data"));
        }
        return false;
      }
    }
  }

  server = new MyServer;
  server->Create(service);

  MyFrame *frame;
  try
  {
    wxImage::AddHandler(new wxPNGHandler);
    wxSystemOptions::SetOption(_T("msw.remap"), 0);
    frame = new MyFrame(
        _("XML Copy Editor"),
        config.get(),
        myLocale,
        singleInstanceCheck,
        lang);
    frame->Show(true);
    if (frame->getHandleCommandLineFlag())
      frame->handleCommandLine();
  }
  catch (exception &e) {
    const char *what;
    what = e.what();
    wxString wideWhat, errorString;
    wideWhat = wxString(what, wxConvLocal, strlen(what));
    
    if (wideWhat.empty())
      wideWhat = _("(unknown error)");

    errorString = _("XML Copy Editor has encountered the following error and needs to close: ");
    errorString += wideWhat;
    errorString += _T(".");
#ifdef __WXMSW__
    ::MessageBox(
      NULL,
      errorString,
      _("Error"),
      MB_ICONERROR | MB_TASKMODAL);
#else
    wxMessageBox(errorString, _("Error"), wxICON_ERROR);
#endif
    exit(EXIT_FAILURE);
  }
  catch (...)
  {
    exit(EXIT_FAILURE);
  }
  return true;
}

void MyApp::OnUnhandledException()
{
#ifdef __WXMSW__
  ::MessageBox(
    NULL,
    _("XML Copy Editor has encountered an error and needs to close."),
    _("Error"),
    MB_ICONERROR | MB_TASKMODAL);
#else
  wxMessageBox(
    _("XML Copy Editor has encountered an error and needs to close."),
    _("Error"),
    wxICON_ERROR);
#endif
  exit(EXIT_FAILURE);
}

bool MyApp::OnExceptionInMainLoop()
{
  try {
    throw;
  }
#ifdef __WXMSW__
  catch (bad_alloc&)
  {
    ::MessageBox(
      NULL,
      _("The operating system has turned down a request for additional memory"),
      _("Out of memory"),
      MB_ICONERROR);
    return true;
  }
#endif
  catch (exception &e)
  {
    const char *what;
    what = e.what();
    wxString wideWhat, errorString;
    wideWhat = wxString(what, wxConvLocal, strlen(what));

    if (wideWhat.empty())
      _("(unknown error)");

    errorString = _("The following error has occurred: ");
    errorString += wideWhat;
    errorString += _(".\n\nSelect \"Abort\" to exit, \"Retry\" to close this window and \"Ignore\" to continue.");
#ifdef __WXMSW__
    int ret = ::MessageBox(
      NULL,
      errorString,
      _("Error"),
      MB_ABORTRETRYIGNORE |
      MB_ICONERROR |
      MB_TASKMODAL);
    switch (ret)
    {
      case IDABORT:
        exit(EXIT_FAILURE);
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
    wxMessageBox(
      errorString,
      _("Error"),
      wxICON_ERROR);
    return false;
#endif
  }
  catch (...)
  {
    wxString otherError(_("XML Copy Editor has encountered an error and needs to close."));
#ifdef __WXMSW__
    ::MessageBox(
      NULL,
      otherError,
      _("Error"),
      MB_ICONERROR);
    return false;
#else
    wxMessageBox(
      otherError,
      _("Error"),
      wxICON_ERROR);
    return false;
#endif
  }
  return false;
}

#ifndef __WXMSW__
void MyApp::HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const
{
  try
  {
    wxApp::HandleEvent(handler, func, event);
  }
  catch (std::bad_alloc&)
  {
    wxMessageBox(
      _("The operating system has turned down a request for additional memory"),
      _("Out of memory"),
      wxICON_ERROR);
    return;
  }
  catch (std::exception& e)
  {
    std::string s(e.what());
    wxString ws = wxString(s.c_str(), wxConvUTF8, s.size());
    wxMessageBox(
      ws,
      _("Error"),
      wxICON_ERROR);
    return;
  }
  catch (...)
  {
    throw;
  }
}
#endif

MyFrame::MyFrame(
  const wxString& title,
  wxConfig *configParameter,
  wxLocale& locale,
  bool singleInstanceCheckParameter,
  int langParameter) :
  wxFrame(NULL, wxID_ANY, title),
  config(configParameter),
  myLocale(locale),
  singleInstanceCheck(singleInstanceCheckParameter),
  lang(langParameter),
  htmlPrinting(new wxHtmlEasyPrinting(
    wxEmptyString,
    this)),
  findDialog(0),
#ifndef __WXMSW__
  helpController(new wxHtmlHelpController()),
#endif
  menuBar(0),
  toolBar(0),
  xmlMenu(0),
  mainBook(0),
  restoreFocusToNotebook(false)
{
 saveRawUtf8(
      wtfn.name(),
      buffer))
    {
      messagePane(
        _("Cannot save temporary copy for validation; please save or discard changes"),
        CONST_STOP);
      return;
    }
    fname = wtfn.wideName();
  }

  std::string fnameLocal = (const char *)fname.mb_str(wxConvLocal);
  
  doc->clearErrorIndicators();
  statusProgress(_("DTD validation in progress..."));

  auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess, catalogPath));
  
  if (!wl->validate(fnameLocal))
  {
    std::string error = wl->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    statusProgress(wxEmptyString);
    messagePane(wideError, CONST_WARNING);
    
    std::pair<int, int> posPair = wl->getErrorPosition();
    --(posPair.first);
    int cursorPos =
      doc->PositionFromLine(posPair.first);
    doc->SetSelection(cursorPos, cursorPos);

    // shallow validate all
    doc->shallowValidate(); // has to come first as it deletes all indicators
    doc->setErrorIndicator(posPair.first, posPair.second);

    return;
  }
  statusProgress(wxEmptyString);
  documentOk(_("valid"));
}

void MyFrame::OnValidateRelaxNG(wxCommandEvent& event)
{
  statusProgress(wxEmptyString);

  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  wxString fileName = doc->getFullFileName();
  
  wxString defaultFile, defaultDir;
  defaultFile = doc->getFullFileName();
  if (!defaultFile.empty())
  {
    wxFileName fn(defaultFile);
    defaultDir = fn.GetPath();
  }
  AssociateDialog ad(
    this,
    _("Select RELAX NG grammar"),
    _("Choose a file:"),
    _("RELAX NG grammar"),
    _T("*.*"),
    lastRelaxNGSchema);
  if (ad.ShowModal() != wxID_OK)
    return;
    
  wxString path = lastRelaxNGSchema = ad.getUrl();

  if (path.empty())
  {
    statusProgress(wxEmptyString);
    return;
  }
  
  validateRelaxNG(doc, path, fileName);
}

void MyFrame::validateRelaxNG(
  XmlDoc *doc,
  const wxString& schemaName,
  wxString& fileName) // not const: may change if empty/document modified
{
  statusProgress(wxEmptyString);
  
  if (!doc)
    return;

  WrapTempFileName wtfn(fileName);
  if (fileName.empty() || doc->GetModify())
  {
    wxString wideBuffer = doc->GetText();
    
    std::string buffer = (const char *)wideBuffer.mb_str(wxConvUTF8);
    if (!saveRawUtf8(
      wtfn.name(),
      buffer))
    {
      messagePane(
        _("Cannot save temporary copy for validation; please save or discard changes"),
        CONST_STOP);
      return;
    }
    fileName = wtfn.wideName();
  }

  doc->clearErrorIndicators();
  statusProgress(_("RELAX NG validation in progress..."));

  auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess, catalogPath));

  std::string schemaFileNameLocal = (const char *)schemaName.mb_str(wxConvLocal);
  std::string fileNameLocal = (const char *)fileName.mb_str(wxConvLocal);
  if (!wl->validateRelaxNG(schemaFileNameLocal, fileNameLocal))
  {
    std::string error = wl->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    statusProgress(wxEmptyString);

    std::pair<int, int> posPair = wl->getErrorPosition();
    --(posPair.first);

    int cursorPos =
      doc->PositionFromLine(posPair.first);
    doc->SetSelection(cursorPos, cursorPos);
    doc->setErrorIndicator(posPair.first, posPair.second);
    messagePane(wideError, CONST_WARNING);
    doc->SetFocus();
    return;
  }

  statusProgress(wxEmptyString);
  documentOk(_("valid"));
  doc->SetFocus();
}

void MyFrame::OnValidatePreset(wxCommandEvent& event)
{
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  wxString fileName = doc->getFullFileName();

  int id = event.GetId();
  wxString schemaFileName = validationPresetMap[id];
  if (schemaFileName.empty())
    return;
  validateRelaxNG(doc, schemaFileName, fileName);
}

void MyFrame::OnValidateSchema(wxCommandEvent& event)
{
  statusProgress(wxEmptyString);

  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

/*
  // schema location required
  std::auto_ptr<XmlSchemaLocator> xsl(new XmlSchemaLocator());
  std::string utf8Buffer;
  getRawText(doc, utf8Buffer);
  XmlEncodingHandler::setUtf8(utf8Buffer, true);
  xsl->parse(utf8Buffer);
  std::string utf8Location = xsl->getSchemaLocation();
  if (utf8Location.empty())
  {
    messagePane(_("No XML Schema found"), CONST_STOP);
    return;
  }
*/

  wxString fileName;
  std::string tempFileNameLocal;
  fileName = doc->getFullFileName();
  
  WrapTempFileName wtfn(fileName);
  if (fileName.empty() || doc->GetModify())
  {
    wxString wideBuffer = doc->GetText();
    
    std::string buffer = (const char *)wideBuffer.mb_str(wxConvUTF8);
    if (!saveRawUtf8(
      wtfn.name(),
      buffer))
    {
      messagePane(
        _("Cannot save temporary copy for validation; please save or discard changes"),
        CONST_STOP);
      return;
    }
    fileName = wtfn.wideName();
  }
  
  doc->clearErrorIndicators();

  std::string error;
  wxString wideError;
#ifdef __WXMSW__
  // separate WrapTempFileName for output log
  WrapTempFileName tempFileName(_T(""));

  wxString cmd = binDir +
    _T("validate.exe \"") +
    fileName + _T("\" \"") +
    tempFileName.wideName() + _T("\"");

  statusProgress(_("Validation in progress..."));
  int result = wxExecute(cmd, wxEXEC_SYNC);
  statusProgress(wxEmptyString);

  switch (result)
  {
    case 0:
      documentOk(_("valid"));
      break;
    case 1:
      messagePane(_("MSXML validation failed (version 4.0 or later required)"), CONST_STOP);
      break;
    case 2:
      if (ReadFile::run(tempFileName.name(), error))
      {
        wideError = wxString(error.c_str(), wxConvLocal, error.size());
        messagePane(wideError, CONST_WARNING);
        // tbd: extract line & column numbers; use error indicators
      }
      else
        messagePane(_("Validation error"), CONST_WARNING);
      break;
    default:
      break;
  }
  doc->SetFocus();
  return;
#else
  std::auto_ptr<WrapXerces> validator(new WrapXerces());
  std::string fileNameLocal = (const char *)fileName.mb_str(wxConvLocal);
  if (!validator->validate(fileNameLocal))
  {
    error = validator->getLastError();
    wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    messagePane(wideError, CONST_WARNING);
    std::pair<int, int> posPair = validator->getErrorPosition();

    int cursorPos =
      doc->PositionFromLine(posPair.first - 1);
    doc->SetSelection(cursorPos, cursorPos);
    doc->setErrorIndicator(posPair.first - 1, posPair.second);
  }
  else
    documentOk(_("valid"));

  /*
  // handle relative locations
  schemaPath = PathResolver::run(schemaPath, fileName);

  statusProgress(_("XML Schema validation in progress..."));

  auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess, catalogPath));
  std::string schemaFileNameLocal = (const char *)schemaPath.mb_str(wxConvLocal);
  std::string fileNameLocal = (const char *)fileName.mb_str(wxConvLocal);
  if (!wl->validateW3CSchema(schemaFileNameLocal, fileNameLocal))
  {
    std::string error = wl->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    statusProgress(wxEmptyString);

    std::pair<int, int> posPair = wl->getErrorPosition();
    --(posPair.first);

    int cursorPos =
      doc->PositionFromLine(posPair.first - 1);
    doc->SetSelection(cursorPos, cursorPos);
    doc->setErrorIndicator(posPair.first - 1, posPair.second);

    messagePane(wideError, CONST_WARNING);
    return;
  }

  statusProgress(wxEmptyString);
  documentOk(_("valid"));
  */
#endif
}

void MyFrame::OnXPath(wxCommandEvent& event)
{
  statusProgress(wxEmptyString);
  closePane();

  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  auto_ptr<wxTextEntryDialog> dlg(new wxTextEntryDialog(
    this,
    _("Enter XPath:"),
    _("Evaluate XPath"),
    xpathExpression));

  int ret = dlg->ShowModal();
  if (ret == wxID_CANCEL)
    return;
  xpathExpression = dlg->GetValue();
  std::string valUtf8 = (const char *)xpathExpression.mb_str(wxConvUTF8);

  // fetch document contents
  std::string rawBufferUtf8;
  getRawText(doc, rawBufferUtf8);
  if (!XmlEncodingHandler::setUtf8(rawBufferUtf8))
  {
    encodingMessage();
    return;
  }
  
  WrapTempFileName tempFileName(doc->getFullFileName());
  
  ofstream rawBufferStream(tempFileName.name().c_str());
  if (!rawBufferStream)
    return;
  rawBufferStream << rawBufferUtf8;
  rawBufferStream.close();

  auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess, catalogPath));
  
  bool success = wl->xpath(valUtf8, tempFileName.name());
  
  if (!success)
  {
    std::string error = wl->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    if (!wideError.empty())
      wideError.Prepend(_T(": "));
    wideError.Prepend(_("Cannot evaluate XPath"));
      
    messagePane(wideError, CONST_WARNING);
    return;
  }
  
  statusProgress(wxEmptyString);
  std::string buffer = wl->getOutput();

  if (buffer.empty())
  {
    messagePane(_("No matching nodes found"), CONST_WARNING);
    return;
  }
  newDocument(buffer);
  statusProgress(wxEmptyString);
}

void MyFrame::OnXslt(wxCommandEvent& event)
{
  statusProgress(wxEmptyString);
  closePane();

  // fetch document contents
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;
  std::string rawBufferUtf8;
  getRawText(doc, rawBufferUtf8);
  if (!XmlEncodingHandler::setUtf8(rawBufferUtf8))
  {
    encodingMessage();
    return;
  }
  
  WrapTempFileName tempFileName(doc->getFullFileName());
  
  ofstream rawBufferStream(tempFileName.name().c_str());
  if (!rawBufferStream)
    return;
  rawBufferStream << rawBufferUtf8;
  rawBufferStream.close();
  
  wxString path;

  int id = event.GetId();
  if (id == ID_XSLT)
  {
    XslLocator xl;
    xl.parse(rawBufferUtf8);
    std::string location = xl.getXslLocation();
  
    path = wxString(location.c_str(), wxConvUTF8, location.size());
    path.Replace(_T("%20"), _T(" "), true);

    path = PathResolver::run(path, doc->getFullFileName());

    if (!wxFileName::FileExists(path))
    {
      if (!path.empty())
      {
        wxString message;
        message.Printf(_("Cannot open stylesheet %s"), path.c_str());
        messagePane(message, CONST_WARNING);
      }
  
      wxString defaultFile, defaultDir;
      defaultFile = doc->getFullFileName();
      if (!defaultFile.empty())
      {
        wxFileName fn(defaultFile);
        defaultDir = fn.GetPath();
      }
      AssociateDialog ad(
        this,
        _("Select stylesheet"),
        _("Choose a file:"),
        _("XSLT stylesheet"),
        _T("*.xsl;*.xslt"),
        lastXslStylesheet);
      if (ad.ShowModal() != wxID_OK)
        return;
    
      path = lastXslStylesheet = ad.getUrl();


      if (path.empty()) // Cancel selected
      {
        statusProgress(wxEmptyString);
        return;
      }
    }
  }
  else
  {
    wxString sep;
    sep.Append(wxFileName::GetPathSeparator());
    switch (id)
    {
      case ID_XSLT_TEI_FO:
        path = applicationDir + sep + _T("tei") + sep + _T("fo") + sep +
          _T("tei.xsl");
        break;
      case ID_XSLT_TEI_HTML:
        path = applicationDir + sep + _T("tei") + sep + _T("html") + sep +
          _T("tei.xsl");
        break;
      case ID_XSLT_TEI_XHTML:
        path = applicationDir + sep + _T("tei") + sep + _T("xhtml") + sep +
          _T("tei.xsl");
        break;
      case ID_XSLT_TEI_LATEX:
        path = applicationDir + sep + _T("tei") + sep + _T("latex") + sep +
          _T("tei.xsl");
        break;
      case ID_XSLT_DOCBOOK_FO:
        path = applicationDir + sep + _T("docbook") + sep + _T("fo") + sep +
          _T("docbook.xsl");
        break;
      case ID_XSLT_DOCBOOK_HTML:
        path = applicationDir + sep + _T("docbook") + sep + _T("html") + sep +
          _T("docbook.xsl");
        break;
      case ID_XSLT_DOCBOOK_XHTML:
        path = applicationDir + sep + _T("docbook") + sep + _T("xhtml") + sep +
          _T("docbook.xsl");
        break;
      default:
        break;
    }
  }
  statusProgress(_("XSL transformation in progress..."));

  std::string stylefnameLocal = (const char *)path.mb_str(wxConvLocal);
  
  auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess, catalogPath));
  if (!wl->xslt(stylefnameLocal, tempFileName.name()))
  {
    std::string error = wl->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    wideError.Prepend(_("Cannot transform: "));
    statusProgress(wxEmptyString);
    messagePane(wideError, CONST_WARNING);
    return;
  }
  std::string buffer = wl->getOutput();
  if (buffer.empty())
  {
    messagePane(_("Output document empty"), CONST_WARNING);
    return;
  }
  statusProgress(wxEmptyString);
  newDocument(buffer);
}

void MyFrame::OnPrettyPrint(wxCommandEvent& event)
{
  statusProgress(wxEmptyString);
  closePane();

  // fetch document contents
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  int line = doc->LineFromPosition(doc->GetCurrentPos());

  std::string rawBufferUtf8;
  getRawText(doc, rawBufferUtf8);

  std::string encoding = XmlEncodingHandler::get(rawBufferUtf8);

  if (!XmlEncodingHandler::setUtf8(rawBufferUtf8, true))
  {
    encodingMessage();
    return;
  }

  statusProgress(_("Pretty-printing in progress..."));

  for (int i = 0; i < 2; i++) // perform two iterations
  {
    WrapTempFileName tempFileName(doc->getFullFileName());

    ofstream rawBufferStream(tempFileName.name().c_str());
    if (!rawBufferStream)
      return;
    rawBufferStream << rawBufferUtf8;
    rawBufferStream.close();
  
    auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess, catalogPath));
    bool success = wl->parse(tempFileName.name(), true);
  
    if (!success)
    {
      std::string error = wl->getLastError();
      wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
      wideError.Prepend(_("Cannot pretty-print: "));
      statusProgress(wxEmptyString);
      messagePane(wideError, CONST_WARNING);
      return;
    }
    rawBufferUtf8 = wl->getOutput();
  }
  
  statusProgress(wxEmptyString);
  if (rawBufferUtf8.empty())
    messagePane(
      _("Pretty-print unsuccessful: output document empty"),
      CONST_STOP);
  else
  {
/*
    if (encoding != "UTF-8")
    {
      std::string output = getEncodedBuffer(rawBufferUtf8, encoding);
      if (!output.empty())
        rawBufferUtf8 = output;
    }
*/
    if (encoding != "UTF-8" && !encoding.empty())
    {
      XmlEncodingHandler::set(rawBufferUtf8, encoding);
    }
    doc->SetTextRaw(rawBufferUtf8.c_str());
    statusProgress(wxEmptyString);
  }
  
  doc->GotoLine(line);
  doc->SetFocus();
}

void MyFrame::OnEncoding(wxCommandEvent& event)
{
  statusProgress(wxEmptyString);
  closePane();

  // fetch document contents
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  std::vector<wxString> encodingVector;
  encodingVector.push_back(_T("UTF-8"));
  encodingVector.push_back(_T("UTF-16"));
  encodingVector.push_back(_T("UTF-16LE"));
  encodingVector.push_back(_T("UTF-16BE"));
  encodingVector.push_back(_T("ISO-8859-1"));
  encodingVector.push_back(_T("US-ASCII"));
  const int vectorSize = encodingVector.size();
  wxString choiceArray[vectorSize + 1];
  for (int i = 0; i < vectorSize; ++i)
    *(choiceArray + i) = encodingVector.at(i);
  wxSingleChoiceDialog scd(
    this, _("Choose an encoding:"), _("Encoding"), vectorSize, choiceArray);

  if (scd.ShowModal() == wxID_CANCEL)
    return;

  wxString selection;
  std::string selectionUtf8, bufferUtf8;

  selection = scd.GetStringSelection();
  selectionUtf8 = selection.mb_str(wxConvUTF8);

  getRawText(doc, bufferUtf8);
  XmlEncodingHandler::setUtf8(bufferUtf8, true);

  WrapTempFileName tempFileName(_T(""));

  std::auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess));
  int res;

  WrapTempFileName sourceFileName(doc->getFullFileName());
  saveRawUtf8(sourceFileName.name(), bufferUtf8);

  res = wl->saveEncodingFromFile(sourceFileName.name(), tempFileName.name(), selectionUtf8);
  if (res == -1)
  {
    std::string error = wl->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    wideError.Prepend(_("Cannot set encoding: "));
    messagePane(wideError, CONST_STOP);
    return;
  }

  std::string newBuffer;
  bool success = ReadFile::run(tempFileName.name(), newBuffer);
  if (!success)
  {
    messagePane(_("Cannot set encoding (cannot open temporary file)"),
      CONST_STOP);
    return;
  }

  std::auto_ptr<XmlUtf8Reader> xur(new XmlUtf8Reader(
    false,
    expandInternalEntities,
    newBuffer.size()));
  if (!xur->parse(newBuffer))
  {
    messagePane(_("Cannot set encoding (cannot parse temporary file)"),
      CONST_STOP);
    return;
  }

/*
  if (wl->getLastError().empty())
  {
    doc->SetTextRaw(xur->getBuffer().c_str());
  }
  else
  {
    std::string err = wl->getLastError();
    wxString wideErr;
    wideErr = _("Cannot set encoding: ");
    wideErr += wxString(err.c_str(), wxConvUTF8, err.size());

    messagePane(wideErr, CONST_STOP);
  }
*/
  doc->SetTextRaw(xur->getBuffer().c_str());
  doc->SetFocus();
}

/*
std::string MyFrame::getEncodedBuffer(
  const std::string& bufferUtf8,
  const std::string& encodingUtf8)
{
  WrapTempFileName tempFileName(_T(""));

  std::auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess));
  int res;

  // change to saveEncodingFromFile?
  res = wl->saveEncoding(bufferUtf8, tempFileName.name(), encodingUtf8);
  if (res == -1)
  {
    std::string error = wl->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    wideError.Prepend(_("Cannot set encoding: "));
    messagePane(wideError, CONST_STOP);
    return "";
  }

  std::string newBuffer;
  bool success = ReadFile::run(tempFileName.name(), newBuffer);
  if (!success)
  {
    messagePane(_("Cannot set encoding (cannot open temporary file)"),
      CONST_STOP);
    return "";
  }

  std::auto_ptr<XmlUtf8Reader> xur(new XmlUtf8Reader(
    false,
    expandInternalEntities,
    newBuffer.size()));
  if (!xur->parse(newBuffer))
  {
    messagePane(_("Cannot set encoding (cannot parse temporary file)"),
      CONST_STOP);
    return "";
  }
  return xur->getBuffer().c_str();
}
*/

void MyFrame::OnHome(wxCommandEvent& event)
{
  navigate(_T("http://xml-copy-editor.sourceforge.net"));
}

void MyFrame::OnDownloadSource(wxCommandEvent& event)
{
  navigate(_T("http://xml-copy-editor.svn.sourceforge.net/viewvc/xml-copy-editor/")); 
}

void MyFrame::OnToolbarVisible(wxCommandEvent& event)
{
  if (!viewMenu)
    return;
#ifdef __WXMSW__
  if (useCoolBarOnStart)
    toolbarVisible = (viewMenu->IsChecked(ID_TOOLBAR_VISIBLE)) ? false : true;
  else
    toolbarVisible = (toolbarVisible) ? false : true;
#else
  toolbarVisible = (toolbarVisible) ? false : true;
#endif
  viewMenu->Check(ID_TOOLBAR_VISIBLE, toolbarVisible);
  showTopBars(toolbarVisible);
  manager.Update();
}

void MyFrame::OnWrapWords(wxCommandEvent& event)
{
  if (!viewMenu)
    return;
    
  bool wrapWords;
#ifdef __WXMSW__
  wrapWords = (viewMenu->IsChecked(ID_WRAP_WORDS)) ? false : true;
#else
  wrapWords = (properties.wrap) ? false : true;
#endif

  viewMenu->Check(ID_WRAP_WORDS, wrapWords);
  properties.wrap = wrapWords;
  
  // update all documents
  int pageCount = mainBook->GetPageCount();
  XmlDoc *currentDoc;
  for (int i = 0; i < pageCount; ++i)
  {
    currentDoc = (XmlDoc *)mainBook->GetPage(i);
    if (!currentDoc)
      break;
    currentDoc->SetWrapMode(wrapWords);
  }
}
  
void MyFrame::OnLocationPaneVisible(wxCommandEvent& event)
{
  wxAuiPaneInfo info = manager.GetPane(locationPanel);
  bool visible = (info.IsShown()) ? false : true;
  manager.GetPane(locationPanel).Show(visible);
  manager.Update();

  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;
  doc->SetFocus();
}

void MyFrame::OnProtectTags(wxCommandEvent& event)
{
  if (!xmlMenu)
    return;
#ifdef __WXMSW__
  if (useCoolBarOnStart)
    protectTags = (xmlMenu->IsChecked(ID_PROTECT_TAGS)) ? false : true;
  else
  
    protectTags = (protectTags) ? false : true;
#else
  protectTags = (protectTags) ? false : true;
#endif
  if (xmlMenu)
    xmlMenu->Check(ID_PROTECT_TAGS, protectTags);
  if (toolBar)
    toolBar->ToggleTool(ID_PROTECT_TAGS, protectTags);

  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  if (protectTags)
    doc->adjustCursor(); // apply to all open docs?

  doc->SetFocus();
}

void MyFrame::OnVisibilityState(wxCommandEvent& event)
{
  
  int id;
  id = event.GetId();
  switch (id)
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
  if (viewMenu)
    viewMenu->Check(id, true);
    
  // iterate over all open documents
  int pageCount = mainBook->GetPageCount();
  XmlDoc *currentDoc;
  for (int i = 0; i < pageCount; ++i)
  {
    currentDoc = (XmlDoc *)mainBook->GetPage(i);
    if (!currentDoc)
      break;
    currentDoc->applyVisibilityState(visibilityState);
  }

  if (visibilityState == HIDE_ATTRIBUTES || visibilityState == HIDE_TAGS)
  {
    if (properties.protectHiddenElements && !protectTags)
    {
      wxCommandEvent e;
      OnProtectTags(e);
    }
  }

  // fetch current document
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  // set focus for current document
  doc->SetFocus();
}

void MyFrame::OnFeedback(wxCommandEvent& event)
{
  wxString forumUrl =
    _T("https://sourceforge.net/forum/forum.php?forum_id=475215");
  navigate(forumUrl);
}

void MyFrame::navigate(const wxString& url)
{
  wxString testString = browserCommand;
  testString.Replace(_T(" -remote"), wxEmptyString, true);
  if (browserCommand.empty())
  {
    messagePane(
      _("Cannot open in browser: no browser defined (see Tools, Options..., General)"),
      CONST_STOP);
    return;
  }
  else if (!wxFileName::FileExists(testString))
  {
    wxString message;
    message.Printf(
      _("Cannot open in browser: %s not found (see Tools, Options..., General)"),
      testString.c_str());
    messagePane(message, CONST_STOP);
    return;
  }
  wxString cmd;
  cmd = browserCommand + _T(" \"") + url + _T("\"");
  wxExecute(cmd, wxEXEC_ASYNC); // make ASYNC an option?
}

void MyFrame::findAgain(wxString s, int flags)
{
  findReplacePanel->flagNotFound(false);

  if (s.empty())
    return;

  statusProgress(wxEmptyString);
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;
    
  // update regex parameter to keep global replace in sync
  findRegex = findReplacePanel->getRegex();

  int newLocation;
  int myFlags = 0;
  if (flags & wxFR_WHOLEWORD)
    myFlags |= wxSTC_FIND_WHOLEWORD;
  if (flags & wxFR_MATCHCASE)
    myFlags |= wxSTC_FIND_MATCHCASE;
  if (findReplacePanel->getRegex())
    myFlags |= wxSTC_FIND_REGEXP;

  bool incrementalFind = 
    (findReplacePanel->getIncrementalFind()) ? true : false;

  //doc->SetYCaretPolicy(wxSTC_CARET_SLOP | wxSTC_CARET_STRICT, 10);

  if (flags & wxFR_DOWN) // find next
  {
    doc->SetTargetStart((incrementalFind) ?
      doc->GetSelectionStart() : doc->GetSelectionEnd());
    doc->SetTargetEnd(doc->GetLength());
    doc->SetSearchFlags(myFlags);
    newLocation = doc->SearchInTarget(s);
    
    // try once more from top
    if (newLocation == -1)
    {
      doc->SetTargetStart(0);
      doc->SetTargetEnd(doc->GetLength());
      newLocation = doc->SearchInTarget(s);
    }
  }
  else // find previous
  {
    doc->SetCurrentPos(
      (doc->GetSelectionStart()) ? doc->GetSelectionStart() - 1 : 0);
    doc->SearchAnchor();
    newLocation = doc->SearchPrev(myFlags, s);
  }

  //doc->SetYCaretPolicy(wxSTC_CARET_SLOP, 0);

  
  if (newLocation == -1)
  {
    findReplacePanel->flagNotFound(true);
    wxString err;
    err.Printf(_("Cannot find '%s'"), s.c_str());
    doc->SetSelectionEnd(doc->GetSelectionStart());

    statusProgress(err);

    // must clear target to prevent replace affecting whole document
    doc->SetTargetStart(0);
    doc->SetTargetEnd(0);

    return;
  }
  
  doc->SetSelection(doc->GetTargetStart(), doc->GetTargetEnd());
  doc->EnsureCaretVisible();
}

bool MyFrame::closeActiveDocument()
{
  statusProgress(wxEmptyString);
  closePane();
  
  int selection = mainBook->GetSelection();
  if (selection == -1 || !mainBook->GetPageCount()) // GetPageCount needed for wxAuiNotebook
    return false;
  
  locationPanel->update(NULL, wxEmptyString);
  insertChildPanel->update(NULL, wxEmptyString);
  insertSiblingPanel->update(NULL, wxEmptyString);

  // workaround -- wxAuiNotebook: send virtual close event? DeletePage doesn't generate one
  wxAuiNotebookEvent e;
  e.SetSelection(selection);
  OnPageClosing(e);
  if (deletePageVetoed)
    return false;

  mainBook->DeletePage(selection);
  return true;
  
  // was:
  // mainBook->DeletePage(selection);
  // return (!deletePageVetoed);
}

bool MyFrame::saveFile(XmlDoc *doc, wxString& fileName, bool checkLastModified)
{
  if (!doc)
    return false;
  
  statusProgress(wxEmptyString);

  if (checkLastModified)
  {
    wxFileName fn(fileName);
    if (fn.IsOk())
    {
      wxDateTime myLastModified = fn.GetModificationTime();
      if (!myLastModified.IsEqualTo(doc->getLastModified()))
      {
        int choice = wxMessageBox(
          _("File has been modified by another application.\nDo you want to proceed?"),
          _("Confirmation"),
          wxICON_QUESTION | wxYES_NO | wxCANCEL);
        if (choice != wxYES)
          return false;
      }
    }
  }

  int bytes = 0;
  std::string utf8Buffer, encoding, fileNameLocal;
  bool isXml = true;
  try
  {
    getRawText(doc, utf8Buffer);
    XmlEncodingSpy es;
    es.parse(utf8Buffer);
    encoding = es.getEncoding();
    
    fileNameLocal = fileName.mb_str(wxConvLocal);
    
    closePane();
    bool success;
    success = true;
    if (getFileType(fileName) != FILE_TYPE_XML)
    {
      isXml = false;
    }

    // raw file conditions
    if (doc->getType() == FILE_TYPE_BINARY)
    {
      success = saveRawUtf8(fileNameLocal, utf8Buffer, true, isXml);
      if (success)
        bytes = utf8Buffer.size();
    }
    else if (!isXml && encoding.empty())
    {
      success = saveRawUtf8(fileNameLocal, utf8Buffer, true, isXml);
      if (success)
        bytes = utf8Buffer.size();
    }
    else if (encoding == "UTF-8")
    {
      auto_ptr<WrapExpat> we(new WrapExpat());
      
      if (!we->parse(utf8Buffer))
      {
        std::string error = we->getLastError();
        wxString werror = wxString(error.c_str(), wxConvUTF8, error.size());
        if (we->isEncodingError())
          ;
        messagePane(werror, CONST_WARNING);
      }
      success = saveRawUtf8(fileNameLocal, utf8Buffer, true, isXml);
      if (success)
        bytes = utf8Buffer.size();
    }
    else
    {
      wxString wideEncoding = wxString(encoding.c_str(), wxConvLocal, encoding.size());

      // IF Unicode, use iconv to convert buffer
      if (encoding == "UTF-16" || encoding == "UTF-16LE" ||
        encoding == "UTF-16BE" || encoding == "UTF-32" || encoding == "UTF-32LE" ||
        encoding == "UTF-32BE")
      {
        iconv_t cd = iconv_open(encoding.c_str(), "UTF-8");
        if (cd == (iconv_t)-1)
        {
          success = saveRawUtf8(fileNameLocal, utf8Buffer, false, isXml);
          if (success)
          {
            bytes = utf8Buffer.size();
            wxString message;
            message.Printf(
              _("%s saved in default encoding UTF-8: unknown encoding %s"), 
              fileName.c_str(),
              wideEncoding.c_str());
            messagePane(message, CONST_WARNING);
          }
          else
            return false;
        }
   	    else
   	    {
          size_t utf8BufferLen = utf8Buffer.size();
        
          size_t iconvBufferLen, iconvBufferLeft, utf8BufferLeft;
          int iconvLenMultiplier = 4; // worst case scenario
          if (encoding == "UTF-16" ||
            encoding == "UTF-16BE" ||
            encoding == "UTF-16LE")
          {
            iconvLenMultiplier = 2;
          }
          else if (encoding == "UTF-32" ||
            encoding == "UTF-32BE" ||
            encoding == "UTF-32LE")
          {
            iconvLenMultiplier = 4;
          }

          iconvBufferLen = iconvBufferLeft = 
            utf8BufferLen * iconvLenMultiplier + 4; // worst case scenario

          char *finalBuffer;
          char *iconvBuffer = new char[iconvBufferLen];
      
          utf8BufferLeft = utf8BufferLen;
          iconvBuffer = new char[iconvBufferLen];
          finalBuffer = iconvBuffer; // iconvBuffer will be incremented by iconv
          size_t nconv;

#ifdef __WXMSW_
          const char *
#else
          char *
#endif
          utf8BufferPtr = 
          (char *)
          utf8Buffer.c_str();

          nconv = iconv(
            cd,
#ifdef __WXMSW__
            (const char **)
#endif
            &utf8BufferPtr,
            &utf8BufferLeft,
            &iconvBuffer,
            &iconvBufferLeft);

          iconv_close(cd);

          if (nconv == (size_t)-1) // conversion failed
          {
            delete[] finalBuffer;
            success = saveRawUtf8(fileNameLocal, utf8Buffer, false, isXml);
            if (success)
            {
              bytes = utf8Buffer.size();
              wxString message;
              message.Printf(
                _("%s saved in default encoding UTF-8: conversion to %s failed"), 
                fileName.c_str(),
                wideEncoding.c_str());
              messagePane(message, CONST_WARNING);
            }
            else
            {
              return false;
            }
          }
          else
          {
            size_t finalBufferLen = iconvBufferLen - iconvBufferLeft;

            std::ofstream ofs(fileNameLocal.c_str(), std::ios::out | std::ios::binary);
            if (!ofs)
            {
              delete[] finalBuffer;
              return false;
            }

            // iconv adds boms for UTF-16 & UTF-32 automatically

            ofs.write(finalBuffer, finalBufferLen);
            ofs.close();
            delete[] finalBuffer;
            bytes = finalBufferLen;
          }
        }
      }
      else // all other encodings handled by Libxml
      {
        XmlEncodingHandler::setUtf8(utf8Buffer);
        auto_ptr<WrapLibxml> wl(new WrapLibxml(libxmlNetAccess));
        
        WrapTempFileName sourceFileName(fileName);
        saveRawUtf8(sourceFileName.name(), utf8Buffer);
        int result = wl->saveEncodingFromFile(sourceFileName.name(), fileNameLocal, encoding);
        if (result == -1)
        {
          success = saveRawUtf8(fileNameLocal, utf8Buffer, false, isXml);
          if (success)
          {
            std::string libxmlError = wl->getLastError();
            bytes = utf8Buffer.size();
            wxString msg, wideEncoding, wideError;
            wideEncoding =
              wxString(encoding.c_str(), wxConvUTF8, encoding.size());
            wideError = wxString(libxmlError.c_str(), wxConvUTF8, libxmlError.size());
            if (wideError.empty())
              wideError = _("unknown error");

            msg.Printf(_("Cannot save document in %s: %s (saved in default encoding UTF-8)"),
              wideEncoding.c_str(), wideError.c_str());
            messagePane(msg, CONST_INFO);
          }
        }
        else
          bytes = result;
      }
    }
  } // try
  catch (std::bad_alloc&)
  {
    if (encoding != "UTF-8")
    {
      int answer = wxMessageBox(
        _("Out of memory: attempt to save in default encoding UTF-8?"),
        _("Out of memory"),
        wxYES_NO | wxCANCEL | wxICON_QUESTION,
        this);
      if (answer == wxCANCEL || answer == wxNO)
        return false;

      bool success = saveRawUtf8(fileNameLocal, utf8Buffer, false, isXml);
      if (success)
      {
        bytes = utf8Buffer.size();
        wxString message;
        message.Printf(
        _("%s saved in default encoding UTF-8"), 
          fileName.c_str());
          messagePane(message, CONST_INFO);
      }
      else
      {
        wxString message;
        message.Printf(_("Cannot save %s"), fileName.c_str());
        messagePane(message, CONST_STOP);
        return false;
      }
    }
  }

  doc->SetFocus();
  doc->SetSavePoint();

  if (properties.validateAsYouType && isXml)
    doc->shallowValidate(utf8Buffer.c_str(), utf8Buffer.size());
  
  if (!unlimitedUndo)
    doc->EmptyUndoBuffer();
  wxFileName fn(fileName);
  if (fn.IsOk())
    doc->setLastModified(fn.GetModificationTime());
  openFileSet.insert(fileName);
  displaySavedStatus(bytes);
  return true;
}

bool MyFrame::saveRawUtf8(
  const std::string& fileNameLocal,
  std::string& bufferUtf8,
  bool ignoreEncoding,
  bool isXml)
{
  ofstream ofs(fileNameLocal.c_str(), std::ios::out | std::ios::binary);
  if (!ofs)
    return false;

  if (!ignoreEncoding && isXml)
    XmlEncodingHandler::setUtf8(bufferUtf8, true);
  
  if (saveBom && isXml)
  {
    char bom[4];
    bom[0] = 0xEF;
    bom[1] = 0xBB;
    bom[2] = 0xBF;
    bom[3] = 0;
    ofs.write(bom, 3);
  }
  ofs.write(bufferUtf8.c_str(), bufferUtf8.size());
  ofs.close();
  return true;
}

void MyFrame::displaySavedStatus(int bytes)
{
  wxString unit;
  float result = 0;
  if (bytes > 1000000)
  {
    result = bytes / 1000000;
    unit = _("MB");
  }
  else if (bytes > 1000)
  {
    result = bytes / 1000;
    unit = _("kB");
  }
  else if (bytes >= 0)
  {
    result = bytes;
    unit = ngettext(L"byte", L"bytes", bytes);
  }
  else
    return;
    
  wxString msg;

  msg.Printf(
    _("%g %s saved"),
    result,
    unit.c_str());
  statusProgress(msg);
}

bool MyFrame::xpThemeActive()
{
#ifndef __WXMSW__
  return false;
#else
  return (wxUxThemeEngine::Get() && wxUxThemeEngine::Get()->IsThemeActive());
#endif
}

bool MyFrame::getHandleCommandLineFlag()
{
  return handleCommandLineFlag;
}

MyMenuBar *MyFrame::getMenuBar()
{
  fileMenu = new wxMenu; // use class-wide data member
  updateFileMenu(false);
  
  // edit menu
  wxMenu *editMenu = new wxMenu;

  wxMenuItem *undoItem =
    new wxMenuItem(NULL, wxID_UNDO, _("&Undo\tCtrl+Z"), _("Undo"));
  undoItem->SetBitmap(undo16Bitmap);

  wxMenuItem *redoItem =
    new wxMenuItem(NULL, wxID_REDO, _("&Redo\tCtrl+Y"), _("Redo"));
  redoItem->SetBitmap(redo16Bitmap);

  wxMenuItem *cutItem =
    new wxMenuItem(NULL, wxID_CUT, _("&Cut\tCtrl+X"), _("Cut"));
  cutItem->SetBitmap(cutBitmap);

  wxMenuItem *copyItem =
    new wxMenuItem(NULL, wxID_COPY, _("C&opy\tCtrl+C"), _("Copy"));
  copyItem->SetBitmap(copyBitmap);

  wxMenuItem *pasteItem =
    new wxMenuItem(NULL, wxID_PASTE, _("&Paste\tCtrl+V"), _("Paste"));
  pasteItem->SetBitmap(pasteBitmap);

  wxMenuItem *pasteNewDocumentItem =
    new wxMenuItem(
      NULL,
      ID_PASTE_NEW_DOCUMENT,
      _("P&aste As New Document"),
      _("Paste As New Document"));
  pasteNewDocumentItem->SetBitmap(wxNullBitmap);

  wxMenuItem *findItem =
    new wxMenuItem(NULL, ID_FIND, _("&Find...\tCtrl+F"), _("Find..."));
  findItem->SetBitmap(searchBitmap);

  wxMenuItem *findAgainItem =
    new wxMenuItem(NULL, ID_FIND_AGAIN, _("F&ind Again\tF3"), _("Find Again"));
  findAgainItem->SetBitmap(wxNullBitmap);

  wxMenuItem *replaceItem =
    new wxMenuItem(NULL, ID_REPLACE, _("&Replace...\tCtrl+R"), _("Replace..."));
  replaceItem->SetBitmap(wxNullBitmap);
  
  wxMenuItem *globalReplaceItem =
    new wxMenuItem(
      NULL,
      ID_GLOBAL_REPLACE,
      _("&Global Replace...\tCtrl+Shift+R"),
      _("Global Replace..."));
  globalReplaceItem->SetBitmap(wxNullBitmap);

  wxMenuItem *gotoItem =
    new wxMenuItem(NULL, ID_GOTO, _("G&o To...\tCtrl+G"), _("Go To..."));
  gotoItem->SetBitmap(wxNullBitmap);

  editMenu->Append(undoItem);
  editMenu->Append(redoItem);
  editMenu->AppendSeparator();
  editMenu->Append(cutItem);
  editMenu->Append(copyItem);
  editMenu->Append(pasteItem);
  editMenu->Append(pasteNewDocumentItem);
  editMenu->AppendSeparator();
  editMenu->Append(findItem);
  editMenu->Append(findAgainItem);
  editMenu->Append(replaceItem);
  editMenu->Append(globalReplaceItem);
  editMenu->AppendSeparator();
  editMenu->Append(gotoItem);
  
  // font size menu
  wxMenu *fontSizeMenu = new wxMenu;
  fontSizeMenu->Append(
    ID_FONT_LARGER, _("Increase\tCtrl+U"), _("Increase"));
  fontSizeMenu->Append(
    ID_FONT_SMALLER, _("Decrease\tCtrl+D"), _("Decrease"));
  fontSizeMenu->AppendSeparator();
  fontSizeMenu->Append(ID_FONT_NORMAL, _("Normal\tCtrl+0"), _("Normal"));
  
  // color scheme menu
  colorSchemeMenu = new wxMenu;
  colorSchemeMenu->AppendRadioItem(
    ID_COLOR_SCHEME_DEFAULT, _("&Default"), _("Default"));
  colorSchemeMenu->AppendRadioItem(
    ID_COLOR_SCHEME_REDUCED_GLARE,
    _("&Blue background, white text"),
    _("Blue background, white text"));
  colorSchemeMenu->AppendRadioItem(
    ID_COLOR_SCHEME_DEFAULT_BACKGROUND,
    _("&Light"),
    _("Light"));
  colorSchemeMenu->AppendRadioItem(
    ID_COLOR_SCHEME_NONE,
    _("&None"),
    _("None"));
    
  switch (properties.colorScheme)
  {
    case COLOR_SCHEME_DEFAULT:
      colorSchemeMenu->Check(ID_COLOR_SCHEME_DEFAULT, true);
      break;
    case COLOR_SCHEME_DEFAULT_BACKGROUND:
      colorSchemeMenu->Check(ID_COLOR_SCHEME_DEFAULT_BACKGROUND, true);
      break;
    case COLOR_SCHEME_REDUCED_GLARE:
      colorSchemeMenu->Check(ID_COLOR_SCHEME_REDUCED_GLARE, true);
      break;
    case COLOR_SCHEME_NONE:
      colorSchemeMenu->Check(ID_COLOR_SCHEME_NONE, true);
      break;
    default:
      break;
  }

  viewMenu = new wxMenu; // use class-wide data member
  viewMenu->Append(ID_PREVIOUS_DOCUMENT, _("&Previous Document\tCtrl+PgUp"), _("Previous Document"));
  viewMenu->Append(ID_NEXT_DOCUMENT, _("&Next Document\tCtrl+PgDn"), _("Next Document"));
  viewMenu->Append(ID_BROWSER, _("&Browser\tCtrl+B"), _("Browser"));
  viewMenu->AppendSeparator();
  viewMenu->AppendRadioItem(
    ID_SHOW_TAGS,
    _("&Show Tags and Attributes\tCtrl+T"), _("Show Tags and Attributes"));
  viewMenu->AppendRadioItem(
    ID_HIDE_ATTRIBUTES,
    _("&Hide Attributes Only\tCtrl+Shift+A"), _("Hide Attributes Only"));
  viewMenu->AppendRadioItem(
    ID_HIDE_TAGS,
    _("H&ide Tags and Attributes\tCtrl+Shift+T"), _("Hide Tags and Attributes"));

  switch (visibilityState)
  {
    case SHOW_TAGS:
      viewMenu->Check(ID_SHOW_TAGS, true);
      break;
    case HIDE_TAGS:
      viewMenu->Check(ID_HIDE_TAGS, true);
      break;
    case HIDE_ATTRIBUTES:
      viewMenu->Check(ID_HIDE_ATTRIBUTES, true);
      break;
    default:
      viewMenu->Check(ID_SHOW_TAGS, true);
      break;
  }   
  viewMenu->AppendSeparator();
  viewMenu->Append(
    ID_TOGGLE_FOLD, _("&Toggle Fold"), _("Toggle Fold"));
  viewMenu->Append(
    ID_FOLD_ALL, _("&Fold Tags\tCtrl+Shift+F"), _("Fold Tags"));
  viewMenu->Append(
    ID_UNFOLD_ALL, _("&Unfold Tags\tCtrl+Shift+U"), _T("Unfold Tags"));
  viewMenu->AppendSeparator();
  viewMenu->AppendCheckItem(
    ID_WRAP_WORDS, _("&Wrap Words\tCtrl+W"), _T("Wrap Words"));
  viewMenu->Check(ID_WRAP_WORDS, properties.wrap);
  viewMenu->Append(wxID_ANY, _("&Color Scheme"), colorSchemeMenu);
  viewMenu->Append(wxID_ANY, _("&Text Size"), fontSizeMenu);
  viewMenu->AppendSeparator();

  viewMenu->AppendCheckItem(
    ID_LOCATION_PANE_VISIBLE,
    _("S&how Current Element Pane"),
    _("Show Current ElementPane"));
  viewMenu->Check(ID_LOCATION_PANE_VISIBLE, false);
  viewMenu->AppendCheckItem(
    ID_TOOLBAR_VISIBLE, _("Sh&ow Toolbar"), _("Show Toolbar"));
  viewMenu->Check(ID_TOOLBAR_VISIBLE, toolbarVisible);
  viewMenu->Append(
    ID_HIDE_PANE, _("C&lose Message Pane\tAlt+C"), _("Close Message Pane"));

  // insert menu
  wxMenu *insertMenu = new wxMenu;
  insertMenu->Append(ID_INSERT_CHILD, _("&Element...\tCtrl+I"), _("Element..."));
  insertMenu->Append(ID_INSERT_SIBLING, _("&Sibling...\tCtrl+Shift+I"), _("Sibling..."));
  insertMenu->Append(ID_INSERT_ENTITY, _("&Entity...\tCtrl+E"), _("Entity..."));
  insertMenu->AppendSeparator();
  insertMenu->Append(ID_INSERT_TWIN, _("&Twin\tCtrl+Enter"), _("Twin"));
  insertMenu->AppendSeparator();
  insertMenu->Append(ID_INSERT_SYMBOL, _("S&ymbol..."), _("Symbol..."));

  // validation menu
  wxMenu *validationMenu = new wxMenu;
  validationMenu->Append(ID_VALIDATE_DTD, _("&DTD\tF4"), _("DTD"));
  validationMenu->Append(
    ID_VALIDATE_W3C_SCHEMA, _("&XML Schema\tF5"), _("XML Schema"));
  validationMenu->AppendSeparator();
  validationMenu->Append(
    ID_VALIDATE_RELAX_NG, _("&RELAX NG...\tF6"), _("RELAX NG..."));

  wxMenu *associateMenu = new wxMenu;
  associateMenu->Append(ID_ASSOCIATE_DTD_PUBLIC, _("&Public DTD..."), _("Public DTD..."));
  associateMenu->Append(ID_ASSOCIATE_DTD_SYSTEM, _("&System DTD..."), _("System DTD..."));
  associateMenu->Append(ID_ASSOCIATE_W3C_SCHEMA, _("&XML Schema..."), _("XML Schema..."));
  associateMenu->Append(ID_ASSOCIATE_XSL, _("XS&LT stylesheet..."), _("XSLT stylesheet..."));

  if (wxFileName::DirExists(rngDir))
  {
    wxString rngMask, rngFile, displayName, shortcutString;
    rngMask = rngDir + wxFileName::GetPathSeparator() + _T("*.rng");
    rngFile = wxFindFirstFile(rngMask, wxFILE);
  
    int id = ID_VALIDATE_PRESET1;
    
    if (!rngFile.empty())
    {
      validationPresetMap.insert(make_pair(id, rngFile));
      wxFileName::SplitPath(rngFile, NULL, NULL, &displayName, NULL);
      displayName.Replace(_T(".rng"), _T(""));
      shortcutString.Printf(_("\tCtrl+%i"), (id - ID_VALIDATE_PRESET1) + 1);
      
      validationMenu->Append(id, displayName + shortcutString, displayName);
      
      for (id = ID_VALIDATE_PRESET2; id <= ID_VALIDATE_PRESET9; ++id)
      {
        rngFile = wxFindNextFile();
        if (rngFile.empty())
          break;
        validationPresetMap.insert(make_pair(id, rngFile));
        wxFileName::SplitPath(rngFile, NULL, NULL, &displayName, NULL);
        shortcutString.Printf(_("\tCtrl+%i"), (id - ID_VALIDATE_PRESET1) + 1);
        displayName.Replace(_T(".rng"), _T(""));
        validationMenu->Append(id, displayName + shortcutString, displayName);
      }
    }
  }

  // xsl menu
  wxMenu *xslMenu = new wxMenu;
  xslMenu->Append(ID_XSLT, _("&XSL Transform...\tF8"),
    _("XSL Transform..."));
  xslMenu->AppendSeparator();
  xslMenu->Append(
    ID_XSLT_DOCBOOK_HTML,
    _("&DocBook to HTML\tAlt+1"), _("DocBook to HTML"));
  xslMenu->Append(
    ID_XSLT_DOCBOOK_XHTML,
    _("&DocBook to XHTML\tAlt+2"), _("DocBook to XHTML"));
  xslMenu->Append(
    ID_XSLT_DOCBOOK_FO,
    _("D&ocBook to XSL-FO\tAlt+3"), _("DocBook to XSL-FO"));
  xslMenu->Append(
    ID_XSLT_TEI_HTML,
    _("&TEI to HTML\tAlt+4"), _("TEI to HTML"));
  xslMenu->Append(
    ID_XSLT_TEI_LATEX,
    _("T&EI to LaTeX\tAlt+5"), _("TEI to LaTeX"));
  xslMenu->Append(
    ID_XSLT_TEI_XHTML,
    _("TE&I to XHTML\tAlt+6"), _("TEI to XHTML"));
  xslMenu->Append(
    ID_XSLT_TEI_FO,
    _("TEI to &XSL-FO\tAlt+7"), _("TEI to XSL-FO"));

  // xml menu
  xmlMenu = new wxMenu; // use class-wide data member
  xmlMenu->Append(
    ID_CHECK_WELLFORMED,
    _("&Check Well-formedness\tF2"), _("Check Well-formedness"));
  xmlMenu->Append(
    wxID_ANY,
    _("&Validate"),
    validationMenu);
  xmlMenu->AppendSeparator();
  xmlMenu->Append(
    wxID_ANY,
    _("&Associate"),
    associateMenu);
  xmlMenu->AppendSeparator();
  xmlMenu->Append(wxID_ANY, _("&XSLT"), xslMenu);
  xmlMenu->Append(
    ID_XPATH,
    _("&Evaluate XPath...\tF9"),
    _("Evaluate XPath..."));

  xmlMenu->AppendSeparator();
  xmlMenu->Append(
    ID_PRETTYPRINT,
    _("&Pretty-print\tF11"), _("Pretty-print"));
  xmlMenu->AppendSeparator();
  xmlMenu->AppendCheckItem(
    ID_PROTECT_TAGS,
    _("&Lock Tags\tCtrl+L"),
    _("Lock Tags"));
  xmlMenu->Check(ID_PROTECT_TAGS, protectTags);
  xmlMenu->AppendSeparator();
  xmlMenu->Append(
    ID_ENCODING,
    _("E&ncoding..."), _("Encoding..."));

  // tools menu
  wxMenu *toolsMenu = new wxMenu;

  wxMenuItem *spellingItem =
    new wxMenuItem(
      NULL,
      ID_SPELL,
      _("&Spelling and Style...\tF7"),
      _("Spelling and Style..."));
  spellingItem->SetBitmap(spelling16Bitmap);
  
  wxMenuItem *wordCountItem =
    new wxMenuItem(
      NULL,
      ID_WORD_COUNT,
      _("&Word Count"),
      _("Word Count"));
  wordCountItem->SetBitmap(wxNullBitmap);
  
  wxMenuItem *commandItem =
    new wxMenuItem(
      NULL,
      ID_COMMAND,
      _("&Command\tCtrl+Alt+C"),
      _("Command"));
  commandItem->SetBitmap(wxNullBitmap);

  wxMenuItem *optionsItem =
    new wxMenuItem(
      NULL,
      ID_OPTIONS,
      _("&Options..."),
      _("Options..."));
  optionsItem->SetBitmap(wxNullBitmap);

  toolsMenu->Append(spellingItem);
  toolsMenu->Append(wordCountItem);
  toolsMenu->AppendSeparator();
  toolsMenu->Append(commandItem);
  toolsMenu->AppendSeparator();
  toolsMenu->Append(optionsItem);

  // help menu
  wxMenu *helpMenu = new wxMenu;
    
  wxMenuItem *helpItem =
    new wxMenuItem(NULL, wxID_HELP,
    _("&XML Copy Editor Help\tF1"), _("Help"));
  helpItem->SetBitmap(helpBitmap);
  wxMenuItem *homeItem = 
    new wxMenuItem(NULL, ID_HOME,
    _("&Home Page"), _("Home Page"));
  homeItem->SetBitmap(wxNullBitmap);
  wxMenuItem *feedbackItem =
    new wxMenuItem(NULL, ID_FEEDBACK, _("&Forum"), _("Forum"));
  feedbackItem->SetBitmap(wxNullBitmap);
  wxMenuItem *aboutItem =
    new wxMenuItem(NULL, wxID_ABOUT,
    _("&About XML Copy Editor"), _("About"));
  aboutItem->SetBitmap(wxNullBitmap);
  wxMenuItem *downloadSourceItem =
    new wxMenuItem(NULL, ID_DOWNLOAD_SOURCE,
    _("&Browse Source"), _("Browse Source"));
  downloadSourceItem->SetBitmap(wxNullBitmap);
  helpMenu->Append(helpItem);
  helpMenu->AppendSeparator();
  helpMenu->Append(homeItem);
  helpMenu->Append(feedbackItem);
  helpMenu->Append(downloadSourceItem);
  helpMenu->AppendSeparator();
  helpMenu->Append(aboutItem);

  MyMenuBar *menuBar = new MyMenuBar(wxMB_DOCKABLE);
  menuBar->Append(fileMenu, _("&File"));
  menuBar->Append(editMenu, _("&Edit"));
  menuBar->Append(viewMenu, _("&View"));
  menuBar->Append(insertMenu, _("&Insert"));
  menuBar->Append(xmlMenu, _("&XML"));
  menuBar->Append(toolsMenu, _("&Tools"));
  menuBar->Append(helpMenu, _("&Help"));
  return menuBar;
}

void MyFrame::updateFileMenu(bool deleteExisting)
{
  if (deleteExisting)
  {
    wxMenuItemList list = fileMenu->GetMenuItems();
    size_t count = list.size();
    for (size_t i = 0; i < count; ++i)
      fileMenu->Delete(list[i]);
  }
  
  wxMenuItem *newItem =
    new wxMenuItem(NULL, wxID_NEW, _("&New...\tCtrl+N"), _("New..."));
  newItem->SetBitmap(new16Bitmap);
  wxMenuItem *openItem =
    new wxMenuItem(NULL, wxID_OPEN, _("&Open...\tCtrl+O"), _("Open..."));
  openItem->SetBitmap(open16Bitmap);
  wxMenuItem *openLargeFileItem =
    new wxMenuItem(NULL, ID_OPEN_LARGE_FILE,
      _("O&pen Large Document...\tCtrl+Shift+O"), _("Open Large Document..."));
  openLargeFileItem->SetBitmap(wxNullBitmap);
  wxMenuItem *closeItem =
    new wxMenuItem(NULL, wxID_CLOSE, _("&Close\tCtrl+F4"), _("Close"));
  closeItem->SetBitmap(wxNullBitmap);
  wxMenuItem *closeAllItem =
    new wxMenuItem(NULL, wxID_CLOSE_ALL, _("C&lose All"), _("Close All"));
  closeAllItem->SetBitmap(wxNullBitmap);
  wxMenuItem *saveItem =
    new wxMenuItem(NULL, wxID_SAVE, _("&Save\tCtrl+S"), _("Save"));
  saveItem->SetBitmap(save16Bitmap);

  wxMenuItem *saveAsItem =
    new wxMenuItem(NULL, wxID_SAVEAS, _("S&ave As...\tF12"), _("Save As..."));
  saveAsItem->SetBitmap(wxNullBitmap);
  wxMenuItem *revertItem =
    new wxMenuItem(NULL, wxID_REVERT, _("&Revert"), _("Revert"));
  revertItem->SetBitmap(wxNullBitmap);
  wxMenuItem *printSetupItem =
    new wxMenuItem(NULL, ID_PRINT_SETUP, _("Pa&ge Setup..."), _("Page Setup..."));
  printSetupItem->SetBitmap(wxNullBitmap);
  wxMenuItem *printPreviewItem =
    new wxMenuItem(NULL, ID_PRINT_PREVIEW, _("P&rint Preview..."), _("Print Preview..."));
  printPreviewItem->SetBitmap(printPreviewBitmap);
  wxMenuItem *printItem =
    new wxMenuItem(NULL, ID_PRINT, _("Pr&int...\tCtrl+P"), _("Print..."));
  printItem->SetBitmap(print16Bitmap);
  wxMenuItem *importMSWordItem = 
    new wxMenuItem(
      NULL, ID_IMPORT_MSWORD, _("I&mport Microsoft Word Document..."));
  importMSWordItem->SetBitmap(wxNullBitmap);
  wxMenuItem *exportMSWordItem = 
    new wxMenuItem(
      NULL, ID_EXPORT_MSWORD, _("&Export Microsoft Word Document..."));
  exportMSWordItem->SetBitmap(wxNullBitmap);
  
  wxMenuItem *exitItem =
    new wxMenuItem(NULL, wxID_EXIT, _("E&xit"), _("Exit"));
  exitItem->SetBitmap(wxNullBitmap);

  fileMenu->Append(newItem);
  fileMenu->Append(openItem);
  fileMenu->Append(openLargeFileItem);
  fileMenu->AppendSeparator();
  fileMenu->Append(closeItem);
  fileMenu->Append(closeAllItem);
  fileMenu->Append(saveItem);
  fileMenu->Append(saveAsItem);
  fileMenu->Append(revertItem);
  fileMenu->AppendSeparator();
  fileMenu->Append(printSetupItem);
  fileMenu->Append(printPreviewItem);
  fileMenu->Append(printItem);
#ifdef __WXMSW__
  fileMenu->AppendSeparator();
  fileMenu->Append(importMSWordItem);
  fileMenu->Append(exportMSWordItem);
#endif
  history.AddFilesToMenu(fileMenu);
  fileMenu->AppendSeparator();
  fileMenu->Append(exitItem);
}

MyToolBar *MyFrame::getToolBar()
{
  MyToolBar *myToolBar = new MyToolBar(
    this,
    ID_TOOLBAR,
    wxDefaultPosition,
    wxDefaultSize,
    wxTB_FLAT |
    wxTB_HORIZONTAL |
    wxTB_DOCKABLE);
  int w, h;
#ifdef __WXMSW__
  w = saveBitmap.GetWidth(), h = saveBitmap.GetHeight();
#else
  w = h = 24;
#endif
  myToolBar->SetToolBitmapSize(wxSize(w, h));

  myToolBar->AddTool(
    wxID_NEW,
    _("New"),
    newBitmap,
    _("New"));
  myToolBar->AddTool(
    wxID_OPEN,
    _("Open"),
    openBitmap,
    _("Open"));
  myToolBar->AddTool(
    wxID_SAVE,
    _("Save"),
    saveBitmap,
    wxNullBitmap,
    wxITEM_NORMAL,
    _("Save"));
  myToolBar->AddTool(
    ID_PRINT,
    _("Print"),
    printBitmap,
    wxNullBitmap,
    wxITEM_NORMAL,
    _("Print"));
  myToolBar->AddTool(
    ID_BROWSER,
    _("Browser"),
    internetBitmap,
    wxNullBitmap,
    wxITEM_NORMAL,
    _("Browser"));
  myToolBar->AddTool(
    ID_SPELL,
    _("Spelling and Style"),
    spellingBitmap,
    wxNullBitmap,
    wxITEM_NORMAL,
    _("Spelling and Style"));

  myToolBar->AddCheckTool(
    ID_PROTECT_TAGS,
    _("Lock Tags"),
    hyperlinkBitmap,
    wxNullBitmap,
    _("Lock Tags"));
  myToolBar->ToggleTool(
    ID_PROTECT_TAGS, protectTags);

  myToolBar->Realize();
  return myToolBar;
}

XmlDoc *MyFrame::getActiveDocument()
{
  if (!mainBook->GetPageCount())
    return NULL;
  return (XmlDoc *)mainBook->GetPage(mainBook->GetSelection());
}

void MyFrame::addSafeSeparator(wxToolBar *toolBar)
{
  if (xpThemeActive())
  {
    toolBar->AddSeparator();
    return;
  }
  wxStaticText *staticControl = new wxStaticText(
    toolBar,
    wxID_ANY,
    _T("  "));
  toolBar->AddControl(staticControl);
}

void MyFrame::statusProgress(const wxString& s)
{
  wxStatusBar *status = GetStatusBar();
  if (!status)
    return;
  status->SetStatusText(s, 0);
}

void MyFrame::messagePane(const wxString& s, int iconType)
{
  wxString paneTitle;
  switch (iconType)
  {
    case (CONST_INFO):
      if (s.Length() < 50) // magic no. necessary?
      {
        statusProgress(s);
        return;
      }
      paneTitle = _("Information");
      break;
    case (CONST_WARNING):
      paneTitle = _("Warning");
      break;
    case (CONST_STOP):
      paneTitle = _("Stopped");
      break;
    case (CONST_QUESTION):
      paneTitle = _("Question");
      break;
    default:
      paneTitle = _("Message");
      break;
  }

  wxAuiPaneInfo info = manager.GetPane(htmlReport);
  if (!info.IsShown())
  {
    manager.GetPane(htmlReport).Show(true);
    manager.Update();
  }

  manager.GetPane(htmlReport).Caption(paneTitle);

  wxString htmlString = s;
  htmlString.Replace(_T("&"), _T("&amp;"), true);
  htmlString.Replace(_T("<"), _T("&lt;"), true);
  htmlString.Replace(_T(">"), _T("&gt;"), true);

	wxString htmlBuffer;
	htmlBuffer += _T("<html><body><table><tr><td width=\"5%\"><img src=\"");
	switch (iconType)
	{
    case (CONST_INFO):
      htmlBuffer += pngDir;
      htmlBuffer += _T("stock_dialog-info-32.png");
      break;
    case (CONST_WARNING):
      htmlBuffer += pngDir;
      htmlBuffer += _T("stock_dialog-warning-32.png");
      break;
    case (CONST_STOP):
      htmlBuffer += pngDir;
      htmlBuffer += _T("stock_dialog-stop-32.png");
      break;
    case (CONST_QUESTION):
      htmlBuffer += pngDir;
      htmlBuffer += _T("stock_dialog-question-32.png");
      break;
    default:
	    break;
	}
	htmlBuffer += _T("\"></td><td width=\"95%\">");
	htmlBuffer += htmlString;
  htmlBuffer += _T("</td></tr></table></body></html>");

	htmlReport->SetPage(htmlBuffer);

	manager.Update();
}

void MyFrame::documentOk(const wxString& status)
{
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;
  wxString message;
  message.Printf(_("%s is %s"),
    doc->getShortFileName().c_str(),
    status.c_str());
  messagePane(message, CONST_INFO);
}

void MyFrame::applyEditorProperties(bool zoomOnly)
{
  XmlDoc *doc;
  size_t documentCount = mainBook->GetPageCount();
  for (size_t i = 0; i < documentCount; i++)
  {
    doc = (XmlDoc *)mainBook->GetPage(i);
    if (doc)
    {
      doc->applyProperties(properties, zoomOnly); 
      if (!properties.validateAsYouType)
        doc->clearErrorIndicators();
    }
  }
}

void MyFrame::modifiedMessage()
{
  messagePane(
    _("Document has been modified: save or discard changes"),
    CONST_STOP);
}

void MyFrame::xmliseWideTextNode(wxString& s)
{
  s.Replace(_T("&"), _T("&amp;"), true);
  s.Replace(_T("<"), _T("&lt;"), true);
  s.Replace(_T(">"), _T("&gt;"), true);
}

int MyFrame::getFileType(const wxString& fileName)
{
  std::string fileNameLocal, fileNameLocalLC;
  fileNameLocal = fileName.mb_str(wxConvLocal);
  fileNameLocalLC = CaseHandler::lowerCase(fileNameLocal);
  
  if (fileNameLocalLC.find(".dtd") != std::string::npos ||
    fileNameLocalLC.find(".ent") != std::string::npos)
    return FILE_TYPE_DTD;
  else if (fileNameLocalLC.find(".css") != std::string::npos)
    return FILE_TYPE_CSS;
  else if (fileNameLocalLC.find(".php") != std::string::npos)
    return FILE_TYPE_PHP;
  else if (fileNameLocalLC.find(".exe") != std::string::npos)
    return FILE_TYPE_BINARY;
  else if (fileNameLocalLC.find(".rnc") != std::string::npos)
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

bool MyFrame::isSpecialFileType(const wxString& fileName)
{
  std::string fileNameLocal, fileNameLocalLC;
  fileNameLocal = fileName.mb_str(wxConvLocal);
  fileNameLocalLC = CaseHandler::lowerCase(fileNameLocal);

  return (
    fileNameLocalLC.find(".dtd") != std::string::npos ||
    fileNameLocalLC.find(".css") != std::string::npos ||
    fileNameLocalLC.find(".php") != std::string::npos);
}

void MyFrame::encodingMessage()
{
  wxString msg = _("Encoding should be one of ");
  msg += ENCODING_INFO;
  messagePane(msg, CONST_STOP); 
}

void MyFrame::updatePaths()
{
  ruleSetDir = applicationDir + wxFileName::GetPathSeparator() + _T("rulesets");
  filterDir = applicationDir + wxFileName::GetPathSeparator() + _T("filters");
  templateDir = applicationDir + wxFileName::GetPathSeparator() + _T("templates") +
    wxFileName::GetPathSeparator();
  binDir = applicationDir + wxFileName::GetPathSeparator() + _T("bin") +
    wxFileName::GetPathSeparator();
  helpDir = applicationDir + wxFileName::GetPathSeparator() + _T("help") +
    wxFileName::GetPathSeparator();
  rngDir = applicationDir + wxFileName::GetPathSeparator() + _T("rng") +
    wxFileName::GetPathSeparator();
  htmlDir = applicationDir + wxFileName::GetPathSeparator() + _T("html") +
    wxFileName::GetPathSeparator();
  pngDir = applicationDir + wxFileName::GetPathSeparator() + _T("png") +
    wxFileName::GetPathSeparator();
  wxString wideCatalogPath =
    applicationDir + wxFileName::GetPathSeparator() + _T("catalog") +
    wxFileName::GetPathSeparator() + _T("catalog");
  catalogPath = wideCatalogPath.mb_str(wxConvLocal);
  wxString wideXslDtdPath =
    applicationDir + wxFileName::GetPathSeparator() + _T("dtd") +
    wxFileName::GetPathSeparator() + _T("xslt10.dtd");
  xslDtdPath = wideXslDtdPath.mb_str(wxConvLocal);
  wxString wideRssDtdPath =
    applicationDir + wxFileName::GetPathSeparator() + _T("dtd") +
      wxFileName::GetPathSeparator() + _T("rss2.dtd");
  rssDtdPath = wideRssDtdPath.mb_str(wxConvLocal);
  wxString wideXtmDtdPath =
    applicationDir + wxFileName::GetPathSeparator() + _T("dtd") + 
      wxFileName::GetPathSeparator() + _T("xtm1.dtd");
  xtmDtdPath = wideXtmDtdPath.mb_str(wxConvLocal);
  wxString wideLzxDtdPath =
    applicationDir + wxFileName::GetPathSeparator() + _T("dtd") + 
      wxFileName::GetPathSeparator() + _T("lzx.dtd");
  lzxDtdPath = wideLzxDtdPath.mb_str(wxConvLocal);
  wxString wideXliffDtdPath =
    applicationDir + wxFileName::GetPathSeparator() + _T("dtd") + 
      wxFileName::GetPathSeparator() + _T("xliff.dtd");
  xliffDtdPath = wideXliffDtdPath.mb_str(wxConvLocal);
}

void MyFrame::OnAssociate(wxCommandEvent& event)
{
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;

  wxString title, label, type, extension, path, defaulturl, defaultaux;
  wxString auxiliaryLabel;
  int id = event.GetId();
  switch (id)
  {
    case ID_ASSOCIATE_DTD_PUBLIC:
      type = _("Public DTD");
      extension = _T("*.dtd");
      defaulturl = lastDtdPublic;
      defaultaux = lastDtdPublicAux;
      break;
    case ID_ASSOCIATE_DTD_SYSTEM:
      type = _("System DTD");
      extension = _T("*.dtd");
      defaulturl = lastDtdSystem;
      defaultaux = _T("");
      break;
    case ID_ASSOCIATE_W3C_SCHEMA:
      type = _("XML Schema");
      extension = _T("*.xsd");
      defaulturl = lastSchema;
      defaultaux = _T("");
      break;
    case ID_ASSOCIATE_XSL:
      type = _("XSLT stylesheet");
      extension = _T("*.xsl;*.xslt");
      defaulturl = lastXslStylesheet;
      defaultaux = _T("");
      break;
    default:
      return;
  }

  std::string utf8Buffer;
  getRawText(doc, utf8Buffer);
  std::auto_ptr<WrapExpat> wellformedparser(new WrapExpat());
  if (!wellformedparser->parse(utf8Buffer))
  {
    std::string error = wellformedparser->getLastError();
    wxString wideError = wxString(error.c_str(), wxConvUTF8, error.size());
    wxString message;
    message.Printf(
      _("Cannot associate %s: %s"),
      type.c_str(),
      wideError.c_str());
    messagePane(message,
      CONST_STOP);
    return;
  }

  title.Printf(_("Associate %s"), type.c_str());
  label = _("Choose a file:");

  bool auxiliaryBox =
    (id == ID_ASSOCIATE_DTD_PUBLIC);
    //(id == ID_ASSOCIATE_W3C_SCHEMA_NS || id == ID_ASSOCIATE_DTD_PUBLIC);
  if (auxiliaryBox)
  {
    auxiliaryLabel = _("Choose a public identifier:");
/*
      (id == ID_ASSOCIATE_DTD_PUBLIC) ? _("Choose a public identifier:") :
        _("Choose a namespace:");
*/
  }
  
  AssociateDialog ad(
    this,
    title,
    label,
    type,
    extension,
    defaulturl,
    auxiliaryBox,
    auxiliaryLabel,
    defaultaux);
  if (ad.ShowModal() != wxID_OK)
    return;

  path = ad.getUrl();

  wxString aux, schemaPathMemory;
  
  if (auxiliaryBox)
  {
    aux = ad.getAux();
  }

  std::string utf8Path = (const char *)path.mb_str(wxConvUTF8);
  
  std::string modifiedBuffer;
  
  // remember choice
  switch (id)
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

  if (id == ID_ASSOCIATE_W3C_SCHEMA)
  {
    std::auto_ptr<XmlAssociateXsd> parser(new XmlAssociateXsd(utf8Path));
    if (!parser->parse(utf8Buffer))
      return;
    modifiedBuffer = parser->getBuffer();
  }
  else if (id == ID_ASSOCIATE_DTD_SYSTEM || id == ID_ASSOCIATE_DTD_PUBLIC)
  {
    std::auto_ptr<XmlAssociateDtd> parser(new XmlAssociateDtd(
      utf8Path,
      (auxiliaryBox) ? (const char *)aux.mb_str(wxConvUTF8) : ""));
    if (!parser->parse(utf8Buffer))
      return;
    modifiedBuffer = parser->getBuffer();
  }
  else if (id == ID_ASSOCIATE_XSL)
  {
    std::auto_ptr<XmlAssociateXsl> parser(new XmlAssociateXsl(
      utf8Path));
    if (!parser->parse(utf8Buffer))
      return;
    modifiedBuffer = parser->getBuffer();
  }
  else
    return;
  doc->SetTextRaw(modifiedBuffer.c_str());
  doc->SetFocus();
}

void MyFrame::openRememberedTabs()
{
  if (openTabsOnClose.empty())
    return;
  wchar_t *s, *it;
  s = it = (wchar_t *)openTabsOnClose.wc_str();
  std::vector<wxString> v;
  wxString buffer = wxEmptyString;
  for (; *it; it++)
  {
    if (*it == L'|')
    {
      if (!buffer.empty()) { v.push_back(buffer); }
      buffer = wxEmptyString;
    }
    else
      buffer += *it; 
  }
  if (!buffer.empty()) { v.push_back(buffer); }

  if (v.empty())
    return;

  std::vector<wxString>::iterator vit;
  for (vit = v.begin(); vit != v.end(); vit++)
  {
    if (!openFile(*vit))
      break;
  }
  XmlDoc *doc;
  if ((doc = getActiveDocument()) != NULL)
    doc->SetFocus();
}

void MyFrame::getRawText(XmlDoc *doc, std::string& buffer)
{
  if (!doc)
  {
    buffer = "";
    return;
  }
  //wxString wideBuffer = doc->GetText();
  //buffer = wideBuffer.mb_str(wxConvUTF8);
  buffer = doc->myGetTextRaw();
}

void MyFrame::OnWordCount(wxCommandEvent& event)
{
  XmlDoc *doc;
  if ((doc = getActiveDocument()) == NULL)
    return;
  wxString wideBuffer;
  std::string buffer;
  wideBuffer = doc->GetText();
  buffer = wideBuffer.mb_str(wxConvUTF8);

  auto_ptr<XmlWordCount> xwc(new XmlWordCount());
  wxString msg;
  if (!xwc->parse(buffer.c_str()))
  {
    std::string error = xwc->getLastError();
    wxString werror = wxString(error.c_str(), wxConvUTF8, error.size());
    statusProgress(wxEmptyString);

    msg.Printf(_("Cannot count words: %s"), werror.c_str());
    messagePane(msg, CONST_STOP);
    return;
  }
  
  int count = xwc->getWordCount();

  msg.Printf(
    ngettext(L"%s contains %i word", L"%s contains %i words", count),
      doc->getShortFileName().c_str(), count);

  messagePane(msg, CONST_INFO);
  doc->SetFocus();
}

void MyFrame::removeUtf8Bom(std::string& buffer)
{
  if (buffer.size() > 3 &&
    (unsigned char)buffer[0] == 0xEF &&
    (unsigned char)buffer[1] == 0xBB &&
    (unsigned char)buffer[2] == 0xBF)
  {
    buffer.erase(0, 3);
  }
}
    
void MyFrame::loadBitmaps()
{
#ifdef __WXMSW__
  // toolbar icons
  newBitmap = wxBITMAP(stock_new);
  openBitmap = wxBITMAP(stock_open);
  saveBitmap = wxBITMAP(stock_save);
  printBitmap = wxBITMAP(stock_print);
  spellingBitmap = wxBITMAP(stock_spellcheck);
  internetBitmap = wxBITMAP(stock_internet);
  hyperlinkBitmap = wxBITMAP(stock_hyperlink);
  filtersBitmap = wxBITMAP(stock_filters);

  // menu icons
  new16Bitmap = wxBITMAP(stock_new_16);
  open16Bitmap = wxBITMAP(stock_open_16);
  save16Bitmap = wxBITMAP(stock_save_16);
  printPreviewBitmap = wxBITMAP(stock_print_preview_16);
  print16Bitmap = wxBITMAP(stock_print_16);
  undo16Bitmap = wxBITMAP(stock_undo_16);
  redo16Bitmap = wxBITMAP(stock_redo_16);
  cutBitmap = wxBITMAP(stock_cut_16);
  copyBitmap = wxBITMAP(stock_copy_16);
  pasteBitmap = wxBITMAP(stock_paste_16);
  findBitmap = wxBITMAP(stock_search_16);
  spelling16Bitmap = wxBITMAP(stock_spellcheck_16);
  helpBitmap = wxBITMAP(stock_help_16);
#else
  // toolbar icons
  newBitmap.LoadFile(pngDir + _T("stock_new.png"), wxBITMAP_TYPE_PNG);
  openBitmap.LoadFile(pngDir + _T("stock_open.png"), wxBITMAP_TYPE_PNG);
  saveBitmap.LoadFile(pngDir + _T("stock_save.png"), wxBITMAP_TYPE_PNG);
  printBitmap.LoadFile(pngDir + _T("stock_print.png"), wxBITMAP_TYPE_PNG);
  spellingBitmap.LoadFile(pngDir + _T("stock_spellcheck.png"), wxBITMAP_TYPE_PNG);
  internetBitmap.LoadFile(pngDir + _T("stock_internet.png"), wxBITMAP_TYPE_PNG);
  hyperlinkBitmap.LoadFile(pngDir + _T("stock_hyperlink.png"), wxBITMAP_TYPE_PNG);
  filtersBitmap.LoadFile(pngDir + _T("stock_filters.png"), wxBITMAP_TYPE_PNG);

  // menu icons
  new16Bitmap.LoadFile(pngDir + _T("stock_new-16.png"), wxBITMAP_TYPE_PNG);
  open16Bitmap.LoadFile(pngDir + _T("stock_open-16.png"), wxBITMAP_TYPE_PNG);
  save16Bitmap.LoadFile(pngDir + _T("stock_save-16.png"), wxBITMAP_TYPE_PNG);
  printPreviewBitmap.LoadFile(pngDir + _T("stock_print_preview.png"), wxBITMAP_TYPE_PNG);
  print16Bitmap.LoadFile(pngDir + _T("stock_print-16.png"), wxBITMAP_TYPE_PNG);
  undo16Bitmap.LoadFile(pngDir + _T("stock_undo-16.png"), wxBITMAP_TYPE_PNG);
  redo16Bitmap.LoadFile(pngDir + _T("stock_redo-16.png"), wxBITMAP_TYPE_PNG);
  cutBitmap.LoadFile(pngDir + _T("stock_cut-16.png"), wxBITMAP_TYPE_PNG);
  copyBitmap.LoadFile(pngDir + _T("stock_copy-16.png"), wxBITMAP_TYPE_PNG);
  pasteBitmap.LoadFile(pngDir + _T("stock_paste-16.png"), wxBITMAP_TYPE_PNG);
  findBitmap.LoadFile(pngDir + _T("stock_find-16.png"), wxBITMAP_TYPE_PNG);
  spelling16Bitmap.LoadFile(pngDir + _T("stock_spellcheck-16.png"), wxBITMAP_TYPE_PNG);
  helpBitmap.LoadFile(pngDir + _T("stock_help-16.png"), wxBITMAP_TYPE_PNG);
#endif
}

#ifdef __WXMSW__
void MyFrame::OnDropFiles(wxDropFilesEvent& event)
{
  int no = event.GetNumberOfFiles();
  wxString *iterator = event.GetFiles();

  if (!no || !iterator)
    return;

  for (int i = 0; i < no; i++, iterator++)
  {
    if (!openFile(*iterator))
      break;
  }
}
#endif

std::string MyFrame::getAuxPath(const std::string& fileName)
{
  if (fileName.find(".xsl") != std::string::npos ||
    fileName.find(".XSL") != std::string::npos)
    return xslDtdPath;
  else if (fileName.find(".rss") != std::string::npos ||
    fileName.find(".RSS") != std::string::npos)
    return rssDtdPath;
  else if (fileName.find(".xtm") != std::string::npos ||
    fileName.find(".xtmm") != std::string::npos ||
    fileName.find(".XTM") != std::string::npos ||
    fileName.find(".XTMM") != std::string::npos)
    return xtmDtdPath;
  else if (fileName.find(".lzx") != std::string::npos ||
    fileName.find(".LZX") != std::string::npos)
    return lzxDtdPath;
  else if (fileName.find(".xlf") != std::string::npos ||
    fileName.find(".XLF") != std::string::npos)
    return xliffDtdPath;
  return "";
}

void MyFrame::OnActivateApp(wxActivateEvent& event)
{
  event.Skip();
  if (!mainBook || !event.GetActive())
    return;
  restoreFocusToNotebook = true;
}

void MyFrame::OnIconize(wxIconizeEvent& event)
{
  event.Skip();
  if (event.Iconized())
    return;
  restoreFocusToNotebook = true;
}

void MyFrame::OnKeyPressed(wxKeyEvent& event)
{
  event.Skip();
}

void MyFrame::setStrictScrolling(bool b)
{
  XmlDoc *doc;
  doc = getActiveDocument();
  if (!doc)
    return;
  doc->SetYCaretPolicy((b) ? (wxSTC_CARET_STRICT | wxSTC_CARET_SLOP) : wxSTC_CARET_EVEN,
    (b) ? 10 : 0);
}
