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

#ifndef XML_PARSE_SCHEMA_NS_H
#define XML_PARSE_SCHEMA_NS_H

#include <vector>
#include <expat.h>
#include <string>
#include <memory>
#include "wrapexpat.h"

struct ParseSchemaNsData : public ParserData
{
	std::vector<std::pair<std::string, std::string> > attributeVector;
	XML_Parser p;
};

class XmlParseSchemaNs : public WrapExpat
{
	public:
		XmlParseSchemaNs();
		virtual ~XmlParseSchemaNs();
		std::vector<std::pair<std::string, std::string> > getAttributeVector()
		{
			return d->attributeVector;
		}
	private:
		std::auto_ptr<ParseSchemaNsData> d;
		static void XMLCALL start (
		    void *data,
		    const XML_Char *el,
		    const XML_Char **attr );
};

#endif
