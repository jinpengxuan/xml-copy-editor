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

/////////////////////////////////////////////////////////////////////////////
// Name:		wxCoolBar.h 0.9.8
// Purpose:	header of wxCoolBar an related classes for wxMSW
// Author:      	Martin Simon
// Modified by:
// Copyright:	(c) Martin Simon - martin.simon@planet-berlin-it.de
// Licence:	 wxWidgets licence
// Modified Gerald Schmidt 29 July 2006
/////////////////////////////////////////////////////////////////////////////

#ifndef _WXCOOLBAR_H_
#define _WXCOOLBAR_H_


#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxCoolBar.cpp"
#endif

#include <wx/dynarray.h>

#ifdef __WXMSW__
class wxCoolBarMSW;
#endif

class wxCoolBar : public wxPanel
{
    DECLARE_CLASS ( wxCoolBar )
    DECLARE_EVENT_TABLE()
public:
    wxCoolBar();
    ~wxCoolBar();
    wxCoolBar ( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr );
    void AddBand ( wxWindow* win, bool newLine = false, wxString label = wxEmptyString, bool gripperAlways = false );
    void ShowBand ( unsigned index, bool show );
protected:
    bool Create ( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr );
    void OnSizeChanged ( wxSizeEvent& event );
    //void OnEraseBG(wxEraseEvent& event);
    //void OnEraseBandBG(wxEraseEvent & event);
    wxBoxSizer* m_sizerv;
#ifdef __WXMSW__
    wxCoolBarMSW* m_coolbarmsw;
#endif
};

#ifdef __WXMSW__
class wxCoolBarMSW : public wxControl
{
    DECLARE_CLASS ( wxCoolBarMSW )
    DECLARE_EVENT_TABLE()
public:
    wxCoolBarMSW();
    ~wxCoolBarMSW();
    wxCoolBarMSW ( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr );
    void AddBand ( wxWindow* win, bool newLine = false, wxString label = wxEmptyString, bool gripperAlways = false );
//protected:
    bool Create ( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr );
    wxSize DoGetBestSize() const;
    void ShowBand ( unsigned index, bool show );
    void OnSizeChanged ( wxSizeEvent& event );
    void OnEraseBG ( wxEraseEvent& event );
    void OnEraseBandBG ( wxEraseEvent & event );
};

struct menuentry
{
    int startx, endx, accel;
};

WX_DECLARE_OBJARRAY ( menuentry, menulist );
WX_DECLARE_OBJARRAY ( wxAcceleratorEntry, AccelArray );

class wxCoolMenuBarMSW : public wxControl
{
    DECLARE_CLASS ( wxCoolMenuBarMSW )
    DECLARE_EVENT_TABLE ()
public:
    wxCoolMenuBarMSW();
    ~wxCoolMenuBarMSW();
    wxCoolMenuBarMSW ( wxWindow* parent, wxWindowID id, wxMenuBar* menubar );
    void ShowBand ( unsigned index, bool show );
protected:
    menulist m_list;
    bool Create ( wxWindow* parent, wxWindowID id );
    void OnMouseDown ( wxMouseEvent &event );
    void OnMouseMove ( wxMouseEvent &event );
    void OnMouseLeave ( wxMouseEvent &event );
    void OpenMenu();
    void OnPaint ( wxPaintEvent& WXUNUSED ( event ) );
    int HitTest ( int x, int y );
    bool IsInSubMenu();
    bool IsSubMenu();
    wxMenuBar* m_menubar;
    static LRESULT CALLBACK MenuInputFilter ( int nCode, WPARAM wParam, LPARAM lParam );
    bool OnMenuInput ( MSG* pMsg );
    wxTopLevelWindow* GetParentFrame();
    HWND GetParentFrameHWND();
    int nextMenu, popedMenu, spotted, counter;
    HMENU selectedMenuHandle;
    bool SelectedIsSubMenu;
    wxPoint m_lastpt;
    static LRESULT CALLBACK FrameInputFilter ( int nCode, WPARAM wParam, LPARAM lParam );
    void OnFrameActivate ( wxActivateEvent& event );
    int GetAccelPos ( wxString& string );
    int GetAccelKey ( wxString& string );
    void UpdateFrameAccels();
    wxColour fontcolour;
};

#endif
//__WXMSW__

#ifndef __WXMSW__
class wxCoolBand : public wxPanel
{
    DECLARE_CLASS ( wxCoolBand )
    DECLARE_EVENT_TABLE()
public:
    wxCoolBand();
    ~wxCoolBand();
    wxCoolBand ( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr );
protected:
    bool Create ( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr );
    wxBoxSizer* m_sizerv;
};

#endif

class wxCoolLabel : public wxControl
{
    DECLARE_CLASS ( wxCoolLabel )
    DECLARE_EVENT_TABLE()
public:
    wxCoolLabel();
    ~wxCoolLabel();
    wxCoolLabel ( wxWindow* parent, wxWindowID id = -1, const wxString& label = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0 );
    bool Create ( wxWindow* parent, wxWindowID id = -1, const wxString& label = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0 );
    void SetLabel ( const wxString& label );
    bool SetFont ( const wxFont& font );
private:
    void OnPaint ( wxPaintEvent& event );
    wxSize DoGetBestSize() const;
};


#endif
// _WXCOOLBAR_H_
