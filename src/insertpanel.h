#ifndef INSERT_PANEL_H
#define INSERT_PANEL_H

#include <wx/wx.h>
#include "xmldoc.h"
#include "xmlcopyeditor.h"

enum {
  INSERT_PANEL_TYPE_CHILD = 0,
  INSERT_PANEL_TYPE_SIBLING,
  INSERT_PANEL_TYPE_ENTITY
};

class InsertPanel : public wxPanel
{
  public:
    InsertPanel(
      wxWindow *parent,
      int id,
      int type = INSERT_PANEL_TYPE_CHILD);
    void update(
      XmlDoc *doc,
      const wxString& parent = wxEmptyString,
      const wxString& grandparent = wxEmptyString);
    void OnEnter(wxCommandEvent& event);
    void OnDoubleClick(wxCommandEvent& event);
    void OnListSelection(wxCommandEvent& event);
    void setEditFocus();
    void OnSize(wxSizeEvent& e);
    void adjustSize();
  private:
    MyFrame *parentWindow;
    int type;
    wxBoxSizer *sizer;
    wxTextCtrl *edit;
    wxListBox *list;
    wxString parent, grandparent, lastParent;
    XmlDoc *doc, *lastDoc;
    
    void handleChoice(const wxString& choice);
    DECLARE_EVENT_TABLE()
};

#endif
