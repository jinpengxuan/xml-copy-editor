/*
 * Copyright 2018 Zane U. Ji.
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

#include "xercesnetaccessor.h"
#include "xmlcopyeditorcopy.h"

#include <wx/string.h>
#include <string.h>
#include <string>

const static char USER_AGENT[] = "User-Agent:";

XercesNetAccessor::XercesNetAccessor ( XMLNetAccessor *netAccessor )
	: mNetAccessor ( netAccessor )
{
}

const XMLCh *XercesNetAccessor::getId() const
{
	return mNetAccessor->getId();
}

BinInputStream *XercesNetAccessor::makeNew (
	const XMLURL &urlSrc,
	const XMLNetHTTPInfo *httpInfo /* = 0 */
) {

	XMLNetHTTPInfo info;
	if ( httpInfo != NULL )
	{
		info = *httpInfo;
	}
	std::string headers ( info.fHeaders, info.fHeadersLen );
	if ( headers.find ( USER_AGENT ) == std::string::npos )
	{
		headers.append ( USER_AGENT );
		headers.append ( " xmlcopyeditor/" );
		headers.append ( wxString ( ABOUT_VERSION ).utf8_str() );
		headers.append ( "\r\n" );
		info.fHeaders = headers.c_str();
		info.fHeadersLen = headers.length();
	}

	return mNetAccessor->makeNew ( urlSrc, &info );
}
