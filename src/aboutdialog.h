#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <wx/wx.h>
#include <wx/wxhtml.h>

class AboutDialog : public wxDialog
{
  public:
    AboutDialog(
      wxWindow *parent,
      const wxString& title,
      const wxString& path,
      const wxString& hyperlink,
      wxPoint positionParameter);
    ~AboutDialog();
    void OnOk(wxCommandEvent& e);
    wxPoint getPosition();
  private:
    wxPoint position;
    wxHtmlWindow *html;
    wxButton *button;
    DECLARE_EVENT_TABLE()
};

#endif
