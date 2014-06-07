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

#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlassociatedtd.h"

XmlAssociateDtd::XmlAssociateDtd (
	const XmlTextInfo &info,
	const wxString &path,
	const wxString &publicID,
	const char *encoding,
	size_t size )
	: WrapExpat ( encoding )
	, d ( new DtdData() )
	, mInfo ( info )
{
	d->buffer.reserve ( size );
	d->path = path;
	d->publicID = publicID;
	d->associated = false;
	d->insideDtd = false;
	XML_SetUserData ( p, this );
	XML_SetElementHandler ( p, start, NULL );
	XML_SetDoctypeDeclHandler ( p, startdoctypehandler, enddoctypehandler );
	XML_SetDefaultHandlerExpand ( p, defaulthandler );
}

XmlAssociateDtd::~XmlAssociateDtd()
{}

void XMLCALL XmlAssociateDtd::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
	XmlAssociateDtd *pThis = (XmlAssociateDtd *)data;
	DtdData *d = pThis->d.get();
	if ( !d->insideDtd )
		d->buffer.append ( s, len );
}

void XMLCALL XmlAssociateDtd::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
	XmlAssociateDtd *pThis = (XmlAssociateDtd *)data;
	DtdData *d = pThis->d.get();

	if ( !d->associated )
	{
		associate ( d, el );
		d->buffer += pThis->mInfo.mEOL.utf8_str();
		d->associated = true;
	}

	XML_DefaultCurrent ( pThis->p );
	XML_SetElementHandler ( pThis->p, NULL, NULL );
}

void XMLCALL XmlAssociateDtd::end ( void *data, const XML_Char *el )
{
	XmlAssociateDtd *pThis = (XmlAssociateDtd *)data;
	DtdData *d = pThis->d.get();
	d->buffer += "</";
	d->buffer += el;
	d->buffer += ">";
}

void XMLCALL XmlAssociateDtd::startdoctypehandler (
    void *data,
    const XML_Char *doctypeName,
    const XML_Char *sysid,
    const XML_Char *pubid,
    int has_internal_subset )
{
	XmlAssociateDtd *pThis = (XmlAssociateDtd *)data;
	DtdData *d = pThis->d.get();
	d->insideDtd = true;

	associate ( d, doctypeName );
	d->associated = true;
}

void XMLCALL XmlAssociateDtd::enddoctypehandler ( void *data )
{
	XmlAssociateDtd *pThis = (XmlAssociateDtd *)data;
	DtdData *d = pThis->d.get();
	d->insideDtd = false;
}

void XmlAssociateDtd::associate ( DtdData *d, const char *doctypeName )
{
	wxASSERT ( d );

	d->buffer += "<!DOCTYPE ";
	d->buffer += doctypeName;
	if ( d->publicID.empty() )
	{
		d->buffer += " SYSTEM \"";
		d->buffer += d->path.utf8_str(); // TODO: Apply the encoding of the parser
	}
	else
	{
		d->buffer += " PUBLIC \"";
		d->buffer += d->publicID.utf8_str();
		d->buffer += "\" \"";
		d->buffer += d->path.utf8_str();// TODO: Apply the encoding of the parser
	}
	d->buffer += "\">";
}
