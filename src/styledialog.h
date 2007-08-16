#ifndef STYLEDIALOG_H
#define STYLEDIALOG_H

#define XHTML_START "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"><html xmlns=\"http://www.w3.org/1999/xhtml\"><head><title>XML Copy Editor report</title><meta http-equiv=\"Content-Type\" content=\"text/html\" charset=\"UTF-8\"/><style type=\"text/css\"><!-- body, h1, h2, h3, h4, th, td { font-family: \"Arial\", \"Helvetica\", sans-serif; } table { border-collapse: collapse; } th { text-align: left; font-size: smaller; padding-left: 5px; padding-right: 5px; } td { border-width: 1px; border-style: solid; border-color: gray; font-size: smaller; padding-left: 5px; padding-right: 5px; } --></style></head><body>"
#define XHTML_END "</body></html>"

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/filename.h>
#include <iostream>
#include <fstream>
#include "housestyle.h"
#include "contexthandler.h"
#include "housestylewriter.h"

enum
{
  ID_STYLE_TABLE = wxID_HIGHEST + 100,
  ID_STYLE_COMBO_RULESET,
  ID_STYLE_COMBO_FILTER,
  ID_STYLE_REPORT,
  ID_MENU_IGNORE_ONCE,
  ID_MENU_IGNORE_ALL,
  ID_MENU_CHANGE_ONCE,
  ID_MENU_CHANGE_ALL,
  ID_MENU_NEW_SUGGESTION,
  ID_MENU_APPLY_SUGGESTION_ALL,
  // disabled unless table contains items
  ID_STYLE_EDIT,
  ID_STYLE_WEB_REPORT,
  ID_STYLE_WEB_SUMMARY,
  ID_STYLE_IGNORE_ALL,
  ID_STYLE_CHANGE_ALL,
};

struct SortData
{
  int column;
  wxListCtrl *table;
};

class StyleDialog : public wxDialog
{
  public:
    StyleDialog(
      wxWindow *parent,
      wxIcon icon,
      const std::string& bufferParameterUtf8,
      const wxString& fileNameParameter,
      const wxString& ruleSetDirectoryParameter,
      const wxString& filterDirectoryParameter,
      const wxString& browserParameter,
      const wxString& ruleSetPresetParameter,
      const wxString& filterPresetParameter,
      bool readOnlyParameter = false,
      wxPoint position = wxDefaultPosition,
      wxSize size = wxSize(720, 540));
    ~StyleDialog();

    void OnColumnClick(wxListEvent& event);
    void OnItemRightClick(wxListEvent& event);
    void OnItemActivated(wxListEvent& event);
    void OnReport(wxCommandEvent& event);
    void OnStyleEdit(wxCommandEvent& event);
    void OnStyleWebReport(wxCommandEvent& event);
    void OnStyleWebSummary(wxCommandEvent& event);
    void OnStyleChangeAll(wxCommandEvent& event);
    void OnStyleIgnoreAll(wxCommandEvent& event);
    void OnMenuChangeOnce(wxCommandEvent& event);
    void OnMenuChangeAll(wxCommandEvent& event);
    void OnMenuIgnoreOnce(wxCommandEvent& event);
    void OnMenuIgnoreAll(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnMenuNewSuggestion(wxCommandEvent& event);
    void OnMenuApplySuggestionAll(wxCommandEvent& event);
    void OnUpdateTableRange(wxUpdateUIEvent& event);
    void styleSetIgnoreAll(bool ignore);
    std::string getEditedString();
    wxString getRuleSetPreset() { return ruleSetPreset; }
    wxString getFilterPreset() { return filterPreset; }
    wxPoint getPosition();
    wxSize getSize();
  private:
    int indexForContextMenu;
    wxPoint framePosition;
    wxSize frameSize;
    wxComboBox *ruleSetCombo, *filterCombo;
    wxListCtrl *table;
    wxStatusBar *status;
    std::string bufferUtf8;
    std::set<wxString> tempFiles;
    wxString fileName, ruleSetDirectory, filterDirectory, browser;
    wxString ruleSetPreset, filterPreset;
    vector<ContextMatch> matchVector;
    bool readOnly;
    bool isIgnore(int item);
    void setIgnore(int item, bool ignore);
    std::string flatWhiteSpace(std::string& s);
    static bool elementAndOffsetCompareFunction(
      ContextMatch m1,
      ContextMatch m2);
    static bool reportCompareFunction(ContextMatch m1, ContextMatch m2);

    static int wxCALLBACK MyCompareFunction(
      long item1,
      long item2,
      long sortData);
    void getAllMatches(vector<ContextMatch> &v);
    void getSelectedMatches(vector<ContextMatch> &v);
    void updateSizeInformation();
    static wxString getTextByColumn(wxListCtrl *table, long index, int col);  
    DECLARE_EVENT_TABLE()
};


#endif
