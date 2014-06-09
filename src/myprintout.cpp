/*
 * Copyright 2014 Zane U. Ji.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/log.h>
#endif
#include "myprintout.h"
#include "xmldoc.h"

MyPrintout::MyPrintout ( XmlDoc *doc, const wxString &title )
	: wxPrintout ( title )
	, mDoc ( doc )
	, mMinPage ( 1 )
	, mMaxPage ( 0 )
{
	wxASSERT ( doc );
}

MyPrintout::~MyPrintout()
{
}

void MyPrintout::OnPreparePrinting()
{
	if ( !mDoc )
		return;

	mDoc->SetPrintWrapMode ( wxSTC_WRAP_WORD );
	mDoc->SetPrintMagnification ( mDoc->GetZoom() );

	SetupDC();

	//Lets make a guess as to how many pages there are based on the default printer.
	mMaxPage = CalcMaxPage();
	mMinPage = 1;  // start printing at page# 1

	// default preparation
	wxPrintout::OnPreparePrinting();
}

void MyPrintout::OnBeginPrinting()
{
	wxPrintout::OnBeginPrinting();

	SetupDC();
}

void MyPrintout::OnEndPrinting()
{
	wxPrintout::OnEndPrinting();
}

bool MyPrintout::HasPage ( int nPage )
{
	return ( nPage >= mMinPage && nPage <= mMaxPage );
}

void MyPrintout::GetPageInfo
	( int *minPage
	, int *maxPage
	, int *pageFrom
	, int *pageTo
	)
{
	*minPage = mMinPage;
	*maxPage = mMaxPage;
	*pageFrom = mMinPage;
	*pageTo = mMaxPage;
}

bool MyPrintout::OnPrintPage ( int page )
{
	wxDC *dc = GetDC();
	if ( !( dc && mDoc ) )
		return false;

	int header, footer;
	dc->SetLogicalOrigin ( -mPrintRect.x, -mPrintRect.y );
	header = PrintHeader();	//print the header
	dc->SetLogicalOrigin ( -mPrintRect.x, -mPrintRect.height - mPrintRect.y );
	footer = PrintFooter();	//Print the footer
	mPrintRect.y += header;
	mPrintRect.height -= header + footer;
	dc->SetLogicalOrigin ( 0, 0 );

	mDoc->FormatRange ( true, mStartPositions [ page - 1 ], mDoc->GetLength(),
					dc, dc, mPrintRect, mPageRect );

	return true;
}

void MyPrintout::SetupDC()
{
	wxDC *dc = GetDC();
	wxASSERT ( dc && mDoc );

	// From wxWidgets
	// get printer and screen sizing values
	wxSize ppiScr;
	GetPPIScreen ( &ppiScr.x, &ppiScr.y );
	if ( ppiScr.x == 0 ) { // most possible guess 96 dpi
		ppiScr.x = 96;
		ppiScr.y = 96;
	}
	wxSize ppiPrt;
	GetPPIPrinter ( &ppiPrt.x, &ppiPrt.y );
	if ( ppiPrt.x == 0 ) { // scaling factor to 1
		ppiPrt.x = ppiScr.x;
		ppiPrt.y = ppiScr.y;
	}
	wxSize dcSize = dc->GetSize();
	wxSize pageSize;
	GetPageSizePixels (&pageSize.x, &pageSize.y);

	// set user scale
	double scaleX = double ( ppiPrt.x ) * dcSize.x /
					( double ( ppiScr.x ) * pageSize.x );
	double scaleY = double ( ppiPrt.y ) * dcSize.y /
					( double ( ppiScr.y ) * pageSize.y );
	dc->SetUserScale ( scaleX, scaleY );

	mPageRect = GetLogicalPageRect();
	mPrintRect = GetLogicalPageMarginsRect ( GetPageData() );
}

int MyPrintout::CalcMaxPage()
{
	mStartPositions.clear();

	wxDC *dc = GetDC();
	if ( !( dc && mDoc ) )
		return 0;

	if ( mPrintRect.IsEmpty() )
		return 0;

	int start = 0;
	int end = mDoc->GetLength();
	while ( start < end )
	{
		mStartPositions.push_back ( start );
		start = mDoc->FormatRange ( false, start, end, dc, dc
						, mPrintRect, mPageRect );
	}

	return mStartPositions.size();
}

// Returns the height of the header
int MyPrintout::PrintHeader()
{
	return 0;
}

// Returns the height of the footer
int MyPrintout::PrintFooter()
{
	return 0;
}

wxPageSetupDialogData &MyPrintout::GetPageData()
{
	static wxPageSetupDialogData data;
	static bool bInitialized = false;

	if ( !bInitialized )
	{
		// Set page margins that are in mm. 25.4mm = 1"
		data.SetMarginTopLeft ( wxPoint ( 15, 15 ) );
		data.SetMarginBottomRight ( wxPoint ( 15, 15 ) );
		data.SetPaperId ( wxPAPER_A4 );

		bInitialized = true;
	}
	return data;
}
