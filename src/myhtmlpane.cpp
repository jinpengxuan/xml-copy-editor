#include "myhtmlpane.h"
#include "xmlcopyeditor.h"

BEGIN_EVENT_TABLE(MyHtmlPane, wxHtmlWindow)
  EVT_LEFT_DCLICK(MyHtmlPane::OnLeftDoubleClick)
END_EVENT_TABLE()

MyHtmlPane::MyHtmlPane(
  wxWindow *parent,
  wxWindowID id,
  const wxPoint& position,
  const wxSize& size) : wxHtmlWindow(parent, id, position, size)
{
}

/*
void MyHtmlPane::OnCellClicked(
  wxHtmlCell *cell,
  wxCoord x,
  wxCoord y,
  const wxMouseEvent& event)
{
  int width = GetSize().GetWidth();
  if (x < (width*9/10))
    return;

  MyFrame *frame = (MyFrame *)GetParent();
  if (frame)
  {
    wxCommandEvent e;
    frame->OnClosePane(e);
  }
}
*/

void MyHtmlPane::OnLeftDoubleClick(wxMouseEvent& WXUNUSED(event)) { }
