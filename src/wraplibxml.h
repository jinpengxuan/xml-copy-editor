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
#include <libxml/uri.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <wx/wx.h>
#include <wx/filename.h>

class WrapLibxml
{
	public:
		static void Init ( const wxString &catalogPath = _T ( "catalog" ) ) throw();

		WrapLibxml ( bool netAccessParameter = false );
		virtual ~WrapLibxml();
		bool validate (
		    const std::string &utf8DocBuf,
		    const wxString &docFileName );
		bool validateRelaxNG (
		    const wxString &schemaFileName,
		    const std::string &utf8DocBuf,
		    const wxString &docFileName );
		bool validateW3CSchema (
		    const wxString &schemaFileName,
		    const std::string &utf8DocBuf,
		    const wxString &docFileName );
		bool parse (
		    const std::string &utf8DocBuf,
		    const wxString &docFileName,
		    bool indent = false,
		    bool resolveEntities = false );
		bool parse (
		    const wxString &fileName,
		    bool indent = false,
		    bool resolveEntities = false );
		bool parse (
		    const char *utf8DocBuf,
		    size_t utf8DocBufSize,
		    const wxString &fileName,
		    bool indent = false,
		    bool resolveEntities = false );
		bool bufferWellFormed ( const std::string& utf8Buffer );
		bool xpath (
		    const wxString &xpath,
		    const std::string &utf8DocBuf,
		    const wxString &docFileName );
		bool xslt (
		    const wxString &styleFileName,
		    const std::string &utf8DocBuf,
		    const wxString &docFileName );
		bool xslt (
		    const wxString &styleFileName,
		    const wxString &docFileName );
		bool xslt (
		    const wxString &styleFileName,
		    const char *utf8DocBuf,
		    size_t utf8DocBufSize,
		    const wxString &docFileName );
		wxString getLastError();
		std::pair<int, int> getErrorPosition();
		std::string getOutput();
		int saveEncoding (
		    const std::string &utf8Buffer,
		    const wxString &fileNameSource,
		    const wxString &fileNameDestination,
		    wxMemoryBuffer *outputBuffer, /* Override fileNameDestination*/
		    const wxString &encoding );
		int saveEncoding (
		    const wxString &fileNameSource,
		    const wxString &fileNameDestination,
		    const wxString &encoding );
		int saveEncoding (
		    const char *utf8Buffer,
		    size_t utf8BufferSize,
		    const wxString &fileNameSource,
		    const wxString &fileNameDestination,
		    wxMemoryBuffer *outputBuffer, /* Override fileNameDestination*/
		    const wxString &encoding );
		wxString catalogResolve (
		    const wxString &publicId,
		    const wxString &systemId );

		static wxString FileNameToURL ( const wxString &fileName );
		static xmlChar *xmlFileNameToURL ( const wxString &fileName );
		static wxFileName URLToFileName ( const wxString &url );
	private:
		bool netAccess;
		std::string output;
		wxString nonParserError;
};

#endif
