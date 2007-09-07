// mymenubar.h

#ifndef MY_MENU_BAR
#define MY_MENU_BAR

#include <wx/wx.h>
#include <wx/dcbuffer.h>

class MyMenuBar : public wxMenuBar
{
  public:
    MyMenuBar(long style = 0);
  private:
    DECLARE_EVENT_TABLE()
};

#endif 
