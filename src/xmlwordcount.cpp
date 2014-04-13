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
#include <stdexcept>
#include "xmlwordcount.h"
#include "getword.h"

XmlWordCount::XmlWordCount ( const char *encoding )
	: WrapExpat ( encoding, false )
	, wcd ( new WordCountData() )
{
	wcd->wordCount = 0;
	XML_SetUserData ( p, wcd.get() );
	XML_SetCharacterDataHandler ( p, characterdata );
	XML_SetEndElementHandler ( p, end );
}

XmlWordCount::~XmlWordCount()
{}

int XmlWordCount::getWordCount()
{
	return wcd->wordCount;
}

void XMLCALL XmlWordCount::characterdata (
    void *data,
    const XML_Char *s,
    int len )
{
	WordCountData *wcd;
	wcd = ( WordCountData * ) data;
	wcd->buffer.append ( s, len );
}

void XMLCALL XmlWordCount::end ( void *data, const XML_Char *el )
{
	WordCountData *wcd;
	wcd = ( WordCountData * ) data;

	char *s = ( char * ) wcd->buffer.c_str();
	size_t len, myCount;
	myCount = 0;
	while ( GetWord::run ( &s, &len ) )
		++myCount;
	wcd->wordCount += myCount;
	wcd->buffer = "";
}
