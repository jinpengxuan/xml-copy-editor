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

#ifndef XML_SCHEMA_LOCATOR_H
#define XML_SCHEMA_LOCATOR_H

#include <expat.h>
#include <string>
#include <memory>
#include "wrapexpat.h"

struct SchemaLocatorData
{
	std::string schemaLocation;
	XML_Parser parser;
};

class XmlSchemaLocator : public WrapExpat
{
	public:
		XmlSchemaLocator ( const char *encoding );
		virtual ~XmlSchemaLocator();
		std::string getSchemaLocation();
	private:
		std::auto_ptr<SchemaLocatorData> d;
		static void XMLCALL starthandler (
		    void *data,
		    const XML_Char *el,
		    const XML_Char **attr );
};

#endif
