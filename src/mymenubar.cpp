//mymenubar.cpp

#include "mymenubar.h"

BEGIN_EVENT_TABLE ( MyMenuBar, wxMenuBar )
END_EVENT_TABLE()

MyMenuBar::MyMenuBar ( long style ) : wxMenuBar ( style )
{
#ifdef __WXMSW__
    wxWindow *window = ( wxWindow * ) this;
    HWND handle = ( HWND ) window->GetHandle();
    if ( !handle )
        return;
#endif
}
