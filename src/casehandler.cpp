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
#include "casehandler.h"

CaseHandler::CaseHandler()
{ }

CaseHandler::~CaseHandler()
{ }

void CaseHandler::adjustCase ( string &buffer, string &comparison )
{
	string::iterator it;
	int countBuffer;
	bool initialCapital;

	countBuffer = 0;
	initialCapital = false;

	// preserve all caps
	for ( it = buffer.begin(); it != buffer.end(); ++it )
		if ( isalpha ( *it ) && isupper ( *it ) )
			++countBuffer;

	if ( countBuffer > 2 )
	{
		for ( it = buffer.begin(); it != buffer.end(); ++it )
			if ( isalpha ( *it ) )
				*it = toupper ( *it );
		return;
	}

	// preserve initial capital
	it = comparison.begin();
	if ( isalpha ( *it ) && isupper ( *it ) )
		initialCapital = true;

	for ( ++it; it != comparison.end(); ++it )
		if ( isalpha ( *it ) && isupper ( *it ) )
			return;

	if ( initialCapital )
	{
		it = buffer.begin();
		if ( isalpha ( *it ) )
			*it = toupper ( *it );
		for ( ++it; it != buffer.end(); ++it )
			if ( isalpha ( *it ) && isupper ( *it ) )
				*it = tolower ( *it );
	}
}

string CaseHandler::lowerCase ( string &s )
{
	string::iterator it;
	string localString ( s );
	for (
	    it = localString.begin();
	    it != localString.end();
	    ++it )
	{
		if ( *it < 0 )
			continue;
		*it = tolower ( *it );
	}
	return localString;
}
