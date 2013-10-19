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
#include <map>
#include <set>
#include <cstring>
#include "xmlfilterreader.h"

using namespace std;

XmlFilterReader::XmlFilterReader() : fd ( new FilterData() )
{
	fd->filterCount = 0;
	fd->setState ( STATE_UNKNOWN );
	XML_SetUserData ( p, fd.get() );
	XML_SetElementHandler ( p, start, end );
	XML_SetCharacterDataHandler ( p, characterdata );
}

XmlFilterReader::~XmlFilterReader()
{}

int XmlFilterReader::getFilterCount()
{
	return fd->filterCount;
}

void XmlFilterReader::getExcludeVector ( vector<string> &v )
{
	v = fd->excludeVector;
}

void XmlFilterReader::getIncludeVector ( vector<string> &v )
{
	v = fd->includeVector;
}

void XmlFilterReader::getFilterMap (
    map<string, map<string, set<string> > > &m
)
{
	m = fd->filterMap;
}

void XMLCALL XmlFilterReader::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
	FilterData *fd;
	fd = ( FilterData * ) data;

	if ( !strcmp ( el, "element" ) )
		fd->setState ( STATE_IN_ELEMENT );
	else if ( !strcmp ( el, "attribute" ) )
		fd->setState ( STATE_IN_ATTRIBUTE );
	else if ( !strcmp ( el, "key" ) )
		fd->setState ( STATE_IN_ATTRIBUTE_KEY );
	else if ( !strcmp ( el, "value" ) )
		fd->setState ( STATE_IN_ATTRIBUTE_VALUE );
	else if ( !strcmp ( el, "exclude" ) )
		fd->setState ( STATE_IN_EXCLUDE );
	else if ( !strcmp ( el, "include" ) )
		fd->setState ( STATE_IN_INCLUDE );
	else
		fd->setState ( STATE_UNKNOWN );
}

void XMLCALL XmlFilterReader::end ( void *data, const XML_Char *el )
{
	FilterData *fd;
	fd = ( FilterData * ) data;

	if ( !strcmp ( el, "element" ) )
	{
		fd->filterMap.insert ( make_pair ( fd->elementString, fd->attributeMap ) );
		( fd->filterCount ) ++;

		fd->elementString = "";
		fd->attributeKeyString = "";
		fd->attributeValueString = "";
		fd->attributeMap.clear();
		fd->setState ( STATE_UNKNOWN );
	}

	else if ( !strcmp ( el, "attribute" ) )
	{
		fd->attributeMap.insert (
		    make_pair ( fd->attributeKeyString, fd->attributeValueSet )
		);
		fd->setState ( STATE_UNKNOWN );
	}
	else if ( !strcmp ( el, "key" ) )
		fd->setState ( STATE_UNKNOWN );
	else if ( !strcmp ( el, "value" ) )
	{
		fd->attributeValueSet.insert ( fd->attributeValueString );
		fd->setState ( STATE_UNKNOWN );
		fd->attributeValueString = "";
	}

	// handle excludes/includes
	else if ( !strcmp ( el, "exclude" ) )
	{
		fd->excludeVector.push_back ( fd->excludeString );
		fd->excludeString = "";
		fd->setState ( STATE_UNKNOWN );
	}
	else if ( !strcmp ( el, "include" ) )
	{
		fd->includeVector.push_back ( fd->includeString );
		fd->includeString = "";
		fd->setState ( STATE_UNKNOWN );
	}
}

void XMLCALL XmlFilterReader::characterdata (
    void *data,
    const XML_Char *s,
    int len )
{
	FilterData *fd;
	fd = ( FilterData * ) data;

	switch ( fd->getState() )
	{
		case STATE_IN_ELEMENT:
			fd->elementString.append ( s, len );
			break;
		case STATE_IN_ATTRIBUTE_KEY:
			fd->attributeKeyString.append ( s, len );
			break;
		case STATE_IN_ATTRIBUTE_VALUE:
			fd->attributeValueString.append ( s, len );
			break;
		case STATE_IN_EXCLUDE:
			fd->excludeString.append ( s, len );
			break;
		case STATE_IN_INCLUDE:
			fd->includeString.append ( s, len );
			break;
		default:
			break;
	}
}
