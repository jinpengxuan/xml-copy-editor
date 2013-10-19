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

#ifndef XML_UTF_READER_H
#define XML_UTF_READER_H

#include <vector>
#include <expat.h>
#include <string>
#include <memory>
#include "wrapexpat.h"

struct UtfData : public ParserData
{
	std::string buffer;
	std::string encoding;
};

class XmlUtf8Reader : public WrapExpat
{
	public:
		XmlUtf8Reader (
		    bool parseDeclaration = false,
		    bool expandInternalEntities = true,
		    size_t size = BUFSIZ
		);
		virtual ~XmlUtf8Reader();
		std::string getBuffer()
		{
			return d->buffer;
		}
		std::string getEncoding()
		{
			return d->encoding;
		}
	private:
		std::auto_ptr<UtfData> d;
		std::string encoding;
		static void XMLCALL xmldeclhandler (
		    void *data,
		    const XML_Char *version,
		    const XML_Char *encoding,
		    int standalone );
		static void XMLCALL defaulthandler (
		    void *data,
		    const XML_Char *s,
		    int len );
};

#endif
