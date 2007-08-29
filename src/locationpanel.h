#ifndef LOCATION_PANEL_H
#define LOCATION_PANEL_H

#include <wx/wx.h>
#include <wx/grid.h>
#include "xmlcopyeditor.h"

class XmlDoc;

class LocationPanel : public wxPanel
{
  public:
    LocationPanel(wxWindow *parent, int id);
    void update(
      XmlDoc *docParameter = NULL,
      const wxString& parent = wxEmptyString);
  private:
    MyFrame *parentWindow;
    XmlDoc *doc;
    wxBoxSizer *sizer;
    wxTextCtrl *edit;
    //wxListBox *list;
    wxString parent;

    DECLARE_EVENT_TABLE()
};

#endif
