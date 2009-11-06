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

#include <cstring>
#include "xmlencodinghandler.h"

bool XmlEncodingHandler::hasDeclaration ( const std::string& utf8 )
{
	size_t len = utf8.size();
	if ( len < 6 ||
	        utf8[0] != '<' ||
	        utf8[1] != '?' ||
	        utf8[2] != 'x' ||
	        utf8[3] != 'm' ||
	        utf8[4] != 'l' )
		return false;
	return true;
}

std::string XmlEncodingHandler::get (
    const std::string& utf8 )
{
	if ( !hasDeclaration ( utf8 ) )
		return "UTF-8";

	size_t len = utf8.size();
	std::string s;
	for ( size_t i = 0; i < len; ++i )
	{
		s += utf8[i];
		if ( utf8[i] == '>' )
			break;
	}

	if ( s.find ( ">" ) == std::string::npos )
		return "";

	s = CaseHandler::lowerCase ( s );

	if ( s.find ( "encoding=" ) == std::string::npos ||
	        s.find ( "utf-8" ) != std::string::npos )
		return "UTF-8";
	else if ( s.find ( "utf-16le" ) != std::string::npos )
		return "UTF-16LE";
	else if ( s.find ( "utf-16be" ) != std::string::npos )
		return "UTF-16BE";
	else if ( s.find ( "utf-16" ) != std::string::npos )
		return "UTF-16";
	else if ( s.find ( "iso-8859-1" ) != std::string::npos )
		return "ISO-8859-1";
	else if ( s.find ( "us-ascii" ) != std::string::npos )
		return "US-ASCII";
	else
		return "";
}

bool XmlEncodingHandler::setUtf8 ( std::string& utf8, bool ignoreCurrentEncoding )
{
	if ( !hasDeclaration ( utf8 ) )
		return true;

	if ( get ( utf8 ).empty() && !ignoreCurrentEncoding )
		return false;

	std::pair<int, int> p = getEncodingValueLimits ( utf8 );
	if ( p.first == -1 || p.second == -1 )
		return true; // attribute missing

	utf8.replace ( p.first, p.second, "UTF-8" );

	return true;
}

bool XmlEncodingHandler::set ( std::string& buffer, std::string& encoding )
{
	std::pair<int, int> p = getEncodingValueLimits ( buffer );
	if ( p.first == -1 || p.second == -1 )
		return false; // attribute missing
	buffer.replace ( p.first, p.second, encoding.c_str() );
	return true;
}

std::pair<int, int>
XmlEncodingHandler::getEncodingValueLimits ( const std::string& utf8 )
{
	if ( !hasDeclaration ( utf8 ) )
		return make_pair ( -1, -1 );
	char *buffer, *start, *end;
	char delimit;
	char anchor[] = "encoding=";
	buffer = ( char * ) utf8.c_str();
	start = strstr ( buffer, anchor );
	if ( !start )
		return make_pair ( -1, -1 );
	start = start + strlen ( anchor );
	if ( *start != '\'' && *start != '\"' )
		return make_pair ( -1, -1 );
	delimit = *start;
	if ( strlen ( start ) < 2 )
		return make_pair ( -1, -1 );
	++start;
	for ( end = start; *end && *end != delimit; ++end )
	{
		if (
		    *end == '?' || *end == '>' || *end == '<' ||
		    *end == ' ' || *end == '\t' || *end == '\n' )
		{
			return make_pair ( -1, -1 );
		}
	}
	if ( !*end )
		return make_pair ( -1, -1 );
	return make_pair ( start - buffer, end - start );
}

