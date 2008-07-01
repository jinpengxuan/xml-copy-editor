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

#include <iostream>
#include <stdexcept>
#include "wrapaspell.h"
#include "aspell.h"
#include "casehandler.h"
#include "contexthandler.h"
#include "getword.h"

#ifdef __WXMSW__
       #include "aspellpaths.h"
#endif

WrapAspell::WrapAspell ( std::string lang )
{
	spell_config = new_aspell_config();
	
#ifdef __WXMSW__
       aspell_config_replace ( spell_config, "data-dir", ASPELL_DATA_PATH );
       aspell_config_replace ( spell_config, "dict-dir", ASPELL_DICT_PATH );
#endif
	
	aspell_config_replace ( spell_config, "lang", lang.c_str() );	
	AspellCanHaveError * possible_err = new_aspell_speller ( spell_config );
	spell_checker = 0;
	if ( aspell_error_number ( possible_err ) != 0)
	{
		puts ( aspell_error_message ( possible_err ) );
		throw;
	}
	else
		spell_checker = to_aspell_speller ( possible_err ); 
}

WrapAspell::~WrapAspell()
{ 
	delete_aspell_speller ( spell_checker ); 
	delete_aspell_config ( spell_config );
}

bool WrapAspell::checkWord ( std::string &s )
{
	return checkWord ( (char *) s.c_str(), s.size() );
}

std::string WrapAspell::getSuggestion (
    std::string &s )
{
	const AspellWordList *suggestions = aspell_speller_suggest ( spell_checker, s.c_str(), s.size() );
	AspellStringEnumeration *elements = aspell_word_list_elements ( suggestions );
	const char *word = aspell_string_enumeration_next ( elements ); // no iteration req'd
	return (word) ? word : "----";
}

void WrapAspell::checkString (
    std::string &s,
    std::vector<ContextMatch> &v,
    int contextRange )
{
	std::string suggestion;
	size_t len;
	char *origin, *iterator, *ptr;
	origin = iterator = ( char * ) s.c_str();
	while ( ( ptr = GetWord::run ( &iterator, &len, true ) ) != NULL )
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

bool WrapAspell::checkWord ( char *s, size_t len )
{
	return aspell_speller_check ( spell_checker, s, len ); 
}

