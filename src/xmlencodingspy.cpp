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
#include "xmlencodingspy.h"

XmlEncodingSpy::XmlEncodingSpy ( const char *encoding )
	: WrapExpat ( encoding )
	, d ( new EncodingData() )
{
	XML_SetUserData ( p, d.get() );
	XML_SetXmlDeclHandler ( p, xmldeclhandler );
	XML_SetStartElementHandler ( p, start );
	d->p = p;
}

XmlEncodingSpy::~XmlEncodingSpy()
{}

void XMLCALL XmlEncodingSpy::xmldeclhandler (
    void *data,
    const XML_Char *version,
    const XML_Char *encoding,
    int standalone )
{
	EncodingData *d;
	d = ( EncodingData * ) data;
	d->encoding = ( encoding ) ? encoding : "UTF-8";
}

void XMLCALL XmlEncodingSpy::start (
    void *data,
    const XML_Char *el,
    const XML_Char **attr )
{
	EncodingData *d;
	d = ( EncodingData * ) data;

	if ( !d->encoding.size() )
		d->encoding = "UTF-8";

	XML_StopParser ( d->p, false );
}
