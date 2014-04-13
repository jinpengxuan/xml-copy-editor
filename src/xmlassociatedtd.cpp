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
    const wxString &path,
    const wxString &publicID,
    const char *encoding,
    size_t size )
    : WrapExpat ( encoding )
    , d ( new DtdData() )
{
	d->buffer.reserve ( size );
	d->path = path;
	d->publicID = publicID;
	d->rootElementSeen = false;
	d->insideDtd = false;
	XML_SetUserData ( p, d.get() );
	XML_SetElementHandler ( p, start, end );
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
	DtdData *d;
	d = ( DtdData * ) data;
	if ( !d->insideDtd )
		d->buffer.append ( s, len );
}

void XMLCALL XmlAssociateDtd::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
	DtdData *d;
	d = ( DtdData * ) data;

	if ( !d->rootElementSeen )
	{
		if ( d->publicID.empty() )
		{
			d->buffer += "<!DOCTYPE ";
			d->buffer += el;
			d->buffer += " SYSTEM \"";
			d->buffer += d->path.utf8_str(); // TODO: Apply the encoding of the parser
			d->buffer += "\">\n";
		}
		else
		{
			d->buffer += "<!DOCTYPE ";
			d->buffer += el;
			d->buffer += " PUBLIC \"";
			d->buffer += d->publicID.utf8_str(); // TODO: Apply the encoding of the parser
			d->buffer += "\" \"";
			d->buffer += d->path.utf8_str();
			d->buffer += "\">\n";
		}
		d->rootElementSeen = true;
	}

	d->buffer += "<";
	d->buffer += el;

	while ( *attr )
	{
		d->buffer += " ";
		d->buffer += *attr;
		d->buffer += "=\"";
		d->buffer += xmliseAttribute ( * ( attr + 1 ) );
		d->buffer += "\"";
		attr += 2;
	}
	d->buffer += ">";
}

void XMLCALL XmlAssociateDtd::end ( void *data, const XML_Char *el )
{
	DtdData *d;
	d = ( DtdData * ) data;
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
	DtdData *d;
	d = ( DtdData * ) data;
	d->insideDtd = true;
}

void XMLCALL XmlAssociateDtd::enddoctypehandler ( void *data )
{
	DtdData *d;
	d = ( DtdData * ) data;
	d->insideDtd = false;
}
