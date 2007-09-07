#ifndef GLOBAL_REPLACE_DIALOG_H
#define GLOBAL_REPLACE_DIALOG_H

#include <wx/wx.h>
#include <wx/tipwin.h>

class GlobalReplaceDialog : public wxDialog
{
public:
    GlobalReplaceDialog (
        wxWindow *parent,
        const wxString& findParameter = _T ( "" ),
        const wxString& replaceParameter = _T ( "" ),
        bool matchCaseParameter = false,
        bool allDocumentsParameter = false,
        bool regexParameter = true );
    ~GlobalReplaceDialog();
    void OnOk ( wxCommandEvent& e );
    void OnContextHelp ( wxHelpEvent& e );
    void OnUpdateOk ( wxUpdateUIEvent& event );
    wxString getFindString();
    wxString getReplaceString();
    bool getRegex();
    bool getAllDocuments();
    bool getMatchCase();
    enum constants
    {
        ID_FIND,
        ID_REPLACE,
        ID_REGEX,
        ID_MATCHCASE,
        ID_ALLDOCUMENTS
    };

private:
    wxTextCtrl *findCtrl, *replaceCtrl;
    wxStaticText *findLabel, *replaceLabel;
    wxCheckBox *matchCaseBox, *allDocumentsBox, *regexBox;
    wxString find, replace;
    bool matchCase, allDocuments, regex;
    DECLARE_EVENT_TABLE()
};

#endif
