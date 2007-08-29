#ifndef FINDREPLACEPANEL_H
#define FINDREPLACEPANEL_H

#include <wx/wx.h>
#include <iostream>
#include <fstream>

class wxFindReplaceData;

enum
{
  ID_FINDREPLACE_FIND_NEXT,
  ID_FINDREPLACE_REPLACE,
  ID_FINDREPLACE_REPLACE_ALL,
  ID_FINDREPLACE_MATCH_CASE,
  ID_FINDREPLACE_REGEX
};

class FindReplacePanel : public wxPanel
{
  public:
    FindReplacePanel(
      wxWindow *parent,
      int id,
      wxFindReplaceData *findDataParameter,
      bool isReplacePanel = true,
      bool isRegex = true);
    ~FindReplacePanel();
    void OnFindNext(wxCommandEvent& event);
    void OnReplace(wxCommandEvent& event);
    void OnReplaceAll(wxCommandEvent& event);
    void focusOnFind();
    bool getIncrementalFind();
    bool getRegex();
    void refresh();
    void setReplaceVisible(bool b);
    void setMatchCase(bool b);
    void setRegex(bool b);
    void flagNotFound(bool b);
    void enableButtons(bool b);
  private:
    wxTextCtrl *findEdit, *replaceEdit;
    wxStaticText *label1, *label2, *spacer1, *spacer2;
    wxButton *findNextButton, *replaceButton, *replaceAllButton;
    wxCheckBox *matchCaseBox, *regexBox;
    wxFindReplaceData *findData;
    wxBoxSizer *sizer;
    wxWindow *parent;
    size_t findEditLength;
    bool matchCaseMemory, regexMemory;
    bool incrementalFind, isReplaceDialog, notFoundSet, isRegex;

    void OnIdle(wxIdleEvent& event);
    void sendFindEvent(size_t flags);

    DECLARE_EVENT_TABLE()
};

#endif
