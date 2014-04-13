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
#include "housestylereader.h"

using namespace std;

HouseStyleReader::HouseStyleReader (
	map<string, map<string, set<string> > > &m
	, const char *encoding /*= NULL*/
	)
	: WrapExpat ( encoding, true )
	, ud ( new HouseStyleReaderData() )
{
	ud->setState ( STATE_UNKNOWN );
	ud->depth = ud->cutoffDepth = 0;
	ud->filterActive = false;
	ud->filterMap = m;

	XML_SetUserData ( p, ud.get() );
	XML_SetElementHandler ( p, start, end );
	XML_SetCharacterDataHandler ( p, characterdata );
	XML_SetCdataSectionHandler ( p, cdatastart, cdataend );
}

HouseStyleReader::~HouseStyleReader()
{}

void HouseStyleReader::getNodeVector ( vector<pair<string, unsigned> > &v )
{
	v = ud->nodevector;
}

void XMLCALL HouseStyleReader::start ( void *data, const XML_Char *el, const XML_Char **attr )
{
	HouseStyleReaderData *ud;
	ud = ( HouseStyleReaderData * ) data;
	ud->setState ( STATE_ON_START );
	++ ( ud->depth );

	if ( ud->textnode.size() )
	{
		if ( !ud->filterActive )
			ud->nodevector.push_back ( make_pair ( ud->textnode, ud->getCount() ) );
		ud->textnode = "";
	}

	if ( !ud->filterActive && ud->filterMap.find ( el ) != ud->filterMap.end() )
	{
		map<string, set<string> > attributeMap;
		attributeMap = ud->filterMap[el];

		// no attribute keys/values specified
		if ( attributeMap.empty() )
		{
			ud->filterActive = true;
			ud->cutoffDepth = ud->depth;
		}

		// examine attribute keys/values
		else
		{

			for ( ; *attr; attr += 2 )
			{
				char *key, *value;
				key = ( char * ) *attr;
				value = ( char * ) * ( attr + 1 );
				set<string> valueSet;
				valueSet = attributeMap[key];

				if ( attributeMap.find ( key ) != attributeMap.end() )
				{
					if ( valueSet.count ( value ) )
					{
						ud->filterActive = true;
						ud->cutoffDepth = ud->depth;
					}
				}
			}
		}
		ud->cutoffDepth = ud->depth;
	}
}

void XMLCALL HouseStyleReader::end ( void *data, const XML_Char *el )
{
	HouseStyleReaderData *ud;
	ud = ( HouseStyleReaderData * ) data;

	ud->setState ( STATE_ON_END );
	-- ( ud->depth );
	if ( ud->textnode.size() )
	{
		if ( !ud->filterActive )
			ud->nodevector.push_back ( make_pair ( ud->textnode, ud->getCount() ) );
		ud->textnode = "";
	}
	if ( ud->filterActive && ud->depth < ud->cutoffDepth )
	{
		ud->cutoffDepth = 0;
		ud->filterActive = false;
	}
}

void XMLCALL HouseStyleReader::characterdata ( void *data, const XML_Char *s, int len )
{
	HouseStyleReaderData *ud;
	ud = ( HouseStyleReaderData * ) data;
	ud->textnode.append ( s, len );
}

void XMLCALL HouseStyleReader::cdatastart ( void *data )
{
	HouseStyleReaderData *ud;
	ud = ( HouseStyleReaderData * ) data;
	ud->setState ( STATE_ON_CDATA_START );

	if ( ud->textnode.size() )
	{
		if ( !ud->filterActive )
			ud->nodevector.push_back ( make_pair ( ud->textnode, ud->getCount() ) );
		ud->textnode = "";
	}
}

void XMLCALL HouseStyleReader::cdataend ( void *data )
{
	HouseStyleReaderData *ud;
	ud = ( HouseStyleReaderData * ) data;
	ud->setState ( STATE_ON_CDATA_END );

	if ( ud->textnode.size() )
	{
		if ( !ud->filterActive )
			ud->nodevector.push_back ( make_pair ( ud->textnode, ud->getCount() ) );
		ud->textnode = "";
	}
}
