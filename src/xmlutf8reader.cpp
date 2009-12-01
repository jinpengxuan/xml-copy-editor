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
#include "xmlutf8reader.h"

XmlUtf8Reader::XmlUtf8Reader (
    bool parseDeclaration,
    bool expandInternalEntities,
    size_t size ) :
		d ( new UtfData() )
{
	d->buffer.reserve ( size );
	XML_SetUserData ( p, d.get() );

	// parse declaration?
	if ( parseDeclaration )
		XML_SetXmlDeclHandler ( p, xmldeclhandler );

	// internal entities
	if ( expandInternalEntities )
		XML_SetDefaultHandlerExpand ( p, defaulthandler );
	else
		XML_SetDefaultHandler ( p, defaulthandler );
}

XmlUtf8Reader::~XmlUtf8Reader()
{}

void XMLCALL XmlUtf8Reader::xmldeclhandler (
    void *data,
    const XML_Char *version,
    const XML_Char *encoding,
    int standalone )
{
	UtfData *d;
	d = ( UtfData * ) data;

	d->encoding = ( encoding ) ? encoding : "UTF-8";

	d->buffer.append ( "<?xml version=\"" );
	d->buffer.append ( version );
	d->buffer.append ( "\" encoding=\"" );
	d->buffer.append ( d->encoding );
	d->buffer.append ( "\"" );

	if ( standalone != -1 )
	{
		d->buffer.append ( " standalone=\"" );
		d->buffer.append ( ( standalone == 1 ) ? "yes" : "no" );
		d->buffer.append ( "\"" );
	}
	d->buffer.append ( "?>" );
}

void XMLCALL XmlUtf8Reader::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
	UtfData *d;
	d = ( UtfData * ) data;
	d->buffer.append ( s, len );
}
