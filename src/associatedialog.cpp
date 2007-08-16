#include <memory>
#include "associatedialog.h"

BEGIN_EVENT_TABLE(AssociateDialog, wxDialog)
  EVT_BUTTON(wxID_OK, AssociateDialog::OnOk)
  EVT_BUTTON(ID_BROWSE, AssociateDialog::OnBrowse)
  EVT_HELP_RANGE(ID_URL, ID_AUX, AssociateDialog::OnContextHelp)
  EVT_HELP(wxID_OK, AssociateDialog::OnContextHelp)
  EVT_HELP(wxID_CANCEL, AssociateDialog::OnContextHelp)
  EVT_UPDATE_UI(wxID_OK, AssociateDialog::OnUpdateOk)
END_EVENT_TABLE()

AssociateDialog::AssociateDialog(
      wxWindow *parent,
      const wxString& titleParameter,
      const wxString& labelParameter,
      const wxString& typeParameter,
      const wxString& extensionParameter,
      const wxString& urlParameter,
      bool auxNeededParameter,
      const wxString& auxLabelTextParameter,
      const wxString& auxParameter) :
        wxDialog(),
        title(titleParameter),
        label(labelParameter),
        type(typeParameter),
        extension(extensionParameter),
        url(urlParameter),
        auxNeeded(auxNeededParameter),
        auxLabelText(auxLabelTextParameter),
        aux(auxParameter)
{
  SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
  Create(
    parent,
    wxID_ANY,
    title,
    wxDefaultPosition,
    wxSize(-1, -1),
    wxDEFAULT_DIALOG_STYLE);

  wxBoxSizer *dialogSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  auxCtrl = NULL; // may or may not be used
  urlLabel = new wxStaticText(this, ID_URL, label);
  urlCtrl = new wxTextCtrl(
    this,
    ID_URL,
    url,
    wxDefaultPosition,
    wxSize(240, -1));
  browseButton = new wxButton(
    this,
    ID_BROWSE,
    _("Browse"));
  horizontalSizer->Add(urlCtrl, 0, wxLEFT, 0);
  horizontalSizer->Add(browseButton, 0, wxLEFT, 5);

  if (auxNeeded)
  {
    auxLabel = new wxStaticText(this, ID_AUX, auxLabelText);
    auxCtrl = new wxTextCtrl(
      this,
      ID_AUX,
      aux,
      wxDefaultPosition,
      wxSize(240, -1));
    dialogSizer->Add(auxLabel, 0, wxLEFT | wxTOP | wxALIGN_LEFT, 10);
    dialogSizer->Add(auxCtrl, 0, wxALL, 5);
  }

  dialogSizer->Add(urlLabel, 0, wxTOP | wxLEFT | wxALIGN_LEFT, 10);
  dialogSizer->Add(horizontalSizer, 0, wxALL, 5);
  dialogSizer->Add(
    CreateButtonSizer(wxOK | wxCANCEL), 0, wxTOP | wxBOTTOM | wxALIGN_RIGHT, 10);
  this->SetSizer(dialogSizer);
  dialogSizer->SetSizeHints(this);

  if (auxNeeded)
    auxCtrl->SetFocus();
  else
    urlCtrl->SetFocus();
}

AssociateDialog::~AssociateDialog()
{ }

void AssociateDialog::OnOk(wxCommandEvent& e)
{
  url = urlCtrl->GetValue();
  if (auxCtrl)
    aux = auxCtrl->GetValue();
  e.Skip();
}

void AssociateDialog::OnContextHelp(wxHelpEvent& e)
{
  wxTipWindow *tw;
  int id = e.GetId();
  if (id == ID_URL)
    tw = new wxTipWindow(
      this,
      _("Provides a space for you to type the path of the file"));
  else if (id == ID_BROWSE)
    tw = new wxTipWindow(
      this,
      _("Opens a standard file dialog"));
  else if (id == ID_AUX)
    tw = new wxTipWindow(
      this,
      _("Provides a space for you to type additional information"));
  else if (id == wxID_CANCEL)
    tw = new wxTipWindow(
      this,
      _("Closes this dialog without making any changes"));
  else if (id == wxID_OK)
    tw = new wxTipWindow(
      this,
      _("Selects the file specified"));
  else { }
  e.Skip();
}

void AssociateDialog::OnUpdateOk(wxUpdateUIEvent& e)
{
  e.Enable(!urlCtrl->GetValue().empty());
}

wxString AssociateDialog::getUrl() { return url; }
wxString AssociateDialog::getAux() { return aux; }

void AssociateDialog::OnBrowse(wxCommandEvent& e)
{
  wxString extensionArgument;
  extensionArgument =
    type +
    _T(" (") +
    extension +
    _T(")|") +
    extension +
    _("|All files (*.*)|*.*");
  std::auto_ptr<wxFileDialog> fd(new wxFileDialog(
    this,
    _("Select ") + type,
    _T(""),
    _T(""),
    extensionArgument,
    wxOPEN | wxFILE_MUST_EXIST | wxCHANGE_DIR
       ));

  if (fd->ShowModal() == wxID_OK)
  {
    wxString newValue = fd->GetPath();
    newValue.Replace(_T("\\"), _T("/"), true);
    newValue.Replace(_T(" "), _T("%20"), true);
    urlCtrl->SetValue(newValue);
  }
}
