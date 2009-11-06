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

#ifndef XML_ENCODING_HANDLER_H
#define XML_ENCODING_HANDLER_H

#include <string>
#include <utility>
#include "casehandler.h"

class XmlEncodingHandler
{
	public:
		static std::string get (
		    const std::string& utf8 );
		static bool setUtf8 ( std::string& utf8, bool ignoreCurrentEncoding = false );
		static bool set ( std::string& buffer, std::string& encoding );
		static bool hasDeclaration ( const std::string& utf8 );
		static std::pair<int, int> getEncodingValueLimits ( const std::string& utf8 );
};

#endif
