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
#include <cstring>
#include <expat.h>
#include "xmlschemalocator.h"

XmlSchemaLocator::XmlSchemaLocator ( const char *encoding )
	: WrapExpat ( encoding, true )
	, d ( new SchemaLocatorData() )
{
	d->parser = p;
	XML_SetUserData ( p, d.get() );
	XML_SetStartElementHandler ( p, starthandler );
}

XmlSchemaLocator::~XmlSchemaLocator()
{}

void XMLCALL XmlSchemaLocator::starthandler (
    void *data,
    const XML_Char *el,
    const XML_Char **attr )
{
	SchemaLocatorData *d;
	d = ( SchemaLocatorData * ) data;

	while ( *attr )
	{
		if (
		    !strcmp ( *attr, "http://www.w3.org/2001/XMLSchema-instance:noNamespaceSchemaLocation" ) ||
		    !strcmp ( *attr, "http://www.w3.org/2001/XMLSchema-instance:schemaLocation" ) )
		{
			d->schemaLocation = * ( attr + 1 );
			XML_StopParser ( d->parser, false );
		}
		attr += 2;
	}
}

std::string XmlSchemaLocator::getSchemaLocation()
{
	return d->schemaLocation;
}
