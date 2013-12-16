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

#include "wraplibxml.h"
#include <sstream>
#include <stdexcept>

#ifdef ATTRIBUTE_PRINTF
#undef ATTRIBUTE_PRINTF
#endif

#include <wx/wx.h>
#include <wx/filesys.h>
#include <wx/uri.h>

#ifdef __WXMSW__ // Libxml supports utf8 file name on windows
#define CONV(url) ( ( const char * ) ( url ).utf8_str() )
#else
#define CONV(url) ( ( const char * ) ( url ).mb_str ( wxConvLocal ) )
#endif


static xmlCatalogPtr catalog = NULL;

class Initializer
{
public:
	Initializer ( const wxString &catalogPath ) throw ()
	{
		xmlSetGenericErrorFunc ( xmlGenericErrorContext,
				&Initializer::OnXmlGenericError );

		LIBXML_TEST_VERSION

		xmlInitializeCatalog();
		xmlLoadCatalog ( catalogPath.mb_str() );
		::catalog = xmlLoadACatalog ( catalogPath.mb_str() );

		initGenericErrorDefaultFunc ( NULL );
	}

	~Initializer ()
	{
		xmlFreeCatalog ( ::catalog );
		::catalog = NULL;

		xsltCleanupGlobals();
		xmlCatalogCleanup();
		xmlCleanupParser();
	}

	static void XMLCDECL OnXmlGenericError (void *ctx, const char *msg, ...) throw()
	{
		va_list args;

		size_t size = 128;
		std::string buffer;
		int chars;
		for (;;)
		{
			buffer.resize ( size );
			if ( buffer.size() < size )
				throw std::runtime_error ( "Out of memory" );

			va_start(args, msg);
			chars = vsnprintf( (char *) buffer.c_str(), size, msg, args);
			va_end(args);

			if ( chars >= 0 && ( size_t ) chars < size )
			{
				buffer.resize ( chars );
				throw std::runtime_error ( buffer );
			}
			if ( chars >= 0 )
				size = chars + 1;
			else
				throw std::runtime_error (
					std::string ( "Can't format message: " ) + msg );
		}
	}
};

void WrapLibxml::Init ( const wxString &catalogPath ) throw()
{
	static Initializer dummy ( catalogPath );
}

WrapLibxml::WrapLibxml ( bool netAccessParameter )
		: netAccess ( netAccessParameter )
{
	WrapLibxml::Init();
}

WrapLibxml::~WrapLibxml()
{
}

bool WrapLibxml::validate ( const std::string& utf8DocBuf,
		const wxString &docUrl )
{
	output = "";

	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr docPtr = NULL;

	ctxt = xmlNewParserCtxt();
	if ( ctxt == NULL )
	{
		return false;
	}

	int flags = XML_PARSE_DTDVALID;
	if ( !netAccess )
		flags |= XML_PARSE_NONET;
	docPtr = xmlCtxtReadMemory ( ctxt, utf8DocBuf.c_str(), utf8DocBuf.length(),
			CONV ( docUrl ), "UTF-8", flags);

	bool returnValue = docPtr != NULL && ctxt->valid != 0;

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );

	return returnValue;
}

bool WrapLibxml::validateRelaxNG (
    const wxString &schemaFileName,
    const std::string &utf8DocBuf,
    const wxString &docUrl )
{
	output = "";

	bool returnValue = false;
	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr docPtr = NULL;
	xmlRelaxNGValidCtxtPtr ctxtPtr = NULL;
	xmlRelaxNGParserCtxtPtr rngParserCtxt = NULL;
	xmlRelaxNGPtr schemaPtr = NULL;

	do {
		rngParserCtxt = xmlRelaxNGNewParserCtxt ( CONV ( schemaFileName ) );
		if ( rngParserCtxt == NULL )
			return false;

		schemaPtr = xmlRelaxNGParse ( rngParserCtxt );
		if ( schemaPtr == NULL )
			break;

		ctxtPtr = xmlRelaxNGNewValidCtxt ( schemaPtr );
		if ( ctxtPtr == NULL )
			break;

		ctxt = xmlNewParserCtxt();
		if ( ctxt == NULL )
			break;

		int flags = XML_PARSE_DTDVALID;
		if ( !netAccess )
			flags |= XML_PARSE_NONET;
		docPtr = xmlCtxtReadMemory ( ctxt, utf8DocBuf.c_str(),
					utf8DocBuf.length(), CONV ( docUrl ), "UTF-8", flags );
		if ( docPtr == NULL )
			break;

		int err = xmlRelaxNGValidateDoc ( ctxtPtr, docPtr );
		returnValue = ( err ) ? false : true;

	} while ( false );

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );
	xmlRelaxNGFreeValidCtxt ( ctxtPtr );
	xmlRelaxNGFree ( schemaPtr );

	return returnValue;
}

bool WrapLibxml::validateW3CSchema (
    const wxString &schemaFileName,
    const std::string &utf8DocBuf,
    const wxString &docUrl )
{
	output = "";

	bool returnValue = false;

	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr docPtr = NULL;
	xmlSchemaValidCtxtPtr ctxtPtr = NULL;
	xmlSchemaParserCtxtPtr rngParserCtxt = NULL;
	xmlSchemaPtr schemaPtr = NULL;

	do {
		rngParserCtxt = xmlSchemaNewParserCtxt ( CONV ( schemaFileName ) );
		if ( rngParserCtxt == NULL )
			return false;

		schemaPtr = xmlSchemaParse ( rngParserCtxt );
		if ( schemaPtr == NULL )
			break;

		ctxtPtr = xmlSchemaNewValidCtxt ( schemaPtr );
		if ( ctxtPtr == NULL )
			break;

		ctxt = xmlNewParserCtxt();
		if ( ctxt == NULL )
			break;

		int flags = XML_PARSE_DTDLOAD;
		if ( !netAccess )
			flags |= XML_PARSE_NONET;
		docPtr = xmlCtxtReadMemory ( ctxt, utf8DocBuf.c_str(),
					utf8DocBuf.length(), CONV ( docUrl ), "UTF-8", flags );
		if ( docPtr == NULL )
			break;

		int res = xmlSchemaValidateDoc ( ctxtPtr, docPtr );

		returnValue = ( res ) ? false : true;

	} while ( false );

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );
	xmlSchemaFree ( schemaPtr );
	xmlSchemaFreeValidCtxt ( ctxtPtr );

	return returnValue;
}

bool WrapLibxml::parse (
    const std::string& utf8DocBuf,
    const wxString &docUrl,
    bool indent,
    bool resolveEntities )
{
	return parse ( utf8DocBuf.c_str(), utf8DocBuf.length(), docUrl,
			indent, resolveEntities );
}

bool WrapLibxml::parse (
    const wxString &docUrl,
    bool indent,
    bool resolveEntities )
{
	return parse ( NULL, 0, docUrl, indent, resolveEntities );
}

bool WrapLibxml::parse (
    const char *utf8DocBuf,
    size_t utf8DocBufSize,
    const wxString &docUrl,
    bool indent,
    bool resolveEntities )
{
	output = "";

	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr docPtr = NULL;

	ctxt = xmlNewParserCtxt();
	if ( ctxt == NULL )
	{
		return false;
	}

	int flags = XML_PARSE_DTDLOAD;
	if ( resolveEntities )
		flags |= XML_PARSE_NOENT;
	if ( !netAccess )
		flags |= XML_PARSE_NONET;

	if ( utf8DocBuf != NULL)
		docPtr = xmlCtxtReadMemory ( ctxt, utf8DocBuf, utf8DocBufSize,
					CONV ( docUrl ), "UTF-8", flags );
	else
		docPtr = xmlCtxtReadFile ( ctxt, CONV ( docUrl ), NULL, flags );
	if ( docPtr == NULL )
	{
		xmlFreeParserCtxt ( ctxt );
		return false;
	}

	xmlKeepBlanksDefault ( indent ? 0 : 1 );

	xmlChar *buf = NULL;
	int size;

	// tbd: link output encoding to input encoding?
	xmlDocDumpFormatMemoryEnc (
	    docPtr,
	    &buf,
	    &size,
	    "UTF-8",
	    indent );

	if ( buf )
	{
		output.append ( ( const char * ) buf );
		free ( buf );
	}

	bool returnValue = ( !ctxt->errNo ) ? true : false;

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );

	return returnValue;
}

bool WrapLibxml::xpath ( const wxString &xpath, const std::string &utf8DocBuf,
	    const wxString &docUrl )
{
	output = "";

	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr docPtr = NULL;

	xmlKeepBlanksDefault ( 0 );

	ctxt = xmlNewParserCtxt();
	if ( ctxt == NULL )
	{
		return false;
	}

	docPtr = xmlCtxtReadMemory (
	             ctxt,
	             utf8DocBuf.c_str(),
	             utf8DocBuf.length(),
	             CONV ( docUrl ),
	             "UTF-8",
	             //(netAccess) ? XML_PARSE_DTDLOAD | XML_PARSE_NOENT : XML_PARSE_DTDLOAD | XML_PARSE_NONET | XML_PARSE_NOENT
	             XML_PARSE_NOENT | XML_PARSE_NONET | XML_PARSE_NSCLEAN
	         );

	if ( docPtr == NULL )
	{
		xmlFreeParserCtxt ( ctxt );
		return false;
	}

	xmlXPathContextPtr context = NULL;
	xmlXPathObjectPtr result = NULL;
	xmlNodeSetPtr nodeset = NULL;

	context = xmlXPathNewContext ( docPtr );
	if ( !context )
	{
		xmlFreeDoc ( docPtr );
		xmlFreeParserCtxt ( ctxt );
		return false;
	}

	// enable namespace prefixes
	xmlXPathRegisterNs ( context, ( xmlChar * ) "xhtml", ( xmlChar * ) "http://www.w3.org/1999/xhtml" );
	// add others as necessary!

	result = xmlXPathEvalExpression (
			// Since the encoding of the buffer is UTF-8
			( const xmlChar * ) ( const char * ) xpath.utf8_str (),
			context );

	bool xpathIsValid = ( result ) ? true : false;

	while ( result != NULL )
	{
		if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
			break;
		xmlBufferPtr bufferPtr = xmlBufferCreate();
		if ( bufferPtr == NULL )
			break;
		nodeset = result->nodesetval;
		for ( int i = 0; i < nodeset->nodeNr; i++ )
		{
			xmlNodePtr node = nodeset->nodeTab[i];
			if ( !node )
				break;
			xmlNodeDump ( bufferPtr, NULL, node, 0, 1 );

			output += ( const char * ) xmlBufferContent ( bufferPtr );
			output += '\n';
			xmlBufferEmpty ( bufferPtr );
		}
		xmlBufferFree ( bufferPtr );
		break;
	}

	xmlXPathFreeObject ( result );
	xmlXPathFreeContext ( context );
	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );

	return xpathIsValid;
}

bool WrapLibxml::xslt (
    const wxString &styleFileName,
    const std::string &utf8DocBuf,
    const wxString &docUrl
)
{
	return xslt ( styleFileName, utf8DocBuf.c_str(), utf8DocBuf.length(),
			docUrl );
}

bool WrapLibxml::xslt (
    const wxString &styleFileName,
    const wxString &docUrl
)
{
	return xslt ( styleFileName, NULL, 0, docUrl );
}

bool WrapLibxml::xslt (
    const wxString &styleFileName,
    const char *utf8DocBuf,
    size_t utf8DocBufSize,
    const wxString &docUrl
)
{
	output = "";

	bool ret = false;

	xsltStylesheetPtr cur = NULL;
	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr doc = NULL, res = NULL;

	do {
		cur = xsltParseStylesheetFile ( ( const xmlChar * )
				CONV ( styleFileName ) );
		if ( !cur )
		{
			nonParserError = "Cannot parse stylesheet";
			return false;
		}

		ctxt = xmlNewParserCtxt();
		if ( !ctxt )
		{
			nonParserError = _("Cannot create parser context");
			break;
		}

		int flags = XML_PARSE_NOENT | XML_PARSE_DTDLOAD;
		if ( !netAccess )
			flags |= XML_PARSE_NONET;
		if ( utf8DocBuf != NULL )
			doc = xmlCtxtReadMemory ( ctxt, utf8DocBuf, utf8DocBufSize,
						CONV ( docUrl ), "UTF-8", flags );
		else
			doc = xmlCtxtReadFile ( ctxt,  CONV ( docUrl ), NULL, flags );
		if ( !doc )
		{
			nonParserError = "Cannot parse file";
			break;
		}

		// ensure entity warnings are treated as errors
		if ( !getLastError().empty() )
			break;

		res = xsltApplyStylesheet ( cur, doc, NULL );
		if ( !res )
		{
			nonParserError = "Cannot apply stylesheet";
			break;
		}

		xmlChar *buf = NULL;
		int size;
		xmlDocDumpFormatMemoryEnc ( res, &buf, &size, "UTF-8", 1 );
		if ( buf )
		{
			output.append ( ( char * ) buf, size );
			xmlFree ( buf );
		}

		ret = true;

	} while ( false );

	xmlFreeDoc ( doc );
	xmlFreeParserCtxt ( ctxt );
	xmlFreeDoc ( res );
	xsltFreeStylesheet ( cur );

	return ret;
}

bool WrapLibxml::bufferWellFormed ( const std::string& buffer )
{
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	if ( !ctxt )
		return false;

	int flags = XML_PARSE_DTDLOAD;
	if ( !netAccess )
		flags |= XML_PARSE_NONET;
	xmlDocPtr docPtr = xmlCtxtReadMemory ( ctxt, buffer.c_str(), buffer.size(),
			"", "UTF-8", flags );
	bool returnValue = ( docPtr ) ? true : false;

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );

	return returnValue;
}

int WrapLibxml::saveEncoding (
    const std::string& buffer,
    const std::string& fileName,
    const std::string& encoding )
{
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	if ( !ctxt )
		return -1;

	xmlSubstituteEntitiesDefault ( 0 );

	xmlKeepBlanksDefault ( 1 ); // prevents single-line output

	xmlDocPtr docPtr = xmlCtxtReadMemory (
	                       ctxt,
	                       buffer.c_str(),
	                       buffer.size(),
	                       "",
	                       NULL,
	                       XML_PARSE_DTDLOAD | XML_PARSE_DTDVALID | XML_PARSE_PEDANTIC//XML_PARSE_NONET//XML_PARSE_DTDLOAD//0//(netAccess) ? XML_PARSE_DTDLOAD | XML_PARSE_NOENT : XML_PARSE_DTDLOAD | XML_PARSE_NONET | XML_PARSE_NOENT//0
	                   );
	if ( !docPtr )
	{
		xmlFreeParserCtxt ( ctxt );
		return -1;
	}

	int result = xmlSaveFileEnc (
	                 fileName.c_str(),
	                 docPtr,
	                 encoding.c_str() );
	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );
	return result;
}

int WrapLibxml::saveEncodingFromFile (
    const std::string& fileNameSource,
    const std::string& fileNameDestination,
    const std::string& encoding )
{
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	if ( !ctxt )
		return -1;

	xmlSubstituteEntitiesDefault ( 0 );
	xmlKeepBlanksDefault ( 1 ); // prevents single-line output

	xmlDocPtr docPtr = xmlCtxtReadFile (
	                       ctxt,
	                       fileNameSource.c_str(),
	                       NULL,
	                       ( netAccess ) ? XML_PARSE_DTDLOAD : XML_PARSE_DTDLOAD | XML_PARSE_NONET );//XML_PARSE_NONET//XML_PARSE_DTDLOAD//0//(netAccess) ? XML_PARSE_DTDLOAD | XML_PARSE_NOENT : XML_PARSE_DTDLOAD | XML_PARSE_NONET | XML_PARSE_NOENT//0
	if ( !docPtr )
	{
		xmlFreeParserCtxt ( ctxt );
		return -1;
	}

	int result = xmlSaveFileEnc (
	                 fileNameDestination.c_str(),
	                 docPtr,
	                 encoding.c_str() );

	// ensure entity warnings are treated as errors!
	if ( !getLastError().empty() )
		result = -1;

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );

	return result;
}

wxString WrapLibxml::getLastError()
{
	xmlErrorPtr err = xmlGetLastError();

	if ( !err )
		return nonParserError;

	wxString error ( err->message, wxConvLocal );
	if ( err->int2 )
		return wxString::Format ( _("Error at line %d, column %d: %s"),
				err->line, err->int2, error.c_str() );

	return wxString::Format ( _("Error at line %d: %s"),
			err->line, error.c_str() );
}

std::pair<int, int> WrapLibxml::getErrorPosition()
{
	xmlErrorPtr err = xmlGetLastError();
	if ( !err )
		return std::make_pair ( 1, 1 );

	return std::make_pair (
	           err->line,
	           err->int2 );
}

std::string WrapLibxml::getOutput()
{
	return output;
}

wxString WrapLibxml::catalogResolve
    ( const wxString &publicId
    , const wxString &systemId
    )
{
	// According to 7.1.2. Resolution of External Identifiers
	// from http://www.oasis-open.org/committees/entity/spec-2001-08-06.html,
	// our catalog may not be used if the system catalog, which is specified
	// in a delegateSystem entry, is out of date, such as the catalog for
	// resolving public ID "-//OASIS//DTD DocBook XML V5.0//EN"
	char *s = ( char * ) xmlACatalogResolve ( ::catalog,
			( const xmlChar * ) ( const char *) publicId.utf8_str(),
			( const xmlChar * ) ( const char *) systemId.utf8_str() );
	if ( s == NULL )
	{
#ifndef __WXMSW__
		s = ( char * ) xmlCatalogResolve (
				( const xmlChar * ) ( const char *) publicId.utf8_str(),
				( const xmlChar * ) ( const char *) systemId.utf8_str() );
		if ( s == NULL )
#endif
			return wxEmptyString;
	}

	wxString url ( s, wxConvUTF8 );
	xmlFree ( s );

	wxFileName file = wxFileSystem::URLToFileName ( url );
	if ( file.IsFileReadable() )
		return file.GetFullPath();

	return url;
}
