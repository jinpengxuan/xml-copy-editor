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

#include <set>
#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "housestylewriter.h"
#include "contexthandler.h"

using namespace std;

HouseStyleWriter::HouseStyleWriter ( vector<ContextMatch> &v ) :
		hswd ( new HouseStyleWriterData() )
{
	set<size_t> elementSet;
	size_t vectorsize = v.size();
	for ( size_t i = 0; i < vectorsize; ++i )
		elementSet.insert ( v[i].elementCount );

	hswd->setState ( STATE_UNKNOWN );
	hswd->v = v;
	hswd->elementSet = elementSet;

	XML_SetUserData ( p, hswd.get() );
	XML_SetElementHandler ( p, start, end );
	XML_SetCharacterDataHandler ( p, characterdata );
	XML_SetXmlDeclHandler ( p, xmldecl );
	XML_SetCdataSectionHandler ( p, cdatastart, cdataend );
	XML_SetDefaultHandler ( p, defaulthandler );
}

HouseStyleWriter::~HouseStyleWriter()
{}

string HouseStyleWriter::getOutput()
{
	return hswd->output;
}

void XMLCALL HouseStyleWriter::start ( void *data, const XML_Char *el, const XML_Char **attr )
{
	HouseStyleWriterData *hswd;
	hswd = ( HouseStyleWriterData * ) data;
	hswd->setState ( STATE_ON_START );

	if ( hswd->textnode.size() )
	{
		if ( hswd->elementSet.count ( hswd->getCount() ) )
			modify ( hswd->v, hswd->textnode, hswd->getCount() );
		hswd->output += xmliseTextNode ( hswd->textnode );
		hswd->textnode = "";
	}

	hswd->output += '<';
	hswd->output += el;
	for ( ; *attr; attr += 2 )
	{
		hswd->output += ' ';
		hswd->output += *attr;
		hswd->output += "=\"";
		hswd->output += xmliseAttribute ( * ( attr + 1 ) );
		hswd->output += '"';
	}
	hswd->output += '>';
}

void XMLCALL HouseStyleWriter::end ( void *data, const XML_Char *el )
{
	HouseStyleWriterData *hswd;
	hswd = ( HouseStyleWriterData * ) data;
	//++(hswd->elementCount);
	hswd->setState ( STATE_ON_END );

	if ( hswd->textnode.size() )
	{
		if ( hswd->elementSet.count ( hswd->getCount() ) )
			modify ( hswd->v, hswd->textnode, hswd->getCount() );
		hswd->output += xmliseTextNode ( hswd->textnode );
		hswd->textnode = "";
	}
	hswd->output += "</";
	hswd->output += el;
	hswd->output += '>';
}

void XMLCALL HouseStyleWriter::characterdata ( void *data, const XML_Char *s, int len )
{
	HouseStyleWriterData *hswd;
	hswd = ( HouseStyleWriterData * ) data;
	hswd->textnode.append ( s, len );
}

void XMLCALL HouseStyleWriter::xmldecl (
    void *data,
    const XML_Char *version,
    const XML_Char *encoding,
    int standalone )
{
	HouseStyleWriterData *hswd;
	hswd = ( HouseStyleWriterData * ) data;
	hswd->output += "<?xml version=\"";
	hswd->output += version;
	hswd->output += "\" encoding=\"UTF-8\"";
	if ( standalone != -1 )
	{
		hswd->output += " standalone=\"";
		hswd->output += ( standalone ) ? "yes" : "no";
		hswd->output += "\"";
	}
	hswd->output += "?>";
}

void XMLCALL HouseStyleWriter::cdatastart ( void *data )
{
	HouseStyleWriterData *hswd;
	hswd = ( HouseStyleWriterData * ) data;
	hswd->setState ( STATE_ON_CDATA_START );

	if ( hswd->textnode.size() )
	{
		if ( hswd->elementSet.count ( hswd->getCount() ) )
			modify ( hswd->v, hswd->textnode, hswd->getCount() );
		hswd->output += hswd->textnode;
		hswd->textnode = "";
	}

	hswd->output += "<![CDATA[";
}

void XMLCALL HouseStyleWriter::cdataend ( void *data )
{
	HouseStyleWriterData *hswd;
	hswd = ( HouseStyleWriterData * ) data;
	hswd->setState ( STATE_ON_CDATA_END );

	if ( hswd->textnode.size() )
	{
		if ( hswd->elementSet.count ( hswd->getCount() ) )
			modify ( hswd->v, hswd->textnode, hswd->getCount() );
		hswd->output += hswd->textnode;
		hswd->textnode = "";
	}

	hswd->output += "]]>";
}

void XMLCALL HouseStyleWriter::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
	HouseStyleWriterData *hswd;
	hswd = ( HouseStyleWriterData * ) data;
	hswd->output.append ( s, len );
}

void HouseStyleWriter::modify (
    vector<ContextMatch> &v,
    string &buffer,
    unsigned elementCount )
{
	int vectorsize, os_adjust, exclusion;
	vectorsize = v.size();
	os_adjust = exclusion = 0;
	string cmp1, cmp2;

	for ( int i = 0; i < vectorsize; ++i )
	{
		unsigned vectorElementCount = v[i].elementCount;
		if ( vectorElementCount < elementCount )
			continue;
		else if ( vectorElementCount > elementCount )
			break;
		else if ( vectorElementCount == elementCount )
		{
			int offset = ( int ) v[i].offset + os_adjust;

			if ( offset < exclusion )
				continue;

			// check match is as expected
			try
			{
				cmp1 = v[i].match;
				cmp2 = buffer.substr ( offset, v[i].match.size() );
			}
			catch ( std::exception& e )
			{
				continue;
			}

			if ( cmp1.compare ( cmp2 ) )
				continue;

			buffer.replace ( offset, v[i].match.size(), v[i].replace.c_str() );

			os_adjust += v[i].replace.size() - v[i].match.size();
			exclusion = offset + v[i].replace.size();
		}
	}
}
