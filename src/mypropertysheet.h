#ifndef MYPROPERTYSHEET_H
#define MYPROPERTYSHEET_H

#include <wx/wx.h>
#include <wx/generic/propdlg.h>
#include <wx/filename.h>
#include "xmlctrl.h"

enum {
  APPLICATION_DIR_BROWSE,
  BROWSER_COMMAND_BROWSE,
  FONT_BROWSE
};

enum langIndex {
  INDEX_CHINESE_SIMPLIFIED = 0,
  INDEX_CHINESE_TRADITIONAL,
  INDEX_ENGLISH_US,
  INDEX_FRENCH,
  INDEX_GERMAN,
  INDEX_SLOVAK,
  INDEX_SWEDISH,
};

class MyPropertySheet : public wxPropertySheetDialog
{
  public:
    MyPropertySheet(
      wxWindow *parent,
      XmlCtrlProperties& propertiesParameter,
      wxString& applicationDirParameter,
      wxString& browserCommandParameter,
      bool rememberOpenTabsParameter,
      bool libxmlNetAccess,
      bool singleInstanceCheck,
      bool saveBom,
      bool unlimitedUndo,
      bool restoreLayout,
      bool expandInternalEntities,
      bool showFullPathOnFrame,
      int lang,
#ifdef __WXMSW__
      bool useCoolBar,
#endif      
      wxWindowID id = wxID_ANY,
      wxString title = _T(""),
      const wxPoint& position = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_DIALOG_STYLE);
    ~MyPropertySheet();
    void OnOk(wxCommandEvent& e);
    void OnBrowserCommandBrowse(wxCommandEvent& e);
    void OnApplicationDirBrowse(wxCommandEvent& e);
    XmlCtrlProperties getProperties();
    wxString getApplicationDir();
    wxString getBrowserCommand();
    bool getSingleInstanceCheck();
    bool getRememberOpenTabs();
    bool getLibxmlNetAccess();
    bool getSaveBom();
    bool getUnlimitedUndo();
    bool getRestoreLayout();
    bool getExpandInternalEntities();
    bool getShowFullPathOnFrame();
#ifdef __WXMSW__
    bool getUseCoolBar();
#endif
    int getLang();
  private:
    int lang;
    wxCheckBox *completionBox,
      *currentLineBox,
      *foldBox,
      *numberBox,
      *whitespaceVisibleBox,
      //*wrapBox,
      *indentLinesBox,
      *deleteWholeTagBox,
      *rememberOpenTabsBox,
      *libxmlNetAccessBox,
      *singleInstanceCheckBox,
      *saveBomBox,
      *protectHiddenElementsBox,
      *toggleLineBackgroundBox,
      *validateAsYouTypeBox,
      *fullPathBox,
      *highlightSyntaxBox,
#ifdef __WXMSW__
      *useCoolBarBox,
#endif
      *unlimitedUndoBox,
      *restoreLayoutBox,
      *expandInternalEntitiesBox,
      *insertCloseTagBox;
    wxChoice *languageBox, *fontBox;
    wxTextCtrl *applicationDirEdit, *browserCommandEdit;
    XmlCtrlProperties properties;
    wxString applicationDir, browserCommand;
    bool singleInstanceCheck,
      rememberOpenTabs,
      libxmlNetAccess,
      saveBom,
#ifdef __WXMSW__
      useCoolBar,
#endif      
      unlimitedUndo,
      restoreLayout,
      expandInternalEntities,
      showFullPathOnFrame,
      insertCloseTag;
    DECLARE_EVENT_TABLE()
};

#endif
