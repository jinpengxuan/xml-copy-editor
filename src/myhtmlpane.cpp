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

#include "myhtmlpane.h"
#include "xmlcopyeditor.h"

BEGIN_EVENT_TABLE ( MyHtmlPane, wxHtmlWindow )
	EVT_LEFT_DCLICK ( MyHtmlPane::OnLeftDoubleClick )
END_EVENT_TABLE()

MyHtmlPane::MyHtmlPane (
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& position,
    const wxSize& size ) : wxHtmlWindow ( parent, id, position, size )
{}

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

void MyHtmlPane::OnLeftDoubleClick ( wxMouseEvent& WXUNUSED ( event ) )
{ }
