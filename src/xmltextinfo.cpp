/*
 * Copyright 2014 Zane U. Ji.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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

#include "xmltextinfo.h"

XmlTextInfo::XmlTextInfo
	( const wxString &path
	, const char *encoding /*= NULL*/
	)
	: WrapExpat ( encoding )
{
	XML_SetUserData ( p, this );
	XML_SetBase ( p, path.utf8_str() );
	XML_SetDoctypeDeclHandler ( p, startdoctypehandler, NULL );
	XML_SetProcessingInstructionHandler ( p, processinghandler );
	XML_SetElementHandler ( p, start, NULL );
	XML_SetDefaultHandlerExpand ( p, defaulthandler );
}

XmlTextInfo::~XmlTextInfo()
{
}

void XMLCALL XmlTextInfo::startdoctypehandler
	( void *data
	, const XML_Char *doctypeName
	, const XML_Char *sysid
	, const XML_Char *pubid
	, int has_internal_subset
	)
{
	XmlTextInfo *pThis = (XmlTextInfo *)data;

	pThis->mDocTypeName = wxString::FromUTF8 ( doctypeName );
}

void XMLCALL XmlTextInfo::processinghandler
	( void *data
	, const XML_Char *target
	, const XML_Char *datastring
	)
{
	XmlTextInfo *pThis = (XmlTextInfo *)data;

	if ( strcmp ( target, "xml-stylesheet" ) )
		return;

	char *href = strstr ( ( char * ) datastring, "href" );
	if ( !href )
		return;
	href += strlen ( "href" );

	char ch = '"'; // Is '' or "" used?
	char *start = strchr ( href, ch);
	char *p = strchr ( href, '\'');
	if ( start == NULL
		|| ( p != NULL && start > p ) )
	{
		start = p;
		ch = '\'';
	}
	if ( !start++ )
		return;
	while ( isspace ( *start ) )
		start++;

	char *end = strchr ( start, ch);
	if ( !end )
		return;
	while ( isspace ( end[-1] ) )
		end--;

	pThis->mXslFile = wxString::FromUTF8 ( start, end - start );
}

void XMLCALL XmlTextInfo::start
	( void *data
	, const XML_Char *el
	, const XML_Char **attr
	)
{
	XmlTextInfo *pThis = (XmlTextInfo *)data;

	for ( ; *attr; attr += 2 )
	{
		if ( strstr ( *attr, ":noNamespaceSchemaLocation" ) )
		{
			pThis->mXsdFile = wxString::FromUTF8 ( attr[1] );
			break;
		}
		if ( strstr ( *attr, ":schemaLocation" ) )
		{ // TODO: Support multiple schemas
			std::string str ( attr[1] );
			char *p = strtok ( ( char * ) str.c_str(), " \t\r\n" );
			if ( !p )
				continue;
			p = strtok ( NULL, " \t\r\n" );
			if ( !p )
				continue;
			pThis->mXsdFile = wxString::FromUTF8 ( p );
			break;
		}
	}

	pThis->mDocTypeName = wxString::FromUTF8 ( el );

	XML_SetElementHandler ( pThis->p, NULL, NULL );
}

void XMLCALL XmlTextInfo::defaulthandler
	( void *data
	, const XML_Char *s
	, int len
	)
{
	XmlTextInfo *pThis = (XmlTextInfo *)data;
	const char *p = s;
	const char *end = s + len;
	while ( p < end && *p != '\r' )
		p++;
	if ( p < end )
	{
		if ( p + 1 < end && p[1] == '\n' )
			pThis->mEOL = _T ( "\r\n" );
		else
			pThis->mEOL = _T ( "\n" );
		XML_SetDefaultHandlerExpand ( pThis->p, NULL );
		return;
	}

	p = s;
	while ( p < end && *p != '\r')
		p++;
	if ( p < end )
	{
		pThis->mEOL = _T ( "\n" );
		XML_SetDefaultHandlerExpand ( pThis->p, NULL );
	}
}
