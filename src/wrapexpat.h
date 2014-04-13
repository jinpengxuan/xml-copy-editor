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

#ifndef WRAP_EXPAT
#define WRAP_EXPAT

#include <wx/wx.h>
#include <expat.h>
#include <string>
#include <vector>
#include <utility>
#include "parserdata.h"

using namespace std;

class WrapExpat
{
	public:
		WrapExpat ( const char *encoding = NULL, bool nameSpaceAware = false );
		virtual ~WrapExpat();
		bool parse ( const string &buffer, bool isFinal = true );
		bool parse ( const char *buffer, size_t size, bool isFinal = true );
		pair<int, int> getErrorPosition();
		wxString getLastError();
		bool isEncodingError();
		static string xmliseTextNode ( const string &textnode );
		static string xmliseAttribute ( const string &attribute );
		static bool isWhitespace ( const string &textnode );
	protected:
		XML_Parser p;
};

#endif
