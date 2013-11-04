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

#ifndef WRAP_ASPELL_H
#define WRAP_ASPELL_H

#include <string>
#include <vector>
#include <wx/wx.h>
#include "contexthandler.h"
#if !defined(USE_ENCHANT)
#include "aspell.h"
#else
namespace enchant
{
	class Broker;
	class Dict;
}
#endif

class WrapAspell
{
	public:
		WrapAspell (
                   const wxString& lang// = "en_US",
#if !defined(USE_ENCHANT) && defined(__WXMSW__)
                   , const wxString& aspellDataPathParameter,
                   const wxString& aspellDictPath
#endif
                   );
		~WrapAspell();
		inline bool checkWord ( const std::string &s );
		void checkString (
		    std::string &s,
		    std::vector<ContextMatch> &v,
		    int contextRange );
		std::string getSuggestion ( std::string &s );
		std::string getVersion();
	private:
#ifdef USE_ENCHANT
		enchant::Broker *spell_broker;
		enchant::Dict *spell_checker;
#else
		AspellConfig *spell_config;
		AspellSpeller *spell_checker;
#endif
		bool checkWord ( const char *s, size_t len );
};
#endif
