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

#include "wrapxerces.h"

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <sstream>
#include <utility>
#include <stdexcept>

using namespace xercesc;

void WrapXerces::Init() throw()
{
	static class Initializer
	{
	public:
		Initializer()
		{
			XMLPlatformUtils::Initialize();
		}
		~Initializer()
		{
			XMLPlatformUtils::Terminate();
		}
	} dummy;
}

WrapXerces::WrapXerces( std::string catalogPath, std::string catalogUtilityPath )
{
	WrapXerces::Init();

	errorPosition = std::make_pair ( 1, 1 );
	catalogResolver = new XercesCatalogResolver( catalogPath, catalogUtilityPath );
}

WrapXerces::~WrapXerces()
{
	delete catalogResolver;
}

bool WrapXerces::validate ( const std::string& fileName )
{
	SAX2XMLReader *parser = XMLReaderFactory::createXMLReader();

	parser->setFeature ( XMLUni::fgSAX2CoreNameSpaces, true );
	parser->setFeature ( XMLUni::fgSAX2CoreValidation, true );
	parser->setFeature ( XMLUni::fgXercesDynamic, false );
	parser->setFeature ( XMLUni::fgXercesSchema, true );
	parser->setFeature ( XMLUni::fgXercesSchemaFullChecking, true );
	parser->setFeature ( XMLUni::fgXercesValidationErrorAsFatal, true );
	parser->setFeature ( XMLUni::fgXercesLoadExternalDTD, true );


	DefaultHandler handler;
	MySAX2Handler mySAX2Handler;
	parser->setContentHandler ( &handler );
	parser->setErrorHandler ( &mySAX2Handler );

	//DefaultHandler handler;
	//parser->setEntityResolver ( &handler );
	parser->setEntityResolver ( catalogResolver );

	try
	{
		parser->parse ( fileName.c_str() );
	}
	catch ( XMLException& e )
	{
		delete parser;
		lastError = toString ( e.getMessage() );
		return false;
	}
	catch ( SAXParseException& e )
	{
		delete parser;
		lastError << _T ( "Validation stopped at line " )
		    << e.getLineNumber() << _T ( ", column " ) << e.getColumnNumber()
		    << _T ( ": " ) << toString ( e.getMessage() );
		errorPosition = std::make_pair ( e.getLineNumber(), e.getColumnNumber() );
		return false;
	}
	catch ( ... )
	{
		delete parser;
		lastError = _T ( "Unexpected validation error" );
		return false;
	}
	delete parser;
	return true;
}

// tbd: cache grammar
bool WrapXerces::validateMemory (
	const char *buffer,
	size_t len,
	const char *system,
	wxThread *thread /*= NULL*/ )
{
	std::auto_ptr<SAX2XMLReader> parser ( XMLReaderFactory::createXMLReader() );

	parser->setFeature ( XMLUni::fgSAX2CoreNameSpaces, true );
	parser->setFeature ( XMLUni::fgSAX2CoreValidation, true );
	parser->setFeature ( XMLUni::fgXercesDynamic, true );
	parser->setFeature ( XMLUni::fgXercesSchema, true );
	//parser->setFeature ( XMLUni::fgXercesSchemaFullChecking, true );
	parser->setFeature ( XMLUni::fgXercesValidationErrorAsFatal, true );
	parser->setFeature ( XMLUni::fgXercesLoadExternalDTD, true );

	DefaultHandler handler;
	MySAX2Handler mySAX2Handler;
	parser->setContentHandler ( &handler );
	parser->setErrorHandler ( &mySAX2Handler );
	//parser->setEntityResolver ( &handler );
	parser->setEntityResolver ( catalogResolver );

	XMLByte* xmlBuffer = (XMLByte*) buffer;
	MemBufInputSource source ( xmlBuffer, len, system );

	try
	{
		if ( thread == NULL )
		{
			parser->parse ( source );
		}
		else if ( !thread->TestDestroy() )
		{
			XMLPScanToken token;
			if ( parser->parseFirst ( source, token ) )
				while ( (!thread->TestDestroy()) && parser->parseNext ( token ) )
					continue;
		}
	}
	catch ( XMLException& e )
	{
		lastError = wxEmptyString;
		return false;
	}
	catch ( SAXParseException& e )
	{
		lastError << _T ( "Ln " ) << e.getLineNumber() << _T ( " Col " )
		    << e.getColumnNumber() << _T ( ": " ) << toString ( e.getMessage() );
		errorPosition = std::make_pair ( e.getLineNumber(), e.getColumnNumber() );
		return false;
	}
	catch ( ... )
	{
		lastError = wxEmptyString;
		return false;
	}
	return true;
}

const wxString &WrapXerces::getLastError()
{
	int i = lastError.Find( _T ( "Message:" ) );
	if ( i != wxNOT_FOUND )
	{
		lastError = lastError.substr( i );
	}

	return lastError;
}

std::pair<int, int> WrapXerces::getErrorPosition()
{
	return errorPosition;
}

wxString WrapXerces::toString ( const XMLCh *str )
{
	static wxMBConvUTF16 conv;

	return wxString ( ( const char * ) str, conv );
}
