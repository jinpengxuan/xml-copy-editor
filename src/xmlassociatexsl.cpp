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

#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include <cstring>
#include "xmlassociatexsl.h"

XmlAssociateXsl::XmlAssociateXsl (
    const wxString &path,
    const char *encoding,
    size_t size )
    : WrapExpat ( encoding )
    , d ( new XslData() )
{
	d->buffer.reserve ( size );
	d->path = path;
	d->rootElementSeen = false;
	XML_SetUserData ( p, d.get() );
	XML_SetElementHandler ( p, start, end );
	XML_SetProcessingInstructionHandler ( p, processinghandler );
	XML_SetDefaultHandlerExpand ( p, defaulthandler );
}

XmlAssociateXsl::~XmlAssociateXsl()
{}

void XMLCALL XmlAssociateXsl::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
	XslData *d;
	d = ( XslData * ) data;
	d->buffer.append ( s, len );
}

void XMLCALL XmlAssociateXsl::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
	XslData *d;
	d = ( XslData * ) data;

	if ( !d->rootElementSeen )
	{
		d->buffer += "<?xml-stylesheet type=\"text/xsl\" href=\"";
		d->buffer += d->path.utf8_str(); // TODO: Apply the encoding of the parser
		d->buffer += "\"?>\n";
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

void XMLCALL XmlAssociateXsl::end ( void *data, const XML_Char *el )
{
	XslData *d;
	d = ( XslData * ) data;
	d->buffer += "</";
	d->buffer += el;
	d->buffer += ">";
}

void XMLCALL XmlAssociateXsl::processinghandler (
    void *data,
    const XML_Char *target,
    const XML_Char *datastring )
{
	XslData *d;
	d = ( XslData * ) data;

	if ( !strcmp ( target, "xml-stylesheet" ) )
		return;

	d->buffer += "<?";
	d->buffer += target;
	d->buffer += " ";
	d->buffer += datastring;
	d->buffer += "?>";
}
