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

#include <wx/wx.h>
#include <stdexcept>
#include "xmlpromptgenerator.h"
#include "xmlencodinghandler.h"
#include "readfile.h"
#include "replace.h"
#include "pathresolver.h"
#include "threadreaper.h"

#undef XMLCALL
#include "catalogresolver.h"

// Xerces-C req'd for Schema parsing
#include "wrapxerces.h" // Declaration of toString() and XERCES_TMPLSINC definition

#include <xercesc/util/NameIdPool.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/DTD/DTDGrammar.hpp>

using namespace xercesc;

extern wxCriticalSection xmlcopyeditorCriticalSection;

DEFINE_EVENT_TYPE ( myEVT_NOTIFY_PROMPT_GENERATED );

XmlPromptGenerator::XmlPromptGenerator (
	wxEvtHandler *handler,
	const char *buffer,
	size_t bufferSize,
    const wxString& basePath,
    const wxString& auxPath,
    const char *encoding
    )
    : WrapExpat ( encoding )
    , wxThread ( wxTHREAD_JOINABLE )
    , d ( new PromptGeneratorData() )
    , mEventHandler ( handler )
    , mBuffer ( buffer, bufferSize )
    , mStopping ( false )
{
	if ( mBuffer.length() != bufferSize )
		throw std::bad_alloc();

	XML_SetUserData ( p, this );
	d->p = p;
	d->basePath = basePath;
	d->auxPath = auxPath;
	d->isRootElement = true;
	d->grammarFound = false;
	d->attributeValueCutoff = 12; // this prevents enums being stored in their thousands
	XML_SetParamEntityParsing ( p, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE );
	XML_SetElementHandler ( p, starthandler, endhandler );
	XML_SetDoctypeDeclHandler ( p, doctypedeclstarthandler, doctypedeclendhandler );
	XML_SetElementDeclHandler ( p, elementdeclhandler );
	XML_SetAttlistDeclHandler ( p, attlistdeclhandler );
	XML_SetEntityDeclHandler ( p, entitydeclhandler );
	XML_SetExternalEntityRefHandlerArg ( p, this );
	XML_SetExternalEntityRefHandler ( p, externalentityrefhandler );
	XML_SetBase ( p, d->basePath.utf8_str() );

	if ( !auxPath.empty() )
		XML_UseForeignDTD ( p, true );
}

XmlPromptGenerator::~XmlPromptGenerator()
{}

void XMLCALL XmlPromptGenerator::starthandler (
    void *data,
    const XML_Char *el,
    const XML_Char **attr )
{
	XmlPromptGenerator *pThis = ( XmlPromptGenerator * ) data;
	PromptGeneratorData *d = pThis->d.get();

	if (d->isRootElement)
	{
		d->rootElement = el;
		pThis->handleSchema ( d, el, attr ); // experimental: schema has been pre-parsed
		d->isRootElement = false;
		if ( d->grammarFound || pThis->TestDestroy() )//if ( d->elementMap.size() == 1) // must be 1 for success
		{
			return;
		}
	}

	d->push ( el );

	wxString parent ( d->getParent().c_str(), wxConvUTF8 );
	wxString element ( el, wxConvUTF8 );

	// update elementMap
	d->elementMap[parent].insert ( element );

	wxString attributeName, attributeValue;

	// update attributeMap
	for ( ; *attr; attr += 2 )
	{
		attributeName = wxString ( *attr, wxConvUTF8 );
		attributeValue = wxString ( * ( attr + 1 ), wxConvUTF8 );

		if (d->attributeMap[element][attributeName].size() < d->attributeValueCutoff)
			d->attributeMap[element][attributeName].insert ( attributeValue );
	}
}

void XMLCALL XmlPromptGenerator::endhandler ( void *data, const XML_Char *el )
{
	XmlPromptGenerator *pThis = ( XmlPromptGenerator * ) data;

	if ( pThis->TestDestroy() )
		XML_StopParser ( pThis->d->p, false );
	else
		pThis->d->pop();
}

// handlers for DOCTYPE handling

void XMLCALL XmlPromptGenerator::doctypedeclstarthandler (
    void *data,
    const XML_Char *doctypeName,
    const XML_Char *sysid,
    const XML_Char *pubid,
    int has_internal_subset )
{
	//PromptGeneratorData *d;
	//d = ( PromptGeneratorData * ) data;
}

void XMLCALL XmlPromptGenerator::doctypedeclendhandler ( void *data )
{
	XmlPromptGenerator *pThis = ( XmlPromptGenerator * ) data;
	PromptGeneratorData *d = pThis->d.get();
	if ( !d->elementMap.empty() )
	{
		d->grammarFound = true;
		XML_StopParser ( d->p, false ); // experimental
	}

	if ( pThis->TestDestroy() )
		XML_StopParser ( d->p, false );
}

void XMLCALL XmlPromptGenerator::elementdeclhandler (
    void *data,
    const XML_Char *name,
    XML_Content *model )
{
	XmlPromptGenerator *pThis = ( XmlPromptGenerator * ) data;
	PromptGeneratorData *d = pThis->d.get();

	wxString myElement ( name, wxConvUTF8 );

	pThis->getContent ( *model, d->elementStructureMap[myElement],
			d->elementMap[myElement] );

	XML_FreeContentModel ( d->p, model );

	if ( pThis->TestDestroy() )
		XML_StopParser ( d->p, false );
}

void XmlPromptGenerator::getContent (
    const XML_Content &content,
    wxString &contentModel,
    std::set<wxString> &list )
{
	if ( TestDestroy() )
		return;

	wxString name;
	switch ( content.type )
	{
	case XML_CTYPE_EMPTY:
		contentModel += _T("EMPTY");
		return;
	case XML_CTYPE_ANY:
		contentModel += _T("ANY");
		return;
	case XML_CTYPE_NAME:
		name = wxString ( content.name, wxConvUTF8 );
		list.insert ( name );
		contentModel += name;
		break;
	case XML_CTYPE_CHOICE:
	case XML_CTYPE_SEQ:
	case XML_CTYPE_MIXED:
	default:
		wxString sep;
		sep = ( content.type == XML_CTYPE_CHOICE ) ? _T("|") : _T(",");
		contentModel += ( content.type == XML_CTYPE_MIXED ) ? _T("(#PCDATA|") : _T("(");
		for ( unsigned i = 0; i < content.numchildren; i++ )
		{
			if ( i > 0 )
				contentModel += sep;
			getContent ( content.children[i], contentModel, list);
		}
		contentModel += _T(")");
		break;
	}

	switch ( content.quant )
	{
	case XML_CQUANT_OPT:
		contentModel += _T("?");
		break;
	case XML_CQUANT_REP:
		contentModel += _T("*");
		break;
	case XML_CQUANT_PLUS:
		contentModel += _T("+");
		break;
	case XML_CQUANT_NONE:
	default:
		break;
	}
}

void XMLCALL XmlPromptGenerator::attlistdeclhandler (
    void *data,
    const XML_Char *elname,
    const XML_Char *attname,
    const XML_Char *att_type,
    const XML_Char *dflt,
    int isrequired )
{
	XmlPromptGenerator *pThis = ( XmlPromptGenerator * ) data;
	PromptGeneratorData *d = pThis->d.get();

	wxString element ( elname, wxConvUTF8 );
	wxString attribute ( attname, wxConvUTF8 );
	std::set<wxString> &attributeValues = d->attributeMap[element][attribute];
	if ( *att_type == '(' ) // change to exclude _known_ identifiers?
	{
		const char *s, *word;
		s = att_type;

		do {
			s++;
			word = s;
			while ( *s != '|' && *s != ')' )
				s++;

			wxString currentValue ( word, wxConvUTF8, s - word );
			attributeValues.insert ( currentValue );

			while ( *s != '|' && *s != ')')
				s++;
		} while ( *s != ')' );
	}

	if ( isrequired )
	{
		d->requiredAttributeMap[element].insert ( attribute );
	}

	if ( pThis->TestDestroy() )
		XML_StopParser ( d->p, false );
}

int XMLCALL XmlPromptGenerator::externalentityrefhandler (
    XML_Parser p,
    const XML_Char *context,
    const XML_Char *base,
    const XML_Char *systemId,
    const XML_Char *publicId )
{
	// arg is set to user data in c'tor
	XmlPromptGenerator *pThis = ( XmlPromptGenerator * ) p;
	PromptGeneratorData *d = pThis->d.get();

	// Either EXPAT or Xerces-C++ is capable of parsing DTDs. The advantage
	// of Xerces-C++ is that it can access DTDs that are on the internet.
#if !PREFER_EXPAT_TO_XERCES_C

	XercesDOMParser parser;
	parser.setDoNamespaces ( true );
	parser.setDoSchema ( true );
	parser.setValidationSchemaFullChecking ( true );

	MySAX2Handler handler;
	XercesCatalogResolver catalogResolver;
	parser.setErrorHandler ( &handler );
	parser.setEntityResolver ( &catalogResolver );

	Grammar *rootGrammar;
	try {
		wxString wideSystemId = wxString::FromUTF8 ( systemId );
		std::auto_ptr<InputSource> source ( WrapXerces::resolveEntity
				( wxString::FromUTF8 ( publicId )
				, wideSystemId
				, d->basePath
				) );
		if ( !source.get() )
		{
			wxLogError ( _T("Cann't open '%s'"), wideSystemId.c_str() );
			return XML_STATUS_ERROR;
		}

		if ( pThis->TestDestroy() )
			return XML_STATUS_ERROR;

		rootGrammar = parser.loadGrammar ( *source, Grammar::DTDGrammarType );
		if ( !rootGrammar )
			return XML_STATUS_ERROR;
	}
	catch ( SAXParseException& e )
	{
		wxLogError ( _T ( "%s" ), handler.getErrors().c_str() );
		return XML_STATUS_ERROR;
	}

	DTDGrammar* grammar = ( DTDGrammar* ) rootGrammar;
	NameIdPoolEnumerator<DTDElementDecl> elemEnum = grammar->getElemEnumerator();

	SubstitutionMap substitutions;
	while ( elemEnum.hasMoreElements() && !pThis->TestDestroy() )
	{
		const DTDElementDecl& curElem = elemEnum.nextElement();
		pThis->buildElementPrompt ( d, &curElem, substitutions );
	}

	NameIdPoolEnumerator<DTDEntityDecl>
			entityEnum = grammar->getEntityEnumerator();
	while ( entityEnum.hasMoreElements() && !pThis->TestDestroy() )
	{
		const DTDEntityDecl &entity = entityEnum.nextElement();
		d->entitySet.insert ( WrapXerces::toString ( entity.getName() ) );
	}

	return pThis->TestDestroy() ? XML_STATUS_ERROR : XML_STATUS_OK;

#else // !PREFER_EXPAT_TO_XERCES_C

	int ret;
	std::string buffer;

	// auxPath req'd?
	if ( !systemId && !publicId )
	{
		ReadFile::run ( ( const char * ) d->auxPath.mb_str(), buffer );
		if ( buffer.empty() )
		{
			return XML_STATUS_ERROR;
		}

		d->encoding = XmlEncodingHandler::get ( buffer );
		XML_Parser dtdParser = XML_ExternalEntityParserCreate ( d->p, context, d->encoding.c_str() );
		if ( pThis->TestDestroy() || !dtdParser )
			return XML_STATUS_ERROR;
		XML_SetBase ( dtdParser, d->auxPath.utf8_str() );
		ret = XML_Parse ( dtdParser, buffer.c_str(), buffer.size(), true );
		XML_ParserFree ( dtdParser );
		return ret;
	}

	wxString widePublicId ( publicId, wxConvUTF8 );
	wxString wideSystemId ( systemId, wxConvUTF8 );
	CatalogResolver cr;
	wideSystemId = cr.catalogResolve ( widePublicId, wideSystemId );

	if ( wideSystemId.empty() )
	{
		if ( systemId )
			wideSystemId = wxString ( systemId, wxConvUTF8 );
		if ( base )
		{
			wxString test = PathResolver::run ( wideSystemId,
					wxString ( base, wxConvUTF8 ) );
			if ( !test.empty() )
			{
				wideSystemId = test;
			}
		}
	}

	std::string localName;
	localName = wideSystemId.mb_str ( wxConvLocal );
	if ( !localName.empty() )
	{
		ReadFile::run ( localName, buffer );
	}

	std::string encoding = XmlEncodingHandler::get ( buffer );
	XML_Parser dtdParser = XML_ExternalEntityParserCreate ( d->p, context, encoding.c_str() );
	if ( pThis->TestDestroy() || !dtdParser )
		return XML_STATUS_ERROR;

	XML_SetBase ( dtdParser, wideSystemId.utf8_str() );

	ret = XML_Parse ( dtdParser, buffer.c_str(), buffer.size(), true );
	XML_ParserFree ( dtdParser );

	return pThis->TestDestroy() ? XML_STATUS_ERROR : ret;

#endif // PREFER_EXPAT_TO_XERCES_C
}

void XMLCALL XmlPromptGenerator::entitydeclhandler (
    void *data,
    const XML_Char *entityName,
    int is_parameter_entity,
    const XML_Char *value,
    int value_length,
    const XML_Char *base,
    const XML_Char *systemId,
    const XML_Char *publicId,
    const XML_Char *notationName )
{
	// arg is set to user data in c'tor
	XmlPromptGenerator *pThis = ( XmlPromptGenerator * ) data;
	PromptGeneratorData *d = pThis->d.get();

	if (
	    entityName &&
	    !is_parameter_entity &&
	    !systemId &&
	    !publicId &&
	    !notationName )
	{
		d->entitySet.insert ( wxString ( entityName, wxConvUTF8 ) );
	}

	if ( pThis->TestDestroy() )
		XML_StopParser ( d->p, false );
}

void XmlPromptGenerator::handleSchema (
    PromptGeneratorData *d,
    const XML_Char *el,
    const XML_Char **attr )
{
	if ( !d->isRootElement )
		return;
	// first check for XML Schema association
	const char **schemaAttr = ( const char ** ) attr; // now redundant; could use attr
	wxString path;
	for ( ; *schemaAttr; schemaAttr += 2 )
	{
		// no namespace
		if ( !strcmp ( *schemaAttr, "xsi:noNamespaceSchemaLocation" ) )
		{
			path = wxString ( schemaAttr[1], wxConvUTF8 );
			break;
		}
		// with namespace -- check if this works
		else if ( !strcmp ( *schemaAttr, "xsi:schemaLocation" ) )
		{
			const char *searchIterator = * ( schemaAttr + 1 );
			while ( *searchIterator && *searchIterator != ' ' && *searchIterator != '\t' && *searchIterator != '\n' )
				searchIterator++;
			if ( *searchIterator )
			{
				path = wxString ( searchIterator + 1, wxConvUTF8 );
				break;
			}
		}
	}

	if ( path.empty() )
	{
		return;
	}


	wxString schemaPath = PathResolver::run ( path, ( d->auxPath.empty() ) ? d->basePath : d->auxPath);

	std::auto_ptr<XercesDOMParser> parser ( new XercesDOMParser() );
	parser->setDoNamespaces ( true );
	parser->setDoSchema ( true );
	parser->setValidationSchemaFullChecking ( true );

	if ( TestDestroy() )
	{
		XML_StopParser ( d->p, false );
		return;
	}

	Grammar *rootGrammar = parser->loadGrammar
			( ( const XMLCh * ) WrapXerces::toString ( schemaPath ).GetData()
			, Grammar::SchemaGrammarType
			);
	if ( !rootGrammar )
	{
		return;
	}

	SchemaGrammar* grammar = ( SchemaGrammar* ) rootGrammar;
	RefHash3KeysIdPoolEnumerator<SchemaElementDecl> elemEnum = grammar->getElemEnumerator();

	if ( !elemEnum.hasMoreElements() )
	{
		return;
	}

	SubstitutionMap substitutions;
	buildSubstitutionMap ( substitutions, *grammar );

	while ( elemEnum.hasMoreElements() && !TestDestroy() )
	{
		const SchemaElementDecl& curElem = elemEnum.nextElement();
		buildElementPrompt ( d, &curElem, substitutions );
	}

	d->grammarFound = true;
	XML_StopParser ( d->p, false );
}

void XmlPromptGenerator::buildElementPrompt (
    PromptGeneratorData *d,
    const XMLElementDecl *element,
    SubstitutionMap &substitutions )
{

	wxString name;

	const QName *qnm = element->getElementName();
	if ( qnm == NULL )
		return;
	name = WrapXerces::toString ( qnm->getRawName() ); // this includes any prefix:localname combinations
	if ( name.empty() )
		return;

	const XMLCh* fmtCntModel = element->getFormattedContentModel();
	if ( fmtCntModel != NULL ) // tbd: this does not yet pick up prefix:localname combinations
	{
		wxString structure = WrapXerces::toString ( fmtCntModel );
		d->elementStructureMap[name] = structure;
	}
	const ContentSpecNode *spec = element->getContentSpec();
	if ( spec != NULL )
	{
		getContent ( d->elementMap[name], spec, substitutions );
	}

	// fetch attributes
	if ( !element->hasAttDefs() )
		return;

	XMLAttDefList& attIter = element->getAttDefList();
	unsigned int i = 0;
	for ( ; i < attIter.getAttDefCount() && !TestDestroy(); i++ )
	{
		wxString attribute, attributeValue;

		XMLAttDef& attr = attIter.getAttDef ( i );
		XMLAttDef::DefAttTypes ty = attr.getDefaultType();
		if ( ty == XMLAttDef::Prohibited )
			continue;
		attribute = WrapXerces::toString ( attr.getFullName() );
		if ( attribute.empty() )
			continue;

		// Value
		attributeValue = WrapXerces::toString ( attr.getValue() );
		d->attributeMap[name][attribute].insert( attributeValue );

		if ( ty == XMLAttDef::Required || ty == XMLAttDef::Required_And_Fixed)
			d->requiredAttributeMap[name].insert ( attribute );
	}
}

void XmlPromptGenerator::buildSubstitutionMap (
    SubstitutionMap &substitutions,
    const SchemaGrammar &grammar )
{
	substitutions.clear();

	RefHash2KeysTableOfEnumerator<ElemVector> list ( grammar.getValidSubstitutionGroups() );
	if ( !list.hasMoreElements() )
		return;

	while ( list.hasMoreElements() && !TestDestroy() )
	{
		const ElemVector &elmts = list.nextElement();

		const QName *qnm;
		const SchemaElementDecl *cur, *substitution;
		substitution = elmts.elementAt ( 0 )->getSubstitutionGroupElem();

		size_t index = elmts.size();
		while ( index-- > 0 )
		{
			cur = elmts.elementAt ( index );
			qnm = cur->getElementName();
			wxString element = WrapXerces::toString ( qnm->getRawName() );

			substitutions[substitution].insert ( element );
		}
	}
}

void XmlPromptGenerator::getContent (
    std::set<wxString> &list,
    const ContentSpecNode *spec,
    SubstitutionMap &substitutions )
{
	if ( TestDestroy() )
		return;
	//if ( spec == NULL) return;

	const QName *qnm = spec->getElement();
	if ( qnm )
	{
		const SchemaElementDecl *elem = (const SchemaElementDecl *)spec->getElementDecl();
		SubstitutionMap::const_iterator itr = substitutions.find ( elem );
		if ( itr == substitutions.end() && elem != NULL )
			itr = substitutions.find ( elem->getSubstitutionGroupElem() );
		if ( itr != substitutions.end() )
		{
			list.insert ( itr->second.begin(), itr->second.end() );
		}
		else
		{
			wxString element = WrapXerces::toString ( qnm->getRawName() );
			if ( !element.IsEmpty() )
				list.insert( element );
		}
	}

	if ( spec->getFirst() != NULL)
		getContent( list, spec->getFirst(), substitutions );
	if ( spec->getSecond() != NULL)
		getContent( list, spec->getSecond(), substitutions );
}

void *XmlPromptGenerator::Entry()
{
	if ( TestDestroy() )
		return NULL;

	parse ( mBuffer );

	wxCriticalSectionLocker locker ( xmlcopyeditorCriticalSection );

	if ( !TestDestroy() )
	{
		wxNotifyEvent event ( myEVT_NOTIFY_PROMPT_GENERATED );
		wxPostEvent ( mEventHandler, event );
	}

	return NULL;
}

void XmlPromptGenerator::PendingDelete ()
{
	Cancel();

	ThreadReaper::get().add ( this );
}
