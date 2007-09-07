// mytoolbar.h

#ifndef MY_TOOLBAR
#define MY_TOOLBAR

#include <wx/wx.h>
#include <wx/dcbuffer.h>

class MyToolBar : public wxToolBar
{
public:
    MyToolBar (
        wxFrame *parent,
        int id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0 );
    void OnEraseBG ( wxEraseEvent& event );
private:
    wxColour m_colorFrom, m_colorTo, m_colorBorder;

    //void FillGradientColor(wxDC *dc, const wxRect& rect);
    void PaintStraightGradientBox (
        wxDC *dc,
        const wxRect& rect,
        const wxColour& startColor,
        const wxColour& endColor,
        bool vertical = true );
    DECLARE_EVENT_TABLE()
};

#endif
