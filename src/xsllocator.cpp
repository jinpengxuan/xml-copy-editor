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
#include "xsllocator.h"

XslLocator::XslLocator ( const char *encoding )
    : WrapExpat ( encoding )
    , d ( new XslLocatorData() )
{
	d->parser = p;
	XML_SetUserData ( p, d.get() );
	XML_SetProcessingInstructionHandler ( p, processingInstructionHandler );
}

XslLocator::~XslLocator()
{}

void XMLCALL XslLocator::processingInstructionHandler (
    void *userData,
    const XML_Char *target,
    const XML_Char *data )
{
	XslLocatorData *d;
	d = ( XslLocatorData * ) userData;

	if ( strcmp ( target, "xml-stylesheet" ) )
		return;

	const char *value, *iterator;
	value = strstr ( ( const char * ) data, "href=" );
	if ( !value || strlen ( value ) < 7 )
		return;
	value += 6;
	iterator = value;
	for ( iterator = value; *iterator; ++iterator )
	{
		if ( *iterator == '"' || *iterator == '\'' )
		{
			break;
		}
	}

	d->xslLocation.assign( value, iterator - value );
	XML_StopParser ( d->parser, false );
}


std::string XslLocator::getXslLocation()
{
	return d->xslLocation;
}
