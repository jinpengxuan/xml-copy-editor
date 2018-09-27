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

#ifndef XMLASSOCIATEINFO_H_
#define XMLASSOCIATEINFO_H_

#include "wrapexpat.h"

class XmlTextInfo : public WrapExpat
{
public:
	XmlTextInfo
		( const wxString &path
		, const char *encoding = NULL
		);
	virtual ~XmlTextInfo();

private:
	static void XMLCALL startdoctypehandler
		( void *userData
		, const XML_Char *doctypeName
		, const XML_Char *sysid
		, const XML_Char *pubid
		, int has_internal_subset
		);
	static void XMLCALL processinghandler
		( void *data
		, const XML_Char *target
		, const XML_Char *datastring
		);
	static void XMLCALL start
		( void *data
		, const XML_Char *el
		, const XML_Char **attr
		);
	static void XMLCALL defaulthandler
		( void *data
		, const XML_Char *s
		, int len
		);

public:
	wxString mDocTypeName;
	wxString mDtdFile, mXsdFile, mXslFile;
	wxString mEOL;
};

#endif /* XMLASSOCIATEINFO_H_ */
