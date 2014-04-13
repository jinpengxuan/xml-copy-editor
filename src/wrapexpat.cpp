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

#include "wrapexpat.h"
//#include "expat.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

WrapExpat::WrapExpat ( const char *encoding, bool nameSpaceAware )
{
	p = ( nameSpaceAware ) ? XML_ParserCreateNS ( encoding, ( XML_Char ) ':' ) : XML_ParserCreate ( encoding );
	if ( p == 0 )
		throw runtime_error ( "WrapExpat::WrapExpat" );
}

WrapExpat::~WrapExpat()
{
	if ( p )
		XML_ParserFree ( p );
}

bool WrapExpat::parse ( const char *buffer, size_t size, bool isFinal )
{
	if ( !p )
		return false;
	if ( XML_Parse ( p, buffer, size, isFinal ) == XML_STATUS_ERROR )
		return false;
	return true;

}

bool WrapExpat::parse ( const string &buffer, bool isFinal )
{
	return parse ( buffer.c_str(), buffer.size(), isFinal );

	/*
	  if (!p)
	    return false;

	  if (XML_Parse(p, buffer.c_str(), buffer.size(), isFinal) == XML_STATUS_ERROR)
	    return false;
	  return true;
	*/

}

pair<int, int> WrapExpat::getErrorPosition()
{
	return make_pair (
	           XML_GetCurrentLineNumber ( p ),// - 1,
	           XML_GetCurrentColumnNumber ( p ) );
}

wxString WrapExpat::getLastError()
{
	if ( !p )
		return _ ( "Unable to create parser instance" );

	stringstream ss;
	ss << "Error at line ";
	ss << XML_GetCurrentLineNumber ( p );
	ss << ", column " << XML_GetCurrentColumnNumber ( p ) + 1 << ":" << endl;
	ss << XML_ErrorString ( XML_GetErrorCode ( p ) );
	return wxString ( ss.str().c_str(), wxConvUTF8 );
}

string WrapExpat::xmliseTextNode ( const string &textnode )
{
	size_t size = textnode.size();
	string output;

	for ( size_t i = 0; i < size; ++i )
	{
		char c = textnode[i];
		switch ( c )
		{
			case '<':
				output += "&lt;";
				break;
			case '>':
				output += "&gt;";
				break;
			case '&':
				output += "&amp;";
				break;
			default:
				output += textnode[i];
		}
	}
	return output;
}

string WrapExpat::xmliseAttribute ( const string &attribute )
{
	string intermediate = xmliseTextNode ( attribute );

	size_t size = intermediate.size();
	string output;

	for ( size_t i = 0; i < size; ++i )
	{
		char c = intermediate[i];
		switch ( c )
		{
			case '"':
				output += "&quot;";
				break;
			case '\'':
				output += "&apos;";
				break;
			default:
				output += intermediate[i];
		}
	}
	return output;
}

bool WrapExpat::isWhitespace ( const string &textnode )
{
	size_t size = textnode.size();
	for ( size_t i = 0; i < size; ++i )
		if ( !isspace ( textnode[i] ) )
			return false;
	return true;
}

bool WrapExpat::isEncodingError()
{
	if ( !p )
		return false;

	int errorCode = XML_GetErrorCode ( p );
	return (
	           errorCode == XML_ERROR_UNKNOWN_ENCODING ||
	           errorCode == XML_ERROR_INCORRECT_ENCODING ) ?
	       true : false;
}

