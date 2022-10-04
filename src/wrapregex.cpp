/*
 * Copyright 2005-2007 Gerald Schmidt.
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

#include <iostream>
#include <string>
#include <ctype.h>
#include <stdexcept>
#include <cstring>
#include "wrapregex.h"
#include "contexthandler.h"

using namespace std;

WrapRegex::WrapRegex (
    const string& pattern,
    bool matchCase,
    const string& replaceParameter ) :
		replace ( replaceParameter ),
		returnValue ( 0 )
{
	if ( pattern.empty() || pattern == ".*" )
	{
		disabled = true;
		patternCode = NULL;
		patternMatchData = NULL;
		patternMatchContext = NULL;
		return;
	}
	disabled = false;

	// compile
	uint32_t optionsFlag = ( matchCase ? 0 : PCRE2_CASELESS ) | PCRE2_UTF | PCRE2_NO_UTF_CHECK;
	int errorCode;
	PCRE2_SIZE errorOffset;

	if ( ( patternCode = pcre2_compile (
		(PCRE2_SPTR)pattern.c_str(), // pattern
		PCRE2_ZERO_TERMINATED, // pattern is zero-terminated
		optionsFlag, // options
		&errorCode, // error number
		&errorOffset, // error offset
		NULL ) ) == NULL ) // default compile context
	{
		char buf[256];
		pcre2_get_error_message ( errorCode, (PCRE2_UCHAR *)buf, sizeof(buf) );
		throw runtime_error ( string(buf) );
	}
	patternMatchData = pcre2_match_data_create_from_pattern ( patternCode, NULL );
	patternMatchContext = pcre2_match_context_create ( NULL );
}

WrapRegex::~WrapRegex()
{
	if ( disabled )
		return;

	pcre2_match_data_free ( patternMatchData );
	pcre2_code_free ( patternCode );
	pcre2_match_context_free ( patternMatchContext );
}

int WrapRegex::matchPatternGlobal (
    string &buffer,
    vector<ContextMatch> &matchVector,
    unsigned elementCount,
    int context )
{
	if ( disabled )
		return 0;

	return matchPatternGlobal_ (
	           buffer.c_str(),
	           buffer.size(),
	           matchVector,
	           elementCount,
	           context );
}

string WrapRegex::replaceGlobal (
    const string& buffer,
    int *matchCount )
{
	*matchCount = 0;

	if ( disabled )
		return buffer;

	const char *s = buffer.c_str();

	string output, match;

	output.reserve ( buffer.size() );
	while ( ( returnValue = pcre2_match (
		patternCode, // compiled pattern
		(PCRE2_SPTR)s, // subject string
		strlen ( s ), // length of the subject
		0, // start at offset 0 in the subject
		0, // default options
		patternMatchData, // block where results will be stored
		patternMatchContext ) ) >= 0 ) // match context
	{
		++ ( *matchCount );

		PCRE2_SIZE *matchArray = pcre2_get_ovector_pointer ( patternMatchData );
		output.append ( s, matchArray[0] );

		match.clear();
		match.append ( s + matchArray[0], matchArray[1] - matchArray[0] );
		output.append ( getInterpolatedString_ ( s, ( char * ) replace.c_str() ) );
		s += matchArray[1];
	}
	output.append ( s );
	return output;
}

int WrapRegex::matchPatternGlobal_ (
    const char *buffer,
    size_t buflen,
    vector<ContextMatch> &matchVector,
    unsigned elementCount,
    int context )
{
	if ( disabled )
		return 0;

	const char *s, *origin;
	int matchcount;
	size_t offset;
	ContextMatch match;

	s = origin = buffer;
	matchcount = 0;
	offset = 0;

	while ( ( returnValue = pcre2_match (
		patternCode, // compiled pattern
		(PCRE2_SPTR)s, // subject string
		buflen, // length of the subject
		offset, // start at this offset in the subject
		0, // default options
		patternMatchData, // block where results will be stored
		patternMatchContext ) ) >= 0 ) // match context
	{
		++matchcount;

		PCRE2_SIZE *matchArray = pcre2_get_ovector_pointer ( patternMatchData );
		if ( context )
		{
			match = ContextHandler::getContext (
			            s + matchArray[0],
			            matchArray[1] - matchArray[0],
			            origin,
			            context );
		}
		else
		{
			match.prelog = match.postlog = "";
			match.match.assign ( s + matchArray[0], matchArray[1] - matchArray[0] );
		}

		// record element and offset information
		match.elementCount = elementCount;
		match.offset = matchArray[0];

		if ( replace != "" )
			match.replace = getInterpolatedString_ ( s, ( char * ) replace.c_str() );

		matchVector.push_back ( match );

		if ( ( offset = matchArray[1] ) >= buflen )
			break;
	}
	return matchcount;
}

string WrapRegex::getInterpolatedString_ ( const char *buffer, const char *source )
{
	if ( disabled )
		return "";

	const char *s = source;

	string interpol_string;

	int escapeState = false;
	for ( ; *s; ++s )
	{
		if ( *s == '\\' )
		{
			escapeState = ( escapeState ) ? false : true;
			if ( escapeState )
			{
				if ( isdigit ( * ( s + 1 ) ) )
				{
					const char *number, *it;
					number = s + 1;
					for ( it = number; *it && isdigit ( * ( it + 1 ) ); ++it )
						;
					size_t len = it - s;
					char *tmp = new char[len + 1];
					memcpy ( tmp, number, sizeof ( char ) * len );
					* ( tmp + len ) = '\0';
					int i = atoi ( tmp );
					delete[] tmp;

					interpol_string += getSubpattern_ ( buffer, i );

					s += len;
					escapeState = false;
				}
				else if ( * ( s + 1 ) == 't' )
				{
					interpol_string += '\t';
					++s;
					escapeState = false;
				}
				else if ( * ( s + 1 ) == 'n' )
				{
					interpol_string += '\n';
					++s;
					escapeState = false;
				}
				else
					interpol_string += *s;
			}
			else
				interpol_string += *s;
		}
		else
			interpol_string += *s;
	}
	return interpol_string;
}

string WrapRegex::getSubpattern_ ( const char *s, unsigned subpattern )
{
	if ( disabled )
		return "";

	char *sub = NULL;
	size_t sublen;
	int ret = pcre2_substring_get_bynumber (
		patternMatchData,
		subpattern,
		(PCRE2_UCHAR **)sub,
		&sublen
	);
	if ( ret == PCRE2_ERROR_NOMATCH || ret == PCRE2_ERROR_BADDATA )
		return "";
	string subString ( sub );
	pcre2_substring_free ( (PCRE2_UCHAR *)sub );
	return subString;
}
