#ifndef COMMANDPANEL_H
#define COMMANDPANEL_H

#include <wx/wx.h>
#include <iostream>
#include <fstream>

enum
{
  ID_RUN,
  ID_SYNC,
  ID_COMMAND_OUTPUT_IGNORE,
  ID_COMMAND_OUTPUT_INSERT,
  ID_COMMAND_OUTPUT_NEW_DOCUMENT,
  ID_BUTTON_PATH,
  ID_BUTTON_NAME,
  ID_BUTTON_EXTENSION,
  ID_BUTTON_FULLPATH
};

class MyFrame;

class CommandPanel : public wxPanel
{
  public:
    CommandPanel(
      wxWindow *parent,
      int id,
      const wxString& cmd,
      bool sync = false,
      int output = ID_COMMAND_OUTPUT_IGNORE,
      const wxString& command = wxEmptyString);
    ~CommandPanel();
    void OnRun(wxCommandEvent& event);
    void OnVariableButton(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void focusOnCommand();
    bool getSync();
    int getOutput();
    wxString getCommand();
  private:
    wxString path, name, extension, fullpath;
    wxTextCtrl *commandEdit;
    //wxStaticText *label;
    wxButton *runButton;
    wxCheckBox *syncBox;
    wxRadioButton *outputIgnore, *outputInsert, *outputNewDocument;
    wxBoxSizer *topSizer, *bottomSizer, *mainSizer;
    MyFrame *parent;
    DECLARE_EVENT_TABLE()
};

#endif
