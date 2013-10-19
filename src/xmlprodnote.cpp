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
#include "xmlprodnote.h"

XmlProdnote::XmlProdnote (
    bool parseDeclaration,
    bool expandInternalEntities,
    size_t size ) :
		d ( new ProdnoteData() )
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

	XML_SetElementHandler ( p, start, end );
	d->level = 100;
}

XmlProdnote::~XmlProdnote()
{}

void XMLCALL XmlProdnote::xmldeclhandler (
    void *data,
    const XML_Char *version,
    const XML_Char *encoding,
    int standalone )
{
	ProdnoteData *d;
	d = ( ProdnoteData * ) data;

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

void XMLCALL XmlProdnote::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
	ProdnoteData *d;
	d = ( ProdnoteData * ) data;
	d->buffer.append ( s, len );
}

void XMLCALL XmlProdnote::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
	ProdnoteData *pd;
	pd = ( ProdnoteData * ) data;

    pd->level += 1;
    
    bool skip = false;
    
    std::string tag;
    tag += "<";
    tag += el;
    
    for ( ; *attr; attr += 2 )
    {
        tag += " ";
        tag += *attr;
        tag += "=\"";
        tag += *(attr + 1);
        tag += "\"";
		if ( !strcmp ( el, "span" ) && !strcmp (*attr, "class") )
		{
            if ( !strcmp ( *(attr + 1), "optional-prodnote" ) ||
                !strcmp ( *(attr + 1), "required-prodnote" ) )
            {
                skip = true;
                pd->level = 1;
                break;
            }
        }
    }
    
    tag += ">";
    
    if ( skip )
    {
        return;
    }

    pd->buffer += tag;
}

void XMLCALL XmlProdnote::end ( void *data, const XML_Char *el )
{
	ProdnoteData *pd;
	pd = ( ProdnoteData * ) data;
    
    pd->level -= 1;
    
    if ( !(pd->level) && !strcmp ( el, "span" ) )
    {
        pd->level = 100;
        return;
    }
    pd->buffer += "</";
    pd->buffer += el;
    pd->buffer += ">";
}
