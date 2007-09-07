#ifndef MY_HTML_PANE_H
#define MY_HTML_PANE_H

#include <wx/wx.h>
#include <wx/wxhtml.h>

class MyHtmlPane : public wxHtmlWindow
{
  public:
    MyHtmlPane(
      wxWindow *parent,
      wxWindowID id = wxID_ANY,
      const wxPoint& position = wxDefaultPosition,
      const wxSize& size = wxDefaultSize);
  private:
    /*
    void OnCellClicked(
      wxHtmlCell *cell,
      wxCoord x,
      wxCoord y,
      const wxMouseEvent& event);
    */
    void OnLeftDoubleClick(wxMouseEvent& event);
    DECLARE_EVENT_TABLE()
};

#endif
