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
#include "pathresolver.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <sstream>
#include <utility>
#include <stdexcept>
#include <boost/static_assert.hpp>

using namespace xercesc;

XMLNetAccessor *WrapXerces::mOriginalNetAccessor = NULL;

void WrapXerces::Init ( bool enableNetAccess ) throw()
{
	static class Initializer
	{
	public:
		Initializer ()
		{
			XMLPlatformUtils::Initialize();
			mOriginalNetAccessor = XMLPlatformUtils::fgNetAccessor;
		}
		~Initializer()
		{
			XMLPlatformUtils::Terminate();
		}
	} dummy;

	enableNetwork ( enableNetAccess );
}

WrapXerces::WrapXerces()
{
	catalogResolver = new XercesCatalogResolver();
}

WrapXerces::~WrapXerces()
{
	delete catalogResolver;
}

// Returns true if the file is valid. But there can be warnings
bool WrapXerces::validate ( const wxString& fileName )
{
	return validateMemory ( NULL, 0, fileName );
}

// tbd: cache grammar
// Returns true if the content is valid. But there can be warnings
bool WrapXerces::validateMemory (
	const char *utf8Buffer,
	size_t len,
	const wxString &fileName,
	wxThread *thread /*= NULL*/,
	bool forceGrammarCheck /*= true*/,
	const wxChar *messageEOL /*= _T("[br]")*/)
{
#if 0 // Test DOM parser
	std::auto_ptr<XercesDOMParser> parser ( new XercesDOMParser() );

	parser->setDoNamespaces(true);
	parser->setExitOnFirstFatalError(true);
	parser->setValidationConstraintFatal(true);
	//parser->setCreateEntityReferenceNodes(true); // Default is true
	parser->setValidationScheme(XercesDOMParser::Val_Auto);
	parser->setDoSchema(true);
	parser->setValidationSchemaFullChecking(true);
	parser->setCreateCommentNodes(false);
#else
	std::auto_ptr<SAX2XMLReader> parser ( XMLReaderFactory::createXMLReader() );

	parser->setFeature ( XMLUni::fgSAX2CoreNameSpaces, true );
	parser->setFeature ( XMLUni::fgSAX2CoreValidation, true );
	parser->setFeature ( XMLUni::fgXercesDynamic, !forceGrammarCheck );
	parser->setFeature ( XMLUni::fgXercesSchema, true );
	parser->setFeature ( XMLUni::fgXercesSchemaFullChecking, true);
	parser->setFeature ( XMLUni::fgXercesValidationErrorAsFatal, true );
	parser->setFeature ( XMLUni::fgXercesLoadExternalDTD, true );

	mySAX2Handler.reset();
	parser->setContentHandler ( &mySAX2Handler );
#endif

	parser->setErrorHandler ( &mySAX2Handler );
	//parser->setEntityResolver ( &handler );
	parser->setEntityResolver ( catalogResolver );

	mySAX2Handler.setEOL ( messageEOL );

	std::auto_ptr<InputSource> source;
	if ( utf8Buffer != NULL )
	{
		source.reset ( new MemBufInputSource ( (XMLByte*) utf8Buffer, len,
				(const XMLCh *) toString ( fileName ).GetData() ) );
		wxString utf8 = _T("UTF-8");
		source->setEncoding ( (const XMLCh *) toString ( utf8 ).GetData() );
	}
	else
	{
		source.reset ( new LocalFileInputSource (
				(const XMLCh *) toString ( fileName ).GetData() ) );
	}
	try
	{
		if ( thread == NULL )
		{
			parser->parse ( *source );
		}
		else if ( !thread->TestDestroy() )
		{
			XMLPScanToken token;
			if ( parser->parseFirst ( *source, token ) )
				while ( (!thread->TestDestroy()) && parser->parseNext ( token ) )
					continue;
		}
	}
	catch ( XMLException& e )
	{
		wxString error = toString ( e.getMessage() );
		int i = error.Find( _T("Message:") );
		if ( i != wxNOT_FOUND )
			error = error.substr( i );
		mySAX2Handler.getErrors() << error;
		return false;
	}
	catch ( SAXParseException& e )
	{
		// It has already been processed in mySAX2Handler
		return false;
	}
	catch ( ... )
	{
		if ( thread != NULL && thread->TestDestroy() )
			throw;
		mySAX2Handler.getErrors() << _("Unexpected validation error");
		return false;
	}

	return true;//mySAX2Handler.getErrors().empty();
}

const wxMBConv &WrapXerces::getMBConv()
{
	switch ( sizeof ( XMLCh ) )
	{
	case 1:
		return wxConvUTF8;
	case 2:
	{
		const static wxMBConvUTF16 conv = wxMBConvUTF16();
		return conv;
	}
	case 4:
	{
		const static wxMBConvUTF32 conv = wxMBConvUTF32();
		return conv;
	}
	default:
		BOOST_STATIC_ASSERT_MSG ( sizeof ( XMLCh ) == 2
			, "Xerces-C doesn't use UTF-16 strings any more");
		break;
	}
	return wxConvUTF8;
}

wxString WrapXerces::toString ( const XMLCh *str )
{
	return wxString ( ( const char * ) str, getMBConv() );
}

wxMemoryBuffer WrapXerces::toString ( const wxString &str )
{
	const static XMLCh chNull = '\0'; // Xerces-C crashes when the file name is NULL. We'd better return something other than NULL.
	wxMemoryBuffer buffer ( 0 );
	const size_t lenWC = str.length() + 1; // Plus '\0'. This is important. Otherwise we can call wxString::mb_str(getMBConv()).
	size_t lenMB = getMBConv().FromWChar ( NULL, 0, str.c_str(), lenWC );
	if ( lenMB == wxCONV_FAILED )
	{
		buffer.AppendData ( &chNull, sizeof chNull );
		return buffer;
	}

	buffer.SetBufSize ( lenMB );
	lenMB = getMBConv().FromWChar ( ( char * ) buffer.GetData(), lenMB, str.c_str(), lenWC );
	buffer.SetDataLen ( lenMB );

	return buffer;
}

bool WrapXerces::enableNetwork ( bool enable /*= true*/ )
{
	bool ret = XMLPlatformUtils::fgNetAccessor != NULL;
	if ( enable )
	{
		wxASSERT ( mOriginalNetAccessor != NULL );
		XMLPlatformUtils::fgNetAccessor = mOriginalNetAccessor;
	}
	else
	{
		XMLPlatformUtils::fgNetAccessor = NULL;
	}
	return ret;
}

void MySAX2Handler::logError ( const wxString &type, wxLogLevel level,
		const SAXParseException& e )
{
	mErrors << wxString::Format (
			_("%s at line %llu, column %llu: %s%s"),
			type.c_str(), e.getLineNumber(), e.getColumnNumber(),
			WrapXerces::toString ( e.getMessage() ).c_str(), mEOL.c_str() );

	// Only save the first error position
	BOOST_STATIC_ASSERT ( wxLOG_Error < wxLOG_Warning );
	if ( level < mLevel	|| ( level == mLevel && mErrorPosition.first == 1
			&& mErrorPosition.second == 1 ) )
	{
		mErrorPosition.first = e.getLineNumber();
		mErrorPosition.second = e.getColumnNumber();
		mLevel = level;
	}
}

InputSource *WrapXerces::resolveEntity (
	const wxString &publicId,
	const wxString &systemId,
	const wxString &fileName
)
{
	XercesCatalogResolver cr;
	InputSource *source = cr.resolveEntity
			( ( const XMLCh * ) WrapXerces::toString ( publicId ).GetData()
			, ( const XMLCh * ) WrapXerces::toString ( systemId ).GetData()
			);
	if ( source )
		return source;

	BOOST_STATIC_ASSERT ( sizeof( xmlChar ) == sizeof ( char ) );

	// Xerces-C++ can't open a file URL when there are multi-byte characters.
	// Let's use the file name instead.
	wxString file = PathResolver::run ( systemId, fileName );
	if ( wxFileExists ( file ) )
		return new LocalFileInputSource (
				( const XMLCh * ) WrapXerces::toString ( file ).GetData() );

	wxString fileURL = WrapLibxml::FileNameToURL ( fileName );
	return new URLInputSource
		( ( const XMLCh * ) WrapXerces::toString ( fileURL ).GetData()
		, ( const XMLCh * ) WrapXerces::toString ( systemId ).GetData()
		, ( const XMLCh * ) WrapXerces::toString ( publicId ).GetData()
		);
}
