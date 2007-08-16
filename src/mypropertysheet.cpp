#include <wx/bookctrl.h>
#include <wx/fontenum.h>
#include "mypropertysheet.h"

BEGIN_EVENT_TABLE(MyPropertySheet, wxPropertySheetDialog)
  EVT_BUTTON(wxID_OK, MyPropertySheet::OnOk)
  EVT_BUTTON(
    APPLICATION_DIR_BROWSE,
    MyPropertySheet::OnApplicationDirBrowse)
  EVT_BUTTON(
    BROWSER_COMMAND_BROWSE,
    MyPropertySheet::OnBrowserCommandBrowse)
END_EVENT_TABLE()

MyPropertySheet::MyPropertySheet(
      wxWindow *parent,
      XmlCtrlProperties& propertiesParameter,
      wxString& applicationDirParameter,
      wxString& browserCommandParameter,
      bool rememberOpenTabsParameter,
      bool libxmlNetAccessParameter,
      bool singleInstanceCheckParameter,
      bool saveBomParameter,
      bool unlimitedUndoParameter,
      bool restoreLayoutParameter,
      bool expandInternalEntitiesParameter,
      bool showFullPathOnFrameParameter,
      int lang,
#ifdef __WXMSW__
      bool useCoolBarParameter,
#endif      
      wxWindowID id,
      wxString title,
      const wxPoint& position,
      const wxSize& size,
      long style) : wxPropertySheetDialog(
        parent, id, title, position, size, style), 
      properties(propertiesParameter),
      applicationDir(applicationDirParameter),
      browserCommand(browserCommandParameter)
{
  CreateButtons(wxOK | wxCANCEL);

  // editor
  wxPanel *editorPanel = new wxPanel(GetBookCtrl());
  wxBoxSizer *editorSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *col1sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *col2sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *tablesizer = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText *labelFont = new wxStaticText(
    editorPanel,
    wxID_ANY,
    _("Font"));
  fontBox = new wxChoice(
    editorPanel,
    wxID_ANY);

  wxArrayString fontArray;
  fontArray = wxFontEnumerator::GetFacenames();
  fontArray.Sort();
  for (size_t i = 0; i < fontArray.GetCount(); i++)
  {
    fontBox->Insert(fontArray.Item(i), i);
  }
  fontBox->SetStringSelection(properties.font);

  deleteWholeTagBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("I&ntelligent backspace/delete"));
  deleteWholeTagBox->SetValue(properties.deleteWholeTag);
  completionBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("&Tag completion"));
  completionBox->SetValue(properties.completion);
  foldBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("&Folding"));
  foldBox->SetValue(properties.fold);
  currentLineBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("&Highlight current line"));
  currentLineBox->SetValue(properties.currentLine);
  indentLinesBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("&Indentation guides"));
  indentLinesBox->SetValue(properties.indentLines);
  insertCloseTagBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("&Always insert closing tag"));
  insertCloseTagBox->SetValue(properties.insertCloseTag);
  highlightSyntaxBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("Hi&ghlight syntax"));
  highlightSyntaxBox->SetValue(properties.highlightSyntax);
  numberBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("&Line numbers"));
  numberBox->SetValue(properties.number);
  protectHiddenElementsBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("L&ock hidden tags"));
  protectHiddenElementsBox->SetValue(properties.protectHiddenElements);
  whitespaceVisibleBox = 
    new wxCheckBox(editorPanel, wxID_ANY, _("&White space visible"));
  whitespaceVisibleBox->SetValue(properties.whitespaceVisible);
  validateAsYouTypeBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("&Validate as you type"));
  validateAsYouTypeBox->SetValue(properties.validateAsYouType);
  toggleLineBackgroundBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("Va&riable highlight in tag free view"));
  toggleLineBackgroundBox->SetValue(properties.toggleLineBackground);

  /*
  wrapBox =
    new wxCheckBox(editorPanel, wxID_ANY, _("Wra&p lines"));
  wrapBox->SetValue(properties.wrap);
  */

  col1sizer->Add(insertCloseTagBox, 0, wxALL | wxALIGN_LEFT, 5);
  col1sizer->Add(foldBox, 0, wxALL | wxALIGN_LEFT, 5);
  col1sizer->Add(currentLineBox, 0, wxALL | wxALIGN_LEFT, 5);
  col1sizer->Add(highlightSyntaxBox, 0, wxALL | wxALIGN_LEFT, 5);
  col1sizer->Add(indentLinesBox, 0, wxALL | wxALIGN_LEFT, 5);
  col1sizer->Add(deleteWholeTagBox, 0, wxALL | wxALIGN_LEFT, 5);
  col2sizer->Add(numberBox, 0, wxALL | wxALIGN_LEFT, 5);
  col2sizer->Add(protectHiddenElementsBox, 0, wxALL | wxALIGN_LEFT, 5);
  col2sizer->Add(completionBox, 0, wxALL | wxALIGN_LEFT, 5);
  col2sizer->Add(validateAsYouTypeBox, 0, wxALL | wxALIGN_LEFT, 5);
  col2sizer->Add(toggleLineBackgroundBox, 0, wxALL | wxALIGN_LEFT, 5);
  col2sizer->Add(whitespaceVisibleBox, 0, wxALL | wxALIGN_LEFT, 5);
  tablesizer->Add(col1sizer, 0, wxALL | wxALIGN_LEFT, 0);
  tablesizer->Add(col2sizer, 0, wxALL | wxALIGN_LEFT, 0);
  editorSizer->Add(tablesizer, 0, wxALL | wxALIGN_LEFT, 0);
  editorSizer->Add(labelFont, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 5);
  editorSizer->Add(fontBox, 0, wxALL | wxALIGN_LEFT, 5);
  editorPanel->SetSizer(editorSizer);
  
  // general
  wxPanel *generalPanel = new wxPanel(GetBookCtrl());
  wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *hsizerBrowser = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *hsizerCheckboxes = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *vsizerCheckbox1 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *vsizerCheckbox2 = new wxBoxSizer(wxVERTICAL);
  
  wxStaticText *label = new wxStaticText(
    generalPanel,
    wxID_ANY,
    _("Application directory"));
  applicationDirEdit = new wxTextCtrl(
    generalPanel,
    wxID_ANY,
    applicationDir,
    wxDefaultPosition,
    wxSize(240, -1));
  wxButton *browse = new wxButton(
    generalPanel,
    APPLICATION_DIR_BROWSE,
    _("Browse"),
    wxDefaultPosition,
    wxSize(-1, applicationDirEdit->GetSize().GetHeight()));
  wxStaticText *labelBrowser = new wxStaticText(
    generalPanel,
    wxID_ANY,
    _("Browser"));
  browserCommandEdit = new wxTextCtrl(
    generalPanel,
    wxID_ANY,
    browserCommand,
    wxDefaultPosition,
    wxSize(240, -1));
  wxButton *browseCommand = new wxButton(
    generalPanel,
    BROWSER_COMMAND_BROWSE,
    _("Browse"),
    wxDefaultPosition,
    wxSize(-1, browserCommandEdit->GetSize().GetHeight()));

  wxStaticText *labelLanguage = new wxStaticText(
    generalPanel,
    wxID_ANY,
    _("Language (restart required)"));
  languageBox = new wxChoice(
    generalPanel,
    wxID_ANY);

  languageBox->Insert(_T("Chinese Simplified"), INDEX_CHINESE_SIMPLIFIED);
  languageBox->Insert(_T("Chinese Traditional"), INDEX_CHINESE_TRADITIONAL);
  languageBox->Insert(_T("English (US)"), INDEX_ENGLISH_US);
  languageBox->Insert(_T("French"), INDEX_FRENCH);
  languageBox->Insert(_T("German"), INDEX_GERMAN);
  languageBox->Insert(_T("Slovak"), INDEX_SLOVAK);
  languageBox->Insert(_T("Swedish"), INDEX_SWEDISH);
  
  switch (lang)
  {
    case wxLANGUAGE_GERMAN:
      languageBox->SetSelection(INDEX_GERMAN);
      break;
    case wxLANGUAGE_FRENCH:
      languageBox->SetSelection(INDEX_FRENCH);
      break;
    case wxLANGUAGE_SLOVAK:
      languageBox->SetSelection(INDEX_SLOVAK);
      break;
    case wxLANGUAGE_SWEDISH:
      languageBox->SetSelection(INDEX_SWEDISH);
      break;
    case wxLANGUAGE_CHINESE_SIMPLIFIED:
      languageBox->SetSelection(INDEX_CHINESE_SIMPLIFIED);
      break;
    case wxLANGUAGE_CHINESE_TRADITIONAL:
      languageBox->SetSelection(INDEX_CHINESE_TRADITIONAL);
      break;
    default:
      languageBox->SetSelection(INDEX_ENGLISH_US); 
  }
  
  libxmlNetAccessBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("&Enable network access for DTD validation"));
  libxmlNetAccessBox->SetValue(libxmlNetAccessParameter);
  expandInternalEntitiesBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("E&xpand internal entities on open"));
  expandInternalEntitiesBox->SetValue(expandInternalEntitiesParameter);
  singleInstanceCheckBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("&One application instance only"));
  singleInstanceCheckBox->SetValue(singleInstanceCheckParameter);
  restoreLayoutBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("Re&member layout on close"));
  restoreLayoutBox->SetValue(restoreLayoutParameter);
  rememberOpenTabsBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("&Remember open tabs on close"));
  rememberOpenTabsBox->SetValue(rememberOpenTabsParameter);
  unlimitedUndoBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("Re&tain undo history on save"));
  unlimitedUndoBox->SetValue(unlimitedUndoParameter);

  saveBomBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("&Save UTF-8 byte order mark"));
  saveBomBox->SetValue(saveBomParameter);

  fullPathBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("S&how full path on frame"));
  fullPathBox->SetValue(showFullPathOnFrameParameter);

#ifdef __WXMSW__
  useCoolBarBox = new wxCheckBox(
    generalPanel, wxID_ANY, _("&Use Microsoft rebar control (restart required)"));
  useCoolBarBox->SetValue(useCoolBarParameter);
#endif  
  
  hsizer->Add(applicationDirEdit, 0, wxALL | wxALIGN_LEFT, 0);
  hsizer->Add(browse, 0, wxLEFT | wxALIGN_LEFT, 5);
  hsizerBrowser->Add(browserCommandEdit, 0, wxALL | wxALIGN_LEFT, 0);
  hsizerBrowser->Add(browseCommand, 0, wxLEFT | wxALIGN_LEFT, 5);
  vsizer->Add(label, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 5);
  vsizer->Add(hsizer, 0, wxALL | wxALIGN_LEFT, 5);
  vsizer->Add(labelBrowser, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 5);
  vsizer->Add(hsizerBrowser, 0, wxALL | wxALIGN_LEFT, 5);

  vsizer->Add(labelLanguage, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 5);
  vsizer->Add(languageBox, 0, wxALL | wxALIGN_LEFT, 5);

  vsizerCheckbox1->Add(libxmlNetAccessBox, 0, wxALL | wxALIGN_LEFT, 5);
  vsizerCheckbox1->Add(expandInternalEntitiesBox, 0, wxALL | wxALIGN_LEFT, 5);
  vsizerCheckbox1->Add(singleInstanceCheckBox, 0, wxALL | wxALIGN_LEFT, 5);
  vsizerCheckbox1->Add(restoreLayoutBox, 0, wxALL | wxALIGN_LEFT, 5);
  vsizerCheckbox1->Add(rememberOpenTabsBox, 0, wxALL | wxALIGN_LEFT, 5);
  vsizerCheckbox2->Add(unlimitedUndoBox, 0, wxALL | wxALIGN_LEFT, 5);
  vsizerCheckbox2->Add(saveBomBox, 0, wxALL | wxALIGN_LEFT, 5);
  vsizerCheckbox2->Add(fullPathBox, 0, wxALL | wxALIGN_LEFT, 5);
#ifdef __WXMSW__
  vsizerCheckbox2->Add(useCoolBarBox, 0, wxALL | wxALIGN_LEFT, 5);
#endif
  hsizerCheckboxes->Add(vsizerCheckbox1, 0, wxALL | wxALIGN_LEFT, 0);
  hsizerCheckboxes->Add(vsizerCheckbox2, 0, wxALL | wxALIGN_LEFT, 0);
  vsizer->Add(hsizerCheckboxes, 0, wxALL | wxALIGN_LEFT, 5);
  generalPanel->SetSizer(vsizer);

  editorSizer->Layout();
  vsizer->Layout();
  GetBookCtrl()->AddPage(generalPanel, _("General"));
  GetBookCtrl()->AddPage(editorPanel, _("Editor"));

  LayoutDialog();
}

MyPropertySheet::~MyPropertySheet() {}

void MyPropertySheet::OnOk(wxCommandEvent& e)
{
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

  wxString testDir = applicationDirEdit->GetValue();
  if (!wxFileName::DirExists(testDir))
  {
    wxMessageBox(_("Cannot access application directory"), _("Options"));
    // tbd: show general tab
    return;
  }
  else
    applicationDir = testDir;
  
  browserCommand = browserCommandEdit->GetValue(); // permit incorrect value
  singleInstanceCheck = singleInstanceCheckBox->GetValue();
  restoreLayout = restoreLayoutBox->GetValue();
  rememberOpenTabs = rememberOpenTabsBox->GetValue();
  libxmlNetAccess = libxmlNetAccessBox->GetValue();
  saveBom = saveBomBox->GetValue();
  unlimitedUndo = unlimitedUndoBox->GetValue();
  expandInternalEntities = expandInternalEntitiesBox->GetValue();
  showFullPathOnFrame = fullPathBox->GetValue();
#ifdef __WXMSW__
  useCoolBar = useCoolBarBox->GetValue();
#endif

  int languageChoice = languageBox->GetSelection();
  switch (languageChoice)
  {
    case INDEX_GERMAN:
      lang = wxLANGUAGE_GERMAN;
      break;
    case INDEX_CHINESE_SIMPLIFIED:
      lang = wxLANGUAGE_CHINESE_SIMPLIFIED;
      break;
    case INDEX_CHINESE_TRADITIONAL:
      lang = wxLANGUAGE_CHINESE_TRADITIONAL;
      break;
    case INDEX_FRENCH:
      lang = wxLANGUAGE_FRENCH;
      break;
    case INDEX_SLOVAK:
      lang = wxLANGUAGE_SLOVAK;
      break;
    case INDEX_SWEDISH:
      lang = wxLANGUAGE_SWEDISH;
      break;
    default:
      lang = wxLANGUAGE_ENGLISH_US;
      break;
  }
  e.Skip();
}

void MyPropertySheet::OnApplicationDirBrowse(wxCommandEvent& e)
{
  wxDirDialog *browseDialog = new wxDirDialog(this);
  if (browseDialog->ShowModal() == wxID_OK)
    applicationDirEdit->SetValue(browseDialog->GetPath()); 
}

void MyPropertySheet::OnBrowserCommandBrowse(wxCommandEvent& e)
{
  wxFileDialog *browseDialog = new wxFileDialog(this);
  if (browseDialog->ShowModal() == wxID_OK)
    browserCommandEdit->SetValue(browseDialog->GetPath()); 
}

XmlCtrlProperties MyPropertySheet::getProperties()
{
  return properties;
}

wxString MyPropertySheet::getApplicationDir()
{
  return applicationDir; 
}

wxString MyPropertySheet::getBrowserCommand()
{
  return browserCommand; 
}

bool MyPropertySheet::getSingleInstanceCheck()
{
  return singleInstanceCheck; 
}

bool MyPropertySheet::getRememberOpenTabs()
{
  return rememberOpenTabs; 
}

bool MyPropertySheet::getLibxmlNetAccess()
{
  return libxmlNetAccess;
}

bool MyPropertySheet::getSaveBom()
{
  return saveBom;
}

bool MyPropertySheet::getUnlimitedUndo()
{
  return unlimitedUndo;
}

#ifdef __WXMSW__
bool MyPropertySheet::getUseCoolBar()
{
  return useCoolBar;
}
#endif

int MyPropertySheet::getLang()
{
  return lang;
}

bool MyPropertySheet::getRestoreLayout()
{
  return restoreLayout;
}

bool MyPropertySheet::getExpandInternalEntities()
{
  return expandInternalEntities; 
}

bool MyPropertySheet::getShowFullPathOnFrame()
{
  return showFullPathOnFrame;
}
