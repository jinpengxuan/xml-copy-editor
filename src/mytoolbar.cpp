//mytoolbar.cpp

#include "mytoolbar.h"

BEGIN_EVENT_TABLE(MyToolBar, wxToolBar)
  EVT_ERASE_BACKGROUND(MyToolBar::OnEraseBG)
END_EVENT_TABLE()

// taken from wxStyledNotebook (c) Eran Ifrah <eranif@bezeqint.net>
static wxColor LightColour(const wxColour& color, int percent)
{
	int rd, gd, bd, high = 0;
	wxColor end_color = wxT("WHITE");
	rd = end_color.Red() - color.Red();
	gd = end_color.Green() - color.Green();
	bd = end_color.Blue() - color.Blue();
	high = 100;

	// We take the percent way of the color from color --> white
	int i = percent;
	int r = color.Red() +  ((i*rd*100)/high)/100;
	int g = color.Green() + ((i*gd*100)/high)/100;
	int b = color.Blue() + ((i*bd*100)/high)/100;
	return wxColor(r, g, b);
}

MyToolBar::MyToolBar(
  wxFrame *parent,
  int id,
  const wxPoint& pos,
  const wxSize& size,
  long style) :
  wxToolBar(parent, id, pos, size, style)
{
	/// Override colors 
	m_colorTo   = LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 0); 
	m_colorFrom = LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 80);//60);
  //m_colorFrom = LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), 0);
}

void MyToolBar::OnEraseBG(wxEraseEvent& event)
{
  wxDC* DC = event.GetDC();
  wxWindow *Window = dynamic_cast<wxWindow *>(event.GetEventObject());
  assert(Window);
  int x, y, w, h;
  x = y = 0;
  Window->GetSize(&w, &h);
  wxRect rect(x, y, w, h);
  PaintStraightGradientBox(DC, rect, m_colorFrom, m_colorTo);
}

// adapted from wxFlatNotebook (c) Eran Ifrah
void MyToolBar::PaintStraightGradientBox(wxDC *dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool vertical)
{
	int rd, gd, bd, high = 0;
	rd = endColor.Red() - startColor.Red();
	gd = endColor.Green() - startColor.Green();
	bd = endColor.Blue() - startColor.Blue();

	/// Save the current pen and brush
	wxPen savedPen = dc->GetPen();
	wxBrush savedBrush = dc->GetBrush();

	if ( vertical )
		high = rect.GetHeight()-1;
	else
		high = rect.GetWidth()-1;

	if( high < 1 )
		return;

	for (int i = 0; i <= high; ++i)
	{
		int r = startColor.Red() +  ((i*rd*100)/high)/100;
		int g = startColor.Green() + ((i*gd*100)/high)/100;
		int b = startColor.Blue() + ((i*bd*100)/high)/100;

		wxPen p(wxColor(r, g, b));
		dc->SetPen(p);

		if ( vertical )
			dc->DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
		else
			dc->DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
	}

	/// Restore the pen and brush
	dc->SetPen( savedPen );
	dc->SetBrush( savedBrush );
}


