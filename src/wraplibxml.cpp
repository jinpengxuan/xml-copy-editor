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
#include <wx/wx.h>

WrapLibxml::WrapLibxml ( bool netAccessParameter, const std::string& catalogPathParameter )
		: netAccess ( netAccessParameter ), catalogPath ( catalogPathParameter )
{
	LIBXML_TEST_VERSION
	xmlInitializeCatalog();
}

WrapLibxml::~WrapLibxml()
{
	xsltCleanupGlobals();
	xmlCleanupParser();
	xmlCatalogCleanup();
}

bool WrapLibxml::validate ( const std::string& fileName )
{
	output = "";

	xmlCleanupParser();
	xmlParserCtxtPtr ctxt;
	xmlDocPtr docPtr;

	ctxt = xmlNewParserCtxt();
	if ( ctxt == NULL )
	{
		return false;
	}

	bool returnValue = false;

	loadCatalog();
	docPtr = xmlCtxtReadFile (
	             ctxt,
	             fileName.c_str(),
	             NULL,
	             ( netAccess ) ? XML_PARSE_DTDVALID : XML_PARSE_DTDVALID | XML_PARSE_NONET );
	if ( docPtr == NULL )
		;
	else if ( ctxt->valid == 0 )
		;
	else
		returnValue = true;

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );

	return returnValue;
}

bool WrapLibxml::validateRelaxNG (
    const std::string& schemaFileName,
    const std::string& fileName )
{
	output = "";

	xmlCleanupParser();

	xmlRelaxNGValidCtxtPtr ctxtPtr;
	xmlDocPtr docPtr;
	xmlRelaxNGParserCtxtPtr schemaParserCtxtPtr;
	xmlRelaxNGPtr schemaPtr;

	schemaParserCtxtPtr = xmlRelaxNGNewParserCtxt ( schemaFileName.c_str() );
	if ( schemaParserCtxtPtr == NULL )
		return false;

	schemaPtr = xmlRelaxNGParse ( schemaParserCtxtPtr );
	if ( schemaPtr == NULL )
	{
		xmlRelaxNGFreeParserCtxt ( schemaParserCtxtPtr );
		return false;
	}

	ctxtPtr = xmlRelaxNGNewValidCtxt ( schemaPtr );
	if ( ctxtPtr == NULL )
	{
		xmlRelaxNGFree ( schemaPtr );
		xmlRelaxNGFreeParserCtxt ( schemaParserCtxtPtr );
		return false;
	}

	docPtr = xmlReadFile ( fileName.c_str(), NULL,
	                       ( netAccess ) ? XML_PARSE_DTDLOAD : XML_PARSE_DTDLOAD | XML_PARSE_NONET );
	if ( docPtr == NULL )
	{
		xmlRelaxNGFree ( schemaPtr );
		xmlRelaxNGFreeValidCtxt ( ctxtPtr );
		return false;
	}

	int res = xmlRelaxNGValidateDoc ( ctxtPtr, docPtr );

	bool returnValue = ( res ) ? false : true;

	xmlFreeDoc ( docPtr );
	xmlRelaxNGFree ( schemaPtr );
	xmlRelaxNGFreeValidCtxt ( ctxtPtr );
	return returnValue;
}

bool WrapLibxml::validateW3CSchema (
    const std::string& schemaFileName,
    const std::string& fileName )
{
	output = "";

	xmlCleanupParser();
	xmlSchemaValidCtxtPtr ctxtPtr;
	xmlDocPtr docPtr;
	xmlSchemaParserCtxtPtr schemaParserCtxtPtr;
	xmlSchemaPtr schemaPtr;

	schemaParserCtxtPtr = xmlSchemaNewParserCtxt ( schemaFileName.c_str() );
	if ( schemaParserCtxtPtr == NULL )
		return false;

	schemaPtr = xmlSchemaParse ( schemaParserCtxtPtr );
	if ( schemaPtr == NULL )
	{
		xmlSchemaFreeParserCtxt ( schemaParserCtxtPtr );
		return false;
	}

	ctxtPtr = xmlSchemaNewValidCtxt ( schemaPtr );
	if ( ctxtPtr == NULL )
	{
		xmlSchemaFree ( schemaPtr );
		xmlSchemaFreeParserCtxt ( schemaParserCtxtPtr );
		return false;
	}

	docPtr = xmlReadFile ( fileName.c_str(), NULL,
	                       ( netAccess ) ? XML_PARSE_DTDLOAD : XML_PARSE_DTDLOAD | XML_PARSE_NONET );
	if ( docPtr == NULL )
	{
		xmlSchemaFree ( schemaPtr );
		xmlSchemaFreeValidCtxt ( ctxtPtr );
		return false;
	}

	int res = xmlSchemaValidateDoc ( ctxtPtr, docPtr );

	bool returnValue = ( res ) ? false : true;

	xmlFreeDoc ( docPtr );
	xmlSchemaFree ( schemaPtr );
	xmlSchemaFreeValidCtxt ( ctxtPtr );
	return returnValue;
}

bool WrapLibxml::parse (
    const std::string& fileName,
    bool indent,
    bool resolveEntities )
{
	output = "";

	xmlCleanupParser();
	xmlParserCtxtPtr ctxt;
	xmlDocPtr docPtr;

	ctxt = xmlNewParserCtxt();
	if ( ctxt == NULL )
	{
		return false;
	}

	loadCatalog();

	int flags = XML_PARSE_DTDLOAD;
	if ( resolveEntities )
		flags |= XML_PARSE_NOENT;
	if ( !netAccess )
		flags |= XML_PARSE_NONET;

	docPtr = xmlCtxtReadFile (
	             ctxt,
	             fileName.c_str(),
	             NULL,
	             flags );

	if ( docPtr == NULL )
	{
		xmlFreeParserCtxt ( ctxt );
		return false;
	}

	if ( indent )
		xmlKeepBlanksDefault ( 0 );

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

bool WrapLibxml::xpath ( const std::string& path, const std::string& fileName )
{
	output = "";

	xmlCleanupParser();

	xmlParserCtxtPtr ctxt;
	xmlDocPtr docPtr;

	ctxt = xmlNewParserCtxt();
	if ( ctxt == NULL )
	{
		return false;
	}

	loadCatalog();
	docPtr = xmlCtxtReadFile (
	             ctxt,
	             fileName.c_str(),
	             NULL,
	             //(netAccess) ? XML_PARSE_DTDLOAD | XML_PARSE_NOENT : XML_PARSE_DTDLOAD | XML_PARSE_NONET | XML_PARSE_NOENT
	             XML_PARSE_NOENT | XML_PARSE_NONET | XML_PARSE_NSCLEAN
	         );

	if ( docPtr == NULL )
	{
		xmlFreeParserCtxt ( ctxt );
		return false;
	}

	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	xmlNodeSetPtr nodeset;

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

	result = xmlXPathEvalExpression ( ( const xmlChar * ) path.c_str(), context );

	xmlKeepBlanksDefault ( 0 );

	bool xpathIsValid = ( result ) ? true : false;

	while ( result != NULL )
	{
		if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
			break;
		nodeset = result->nodesetval;
		for ( int i = 0; i < nodeset->nodeNr; i++ )
		{
			xmlNodePtr node = nodeset->nodeTab[i];
			xmlBufferPtr bufferPtr = xmlBufferCreate();
			if ( !node || !bufferPtr )
				break;
			xmlNodeDump ( bufferPtr, NULL, node, 0, 1 );
			std::string nodeBuffer = ( const char * ) xmlBufferContent ( bufferPtr );

			output += nodeBuffer;
			output += '\n';
		}
		break;
	}
	if ( result )
		xmlXPathFreeObject ( result );
	xmlXPathFreeContext ( context );
	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );

	xmlCleanupParser();

	return xpathIsValid;
}

bool WrapLibxml::xslt (
    const std::string& styleFileName,
    const std::string& fileName
)
{
	xmlCleanupParser();
	output = "";

	xsltStylesheetPtr cur;
	xmlDocPtr doc, res;
	xmlSubstituteEntitiesDefault ( 1 );
	xmlLoadExtDtdDefaultValue = 1;
	cur = xsltParseStylesheetFile ( ( const xmlChar * ) styleFileName.c_str() );
	if ( !cur )
	{
		return false;
	}

	loadCatalog();
	doc = xmlParseFile ( fileName.c_str() );
	if ( !doc )
	{
		xsltFreeStylesheet ( cur );
		return false;
	}

	// ensure entity warnings are treated as errors
	if ( !getLastError().empty() )
	{
		xmlFreeDoc ( doc );
		xsltFreeStylesheet ( cur );
		return false;
	}

	res = xsltApplyStylesheet ( cur, doc, NULL );
	if ( !res )
	{
		xmlFreeDoc ( doc );
		xsltFreeStylesheet ( cur );
		return false;
	}

	xmlChar *buf = NULL;
	int size;
	xmlDocDumpFormatMemoryEnc ( res, &buf, &size, "UTF-8", 1 );
	if ( buf )
	{
		output.append ( ( char * ) buf, size );
		free ( buf ); // as above?
	}

	xsltFreeStylesheet ( cur );
	xmlFreeDoc ( doc );
	xmlFreeDoc ( res );

	return true;
}

bool WrapLibxml::bufferWellFormed ( const std::string& buffer )
{
	xmlCleanupParser();

	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	if ( !ctxt )
		return false;

	loadCatalog();
	xmlDocPtr docPtr = xmlCtxtReadMemory (
	                       ctxt,
	                       buffer.c_str(),
	                       buffer.size(),
	                       "",
	                       "UTF-8",
	                       0 );
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
	xmlCleanupParser();

	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	if ( !ctxt )
		return -1;

	xmlSubstituteEntitiesDefault ( 0 );

	loadCatalog();

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
	xmlCleanupParser();

	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();
	if ( !ctxt )
		return -1;

	xmlSubstituteEntitiesDefault ( 0 );

	loadCatalog();

	xmlKeepBlanksDefault ( 1 ); // prevents single-line output

	xmlDocPtr docPtr = xmlCtxtReadFile (
	                       ctxt,
	                       fileNameSource.c_str(),
	                       "UTF-8",
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
		return -1;

	xmlFreeDoc ( docPtr );
	xmlFreeParserCtxt ( ctxt );
	return result;
}

std::string WrapLibxml::getLastError()
{
	xmlErrorPtr err = xmlGetLastError();

	if ( !err )
		return "";

	std::stringstream ss;
	ss << "Error at line ";
	ss << err->line;
	if ( err->int2 )
	{
		ss << ", column ";
		ss << err->int2;
	}
	ss << ": ";
	ss << err->message;
	return ss.str();
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

void WrapLibxml::loadCatalog()
{
#ifdef __WXMSW__
	xmlLoadCatalog ( catalogPath.c_str() );
#endif
}

std::string WrapLibxml::lookupPublicId ( const std::string& id )
{
	loadCatalog();
	std::string ret;
	char *s = ( char * ) xmlCatalogResolvePublic ( ( const xmlChar * ) id.c_str() );
	if ( !s )
		return ret;
	ret = s;
	free ( s );
	return ret;
}
