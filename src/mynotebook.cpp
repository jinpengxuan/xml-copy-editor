#include "mynotebook.h"
#include "xmlcopyeditor.h"
#include "xmldoc.h"

BEGIN_EVENT_TABLE ( MyNotebook, wxAuiNotebook )
EVT_LEFT_DOWN ( MyNotebook::OnLeftDown )
//EVT_MIDDLE_DOWN(MyNotebook::OnMiddleDown)
EVT_RIGHT_DOWN ( MyNotebook::OnRightDown )
EVT_MENU ( ID_MENU_CLOSE, MyNotebook::OnMenuClose )
EVT_MENU ( ID_MENU_CLOSE_ALL, MyNotebook::OnMenuCloseAll )
END_EVENT_TABLE()

MyNotebook::MyNotebook (
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& position,
    const wxSize& size,
    int style ) : wxAuiNotebook ( parent, id, position, size, style )
{
    rightClickPage = -1;
}

void MyNotebook::OnLeftDown ( wxMouseEvent& event )
{
    int page = HitTest ( wxPoint ( event.GetX(), event.GetY() ) );
    if ( page == -1 )
    {
        event.Skip();
        return;
    }
    XmlDoc *doc = ( XmlDoc * ) GetPage ( page );
    if ( !doc )
    {
        event.Skip();
        return;
    }
    SetSelection ( page );
    doc->SetFocus();
}

void MyNotebook::OnMiddleDown ( wxMouseEvent& event )
{
    int page = HitTest ( wxPoint ( event.GetX(), event.GetY() ) );
    if ( page == -1 )
    {
        event.Skip();
        return;
    }
    SetSelection ( page );
    MyFrame *frame = ( MyFrame * ) GetParent();
    if ( frame )
        frame->closeActiveDocument();
}

void MyNotebook::OnRightDown ( wxMouseEvent& event )
{
    rightClickPage = HitTest ( wxPoint ( event.GetX(), event.GetY() ) );
    if ( rightClickPage == -1 )
    {
        event.Skip();
        return;
    }
    SetSelection ( rightClickPage );
    wxMenu contextMenu;
    contextMenu.Append ( ID_MENU_CLOSE, _ ( "Close" ) );
    contextMenu.Append ( ID_MENU_CLOSE_ALL, _ ( "Close all" ) );
    contextMenu.Enable ( ID_MENU_CLOSE_ALL, ( this->GetPageCount() > 1 ) );
    PopupMenu ( &contextMenu, wxPoint ( -1, -1 ) );
}

void MyNotebook::OnMenuClose ( wxCommandEvent& WXUNUSED ( event ) )
{
    if ( rightClickPage == -1 )
        return;
    MyFrame *frame = ( MyFrame * ) GetParent();
    if ( frame )
        frame->closeActiveDocument();
}

void MyNotebook::OnMenuCloseAll ( wxCommandEvent& WXUNUSED ( event ) )
{
    MyFrame *frame = ( MyFrame * ) GetParent();
    if ( !frame )
        return;
    wxCommandEvent e;
    frame->OnCloseAll ( e );
}
