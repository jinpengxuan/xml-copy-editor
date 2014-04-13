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
#include <cstring>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlschemaparser.h"

XmlSchemaParser::XmlSchemaParser (
    PromptGeneratorData *data,
    const char *encoding,
    bool nameSpaceAware
    )
    : WrapExpat ( encoding, nameSpaceAware )
    , d ( new SchemaParserData )
{
    d->promptData = data;
    d->setState ( STATE_UNKNOWN );
    XML_SetUserData ( p, d.get() );
    XML_SetElementHandler ( p, starthandler, endhandler );
}

XmlSchemaParser::~XmlSchemaParser()
{}

void XMLCALL XmlSchemaParser::starthandler (
    void *data,
    const XML_Char *el,
    const XML_Char **attr )
{
    SchemaParserData *d;
    d = ( SchemaParserData * ) data;
    if ( !strcmp ( el, "http://www.w3.org/2001/XMLSchema:complexType" ) )
    {
        d->setState ( STATE_COMPLEX_TYPE );
    }

    while ( *attr )
    {
        attr += 2;
    }
}

void XMLCALL XmlSchemaParser::endhandler ( void *data, const XML_Char *el )
{
    return;
}
