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

#ifndef XML_ENCODING_SPY_H
#define XML_ENCODING_SPY_H

#include <expat.h>
#include <string>
#include <memory>
#include "wrapexpat.h"

struct EncodingData : public ParserData
{
	std::string encoding;
	XML_Parser p;
};

class XmlEncodingSpy : public WrapExpat
{
	public:
		XmlEncodingSpy ( const char *encoding = NULL );
		virtual ~XmlEncodingSpy();
		std::string getEncoding()
		{
			return d->encoding;
		}
	private:
		std::auto_ptr<EncodingData> d;
		static void XMLCALL xmldeclhandler (
		    void *data,
		    const XML_Char *version,
		    const XML_Char *encoding,
		    int standalone );
		static void XMLCALL start (
		    void *data,
		    const XML_Char *el,
		    const XML_Char **attr );
};

#endif
