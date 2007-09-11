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

#include "spellcheck.h"
#include "casehandler.h"
#include "contexthandler.h"
#include "stringset.h"

Spellcheck::Spellcheck (
    boost::shared_ptr<StringSet<char> > dictionaryParameter,
    boost::shared_ptr<StringSet<char> > passiveDictionaryParameter ) :
        dictionary ( dictionaryParameter ),
        passiveDictionary ( passiveDictionaryParameter )
{}

Spellcheck::~Spellcheck()
{ }

bool Spellcheck::checkWord ( string &s )
{
    return checkWord ( ( char * ) s.c_str(), s.size() );
}

string Spellcheck::getSuggestion (
    string &s )
{
    string suggestion;
    size_t len = s.size();

    // transpose
    if ( len > 1 )
    {
        for ( size_t ui = 0; ui < len - 1; ++ui )
        {
            if ( ( unsigned char ) s[ui] > 127 )
                continue;

            suggestion = s;
            char c = suggestion[ui];
            suggestion[ui] = suggestion[ui + 1];
            suggestion[ui + 1] = c;
            if (
                checkWord ( suggestion ) &&
                passiveDictionary->find ( suggestion ) == NULL )
            {
                CaseHandler::adjustCase ( suggestion, s );
                return suggestion;
            }
        }
    }

    // change one letter
    for ( size_t ui = 1; ui < len; ++ui )
    {
        if ( ( unsigned char ) s[ui] > 127 )
            continue;

        for ( char c = 'a'; c <= 'z'; ++c )
        {
            suggestion = s;
            suggestion[ui] = c;
            if (
                checkWord ( suggestion ) &&
                passiveDictionary->find ( suggestion ) == NULL )
            {
                CaseHandler::adjustCase ( suggestion, s );
                return suggestion;
            }
        }
    }

    // two words
    if ( len > 2 )
    {
        for ( size_t ui = 1; ui < len; ++ui )
        {
            char *ptr = ( char * ) s.c_str();
            if (
                checkWord ( ptr, ui ) &&
                checkWord ( ptr + ui, len - ui )
            )
            {
                suggestion = s;
                suggestion.insert ( ui, 1, ' ' );
                return suggestion;
            }
        }
    }

    // add one letter
    for ( size_t ui = 1; ui < len; ++ui )
    {
        if ( ( unsigned char ) s[ui] > 127 )
            continue;

        for ( char c = 'a'; c <= 'z'; ++c )
        {
            suggestion = s;
            suggestion.insert ( ui, 1, c );
            if (
                checkWord ( suggestion ) &&
                passiveDictionary->find ( suggestion ) == NULL )
            {
                CaseHandler::adjustCase ( suggestion, s );
                return suggestion;
            }
        }
    }

    // omit one letter
    if ( len > 2 )
    {
        for ( size_t ui = 0; ui < len; ++ui )
        {
            // ignore UTF-8 multibyte sequences
            if ( ( unsigned char ) s[ui] > 127 )
                continue;

            suggestion = s;
            suggestion.erase ( ui, 1 );
            if (
                checkWord ( suggestion ) &&
                passiveDictionary->find ( suggestion ) == NULL )
            {

                CaseHandler::adjustCase ( suggestion, s );
                return suggestion;
            }
        }
    }

    return "---";
}

void Spellcheck::checkString (
    string &s,
    vector<ContextMatch> &v,
    int contextRange )
{
    string suggestion;
    size_t len;
    char *origin, *iterator, *ptr;
    origin = iterator = ( char * ) s.c_str();
    while ( ( ptr = getWord ( &iterator, &len ) ) != NULL )
        if ( !checkWord ( ptr, len ) )
        {
            ContextMatch m = ContextHandler::getContext (
                                 ptr,
                                 len,
                                 origin,
                                 contextRange );

            // handle suggestion
            suggestion = getSuggestion ( m.match );
            m.replace.append ( suggestion );
            m.elementCount = 0;
            m.offset = ptr - origin;
            v.push_back ( m );
        }
}

bool Spellcheck::checkWord ( char *s, size_t len )
{
    // pass empty strings and single-character words
    if ( len < 2 )
        return true;

    string buffer;
    buffer.append ( s, len );
    if ( dictionary->find ( buffer ) != NULL )
        return true;

    // lower-case with capital initial
    string::iterator it = buffer.begin();
    ++it;
    for ( ; it != buffer.end(); it++ )
        *it = tolower ( *it );

    if ( dictionary->find ( buffer ) != NULL )
        return true;

    // lower-case throughout
    it = buffer.begin();
    *it = tolower ( *it );

    if ( dictionary->find ( buffer ) != NULL )
        return true;

    return false;
}

bool Spellcheck::isWordCharacter ( char *s, size_t *bytes )
{
    *bytes = 0;

    unsigned char *us = ( unsigned char * ) s;
    if (
        *us < 65 ||
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
                   *us == 226 && * ( us + 1 ) == 128 ||
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

char *Spellcheck::getWord ( char **s, size_t *len )
{
    size_t bytes;
    char *t, *u;
    int offset;

    t = *s;

    while ( *t )
    {
        if ( isWordCharacter ( t, &bytes ) )
        {
            for ( u = t; *u; )
            {
                if ( !isWordCharacter ( u, &bytes ) )
                {
                    *len = u - t;
                    offset = t - *s;
                    *s += *len + offset + bytes;
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
        else
            t += bytes;
    }
    return NULL;
}
