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

#ifndef MYPRINTOUT_H_
#define MYPRINTOUT_H_

#include <wx/prntbase.h>
#include <vector>

class XmlDoc;

class MyPrintout: public wxPrintout
{
public:
	MyPrintout ( XmlDoc *doc, const wxString &title );
	virtual ~MyPrintout();

	virtual void OnPreparePrinting();

	virtual void OnBeginPrinting();
	virtual void OnEndPrinting();

	virtual bool HasPage ( int nPage );
	virtual bool OnPrintPage ( int nPage );
	virtual void GetPageInfo ( int *minPage, int *maxPage, int *pageFrom
			, int *pageTo );

	static wxPageSetupDialogData &GetPageData();

protected:
	void SetupDC();

	int CalcMaxPage();

	// Returns the height of the header
	int PrintHeader();
	// Returns the height of the footer
	int PrintFooter();

protected:
	XmlDoc *mDoc;
	int mMinPage, mMaxPage; // 1 based
	std::vector<int> mStartPositions; // Start positions of each page
	wxRect mPrintRect, mPageRect;
};

#endif /* MYPRINTOUT_H_ */
