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
#include "contexthandler.h"
#include "aspell.h"

class WrapAspell
{
	public:
		WrapAspell (
                   std::string lang,// = "en_US",
#ifdef __WXMSW__
                   const std::string& aspellDataPathParameter,
                   const std::string& aspellDictPath
#endif
                   );
		~WrapAspell();
		inline bool checkWord ( std::string &s );
		void checkString (
		    std::string &s,
		    std::vector<ContextMatch> &v,
		    int contextRange );
		std::string getSuggestion ( std::string &s );
		std::string getVersion();
	private:
		AspellConfig *spell_config;
		AspellSpeller *spell_checker;
		bool checkWord ( char *s, size_t len );
};
#endif
