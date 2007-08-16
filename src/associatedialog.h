#ifndef ASSOCIATE_DIALOG_H
#define ASSOCIATE_DIALOG_H

#include <wx/wx.h>
#include <wx/tipwin.h>

class AssociateDialog : public wxDialog
{
  public:
    AssociateDialog(
      wxWindow *parent,
      const wxString& titleParameter = wxEmptyString,
      const wxString& labelParameter = wxEmptyString,
      const wxString& typeParameter = _T("All"),
      const wxString& extensionParameter = _T("*.*"),
      const wxString& urlParameter = wxEmptyString,
      bool auxNeededParameter = false,
      const wxString& auxLabelTextParameter = wxEmptyString,
      const wxString& auxParameter = wxEmptyString);
    ~AssociateDialog();
    void OnOk(wxCommandEvent& e);
    void OnBrowse(wxCommandEvent& e);
    void OnContextHelp(wxHelpEvent& e);
    void OnUpdateOk(wxUpdateUIEvent& event);
    wxString getUrl();
    wxString getAux();
    enum constants {
      ID_URL,
      ID_BROWSE,
      ID_AUX
    };
  private:
    wxTextCtrl *urlCtrl, *auxCtrl;
    wxButton *browseButton;
    wxStaticText *urlLabel, *auxLabel;
    wxString title, label, type, extension, url;
    bool auxNeeded;
    wxString auxLabelText, aux;
    DECLARE_EVENT_TABLE()
};

#endif
