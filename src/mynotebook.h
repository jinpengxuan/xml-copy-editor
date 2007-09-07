#ifndef MY_NOTEBOOK_H
#define MY_NOTEBOOK_H

#include <wx/wx.h>
#include <wx/aui/auibook.h>

enum {
    ID_MENU_CLOSE,
    ID_MENU_CLOSE_ALL
};

class MyNotebook : public wxAuiNotebook
{
public:
    MyNotebook (
        wxWindow *parent,
        wxWindowID id,
        const wxPoint& position,
        const wxSize& size,
        int style );
    void OnLeftDown ( wxMouseEvent& event );
    void OnMiddleDown ( wxMouseEvent& event );
    void OnRightDown ( wxMouseEvent& event );
    void OnMenuClose ( wxCommandEvent& event );
    void OnMenuCloseAll ( wxCommandEvent& event );
private:
    int rightClickPage;
    DECLARE_EVENT_TABLE()
};

#endif
