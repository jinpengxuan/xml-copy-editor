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

#include "getword.h"

char *GetWord::run ( char **s, size_t *len, bool skipTagsActive )
{
	size_t bytes;
	char *t, *u;
	int offset;

	t = *s;
	bool openAngleBracketEndingWord = false;

	while ( *t )
	{
		if ( GetWord::isWordCharacter ( t, &bytes ) )
		{
			for ( u = t; *u; )
			{
				if ( !GetWord::isWordCharacter ( u, &bytes ) )
				{
					if (*u == '<')
						openAngleBracketEndingWord = true;	
					*len = u - t;
					offset = t - *s;
					*s += *len + offset;

					if (!openAngleBracketEndingWord)
						*s += bytes;
					
					return t;
				}
				else
					u += bytes;
			}
			if ( *t && ! ( *u ) )
			{
				*len = u - t;
				offset = t - *s;
				*s = u;
				return t;
			}
			t = u;
		}
		else if ( *t == '<' && skipTagsActive )
		{
			t = skipTags ( t );
		}
		else
		{
			t += bytes;
		}
	}
	return NULL;
}

bool GetWord::isWordCharacter ( char *s, size_t *bytes )
{
	*bytes = 0;

	unsigned char *us = ( unsigned char * ) s;
	if (
	    ( *us < 65  && *us != 45 ) ||
	    ( *us > 90 && *us < 97 ) ||
	    ( *us > 123 && *us < 128 )
	)
	{
		*bytes = 1;
		return false;
	}

	// check for UTF-8 byte sequences
	else if ( *us > 127 )
	{
		// determine length
		unsigned char *it = us;
		for ( ; *it > 127; ++it )
			++ ( *bytes );

		// Unicode punctuation marks
		// Based on http://www1.tip.nl/~t876506/utf8tbl.html
		return (
		           ( *us == 226 && * ( us + 1 ) == 128 ) ||
		           *us == 194 ||
		           *us == 203
		       ) ? false : true;
	}
	else
	{
		*bytes = 1;
		return true;
	}
}

char *GetWord::skipTags ( char *s )
{
	if (*s == '<')
	{
		// CDATA
		if (	* ( s + 1 ) == '!' &&
			* ( s + 2) == '[' &&
			* ( s + 3) == 'C'	)
		{
			s += 3;
			for ( ; *s; s++ )
			{
				if ( *s == ']' &&
					* (s + 1 ) == ']' &&
					* (s + 2 ) == '>')
				{
					return s += 3;
				}
			}
		}
		// comment
		else if (	* ( s + 1 ) == '!' &&
				* ( s + 2 ) == '-' &&
				* ( s + 3 ) == '-')
		{
			s += 3;
			for ( ; *s; s++ )
			{
				if ( *s == '-' &&
					* ( s + 1 ) == '-' &&
					* ( s + 2 ) == '>')
				{
					return s + 3;
				}
			}	
		}
		else
		{
			for ( ; *s; s++ )
			{
				if ( *s == '>' )
					return ++s;
			}
		}
	}
	return ++s;
}
