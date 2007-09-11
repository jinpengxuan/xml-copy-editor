/* 
 * Copyright 2005-2007 Gerald Schmidt.
 * 
 * This file is part of Xml Copy Editor.
 * 
 * Xml Copy Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * Xml Copy Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Xml Copy Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
