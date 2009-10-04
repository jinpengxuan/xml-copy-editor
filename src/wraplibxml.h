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

#ifndef WRAPLIBXML_H
#define WRAPLIBXML_H

#include <string>
#include <utility>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/catalog.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

class WrapLibxml
{
	public:
		WrapLibxml (
		    bool netAccessParameter = false,
		    const std::string& catalogPathParameter = "catalog" );
		virtual ~WrapLibxml();
		bool validate ( const std::string& fileName );
		bool validateRelaxNG (
		    const std::string& schemaFileName,
		    const std::string& fileName );
		bool validateW3CSchema (
		    const std::string& schemaFileName,
		    const std::string& fileName );
		bool parse (
		    const std::string& fileName,
		    bool indent = false,
		    bool resolveEntities = false );
		bool bufferWellFormed ( const std::string& buffer );
		bool xpath ( const std::string& path, const std::string& fileName );
		bool xslt ( const std::string& styleFileName, const std::string& fileName );
		std::string getLastError();
		std::pair<int, int> getErrorPosition();
		std::string getOutput();
		int saveEncoding (
		    const std::string& buffer,
		    const std::string& fileName,
		    const std::string& encoding );
		int saveEncodingFromFile (
		    const std::string& fileNameSource,
		    const std::string& fileNameDestination,
		    const std::string& encoding );
		std::string lookupPublicId ( const std::string& id );
	private:
		bool netAccess;
		std::string catalogPath, output, nonParserError;
		int errorLine;
		void loadCatalog();
};

#endif
