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

#include <wx/intl.h>
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include "wrapaspell.h"
#ifdef USE_ENCHANT
#include <enchant++.h>
#endif
#include "casehandler.h"
#include "contexthandler.h"
#include "getword.h"

WrapAspell::WrapAspell (
                       const wxString& lang
#if !defined(USE_ENCHANT) && defined(__WXMSW__)
                       ,
                       const wxString& aspellDataPathParameter,
                       const wxString& aspellDictPathParameter
#endif
)
{
#ifdef USE_ENCHANT
	spell_broker = enchant::Broker::instance();
	spell_checker = spell_broker->request_dict( std::string ( lang.mb_str() ) );
#else
	spell_config = new_aspell_config();
	
#ifdef __WXMSW__
       aspell_config_replace ( spell_config, "data-dir", aspellDataPathParameter.mb_str() );//ASPELL_DATA_PATH );
       aspell_config_replace ( spell_config, "dict-dir", aspellDictPathParameter.mb_str() );//ASPELL_DICT_PATH );
#endif
	
	aspell_config_replace ( spell_config, "lang", lang.mb_str() );
	AspellCanHaveError * possible_err = new_aspell_speller ( spell_config );
	spell_checker = 0;
	if ( aspell_error_number ( possible_err ) != 0)
	{
		puts ( aspell_error_message ( possible_err ) );
		throw;
	}
	else
		spell_checker = to_aspell_speller ( possible_err ); 
#endif
}

WrapAspell::~WrapAspell()
{ 
#ifdef USE_ENCHANT
	delete spell_checker;
#else
	delete_aspell_speller ( spell_checker ); 
	delete_aspell_config ( spell_config );
#endif
}

bool WrapAspell::checkWord ( const std::string &s )
{
#ifdef USE_ENCHANT
	return spell_checker->check(s);
#else
	return checkWord ( (char *) s.c_str(), s.size() );
#endif
}

std::string WrapAspell::getSuggestion (
    std::string &s )
{
#ifdef USE_ENCHANT
	std::vector<std::string> out_suggestions;
	spell_checker->suggest(s, out_suggestions);
	return out_suggestions.empty() ? "----" : out_suggestions[0];
#else
	const AspellWordList *suggestions = aspell_speller_suggest ( spell_checker, s.c_str(), s.size() );
	AspellStringEnumeration *elements = aspell_word_list_elements ( suggestions );
	const char *word = aspell_string_enumeration_next ( elements ); // no iteration req'd
	return (word) ? word : "----";
#endif
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

bool WrapAspell::checkWord ( const char *s, size_t len )
{
#ifdef USE_ENCHANT
	return checkWord( std::string(s, len) );
#else
	return aspell_speller_check ( spell_checker, s, len ); 
#endif
}
