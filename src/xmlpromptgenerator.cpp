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
#include <wx/filename.h>
#include <stdexcept>
#include "xmlpromptgenerator.h"
#include "xmlencodinghandler.h"
#include "readfile.h"
#include "replace.h"
#include "pathresolver.h"

#undef XMLCALL
#include "catalogresolver.h"

// Xerces-C req'd for Schema parsing
#ifdef __WXMSW__
#define XERCES_TMPLSINC
#endif

#include <xercesc/util/NameIdPool.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
//#include "wrapxerces.h"

using namespace xercesc;

XmlPromptGenerator::XmlPromptGenerator (
    const std::string& catalogPath,
    const std::string& basePath,
    const std::string& auxPath ) : d ( new PromptGeneratorData() )
{
	XML_SetUserData ( p, d.get() );
	d->p = p;
	d->catalogPath = catalogPath;
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
	XML_SetExternalEntityRefHandlerArg ( p, d.get() );
	XML_SetExternalEntityRefHandler ( p, externalentityrefhandler );
	XML_SetBase ( p, basePath.c_str() );

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
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) data;

	if (d->isRootElement)
	{
		d->rootElement = el;
		handleSchema ( d, el, attr ); // experimental: schema has been pre-parsed
		d->isRootElement = false;
		if ( ! (d->elementMap.empty() )  )//if ( d->elementMap.size() == 1) // must be 1 for success
		{
			d->grammarFound = true;
			XML_StopParser ( d->p, false );
			return;
		}
	}

	d->push ( el );

	std::string parent, element;
	parent = d->getParent();
	element = el;

	// update elementMap
	if ( d->elementMap.find ( parent ) == d->elementMap.end() )
	{
		std::set<std::string> childSet;
		childSet.insert ( element );
		d->elementMap.insert ( make_pair ( parent, childSet ) );
	}
	else
		d->elementMap[parent].insert ( element );

	std::string attributeName, attributeValue;

	// update attributeMap
	// case 1: element unknown, no attributes
	if ( ! ( *attr ) && d->attributeMap.find ( element ) == d->attributeMap.end() )
	{
		std::map<std::string, std::set<std::string> > currentAttributeMap;
		d->attributeMap.insert ( make_pair ( element, currentAttributeMap ) );
	}

	for ( ; *attr; attr += 2 )
	{
		attributeName = *attr;
		attributeValue = * ( attr + 1 );

		if (d->attributeMap[element][attributeName].size() < d->attributeValueCutoff)
			d->attributeMap[element][attributeName].insert ( attributeValue );
	}
}

void XMLCALL XmlPromptGenerator::endhandler ( void *data, const XML_Char *el )
{
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) data;
	d->pop();
}

bool XmlPromptGenerator::getGrammarFound()
{
	return d->grammarFound;
}

void XmlPromptGenerator::getAttributeMap (
    std::map<std::string, std::map<std::string, std::set<std::string> > >
    &attributeMap )
{
	attributeMap = d->attributeMap;
}

void XmlPromptGenerator::getRequiredAttributeMap (
    std::map<std::string, std::set<std::string> >& requiredAttributeMap )
{
	requiredAttributeMap = d->requiredAttributeMap;
}

void XmlPromptGenerator::getElementMap (
    std::map<std::string, std::set<std::string> > &elementMap )
{
	elementMap = d->elementMap;
}

void XmlPromptGenerator::getEntitySet (
    std::set<std::string> &entitySet )
{
	entitySet = d->entitySet;
}

void XmlPromptGenerator::getElementStructureMap (
    std::map<std::string, std::string> &elementStructureMap )
{
	elementStructureMap = d->elementStructureMap;
}

// handlers for DOCTYPE handling

void XMLCALL XmlPromptGenerator::doctypedeclstarthandler (
    void *data,
    const XML_Char *doctypeName,
    const XML_Char *sysid,
    const XML_Char *pubid,
    int has_internal_subset )
{
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) data;
}

void XMLCALL XmlPromptGenerator::doctypedeclendhandler ( void *data )
{
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) data;
	if ( !d->elementMap.empty() )
	{
		d->grammarFound = true;
		XML_StopParser ( d->p, false ); // experimental
	}
}

void XMLCALL XmlPromptGenerator::elementdeclhandler (
    void *data,
    const XML_Char *name,
    XML_Content *model )
{
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) data;

	std::string myElement = name;

	std::set<std::string> children;
	getContent ( *model, d->elementStructureMap[myElement], children );
	if ( !children.empty() )
		d->elementMap[myElement] = children;

	XML_FreeContentModel ( d->p, model );
}

void XmlPromptGenerator::getContent (
    const XML_Content &content,
    std::string &contentModel,
    std::set<std::string> &list )
{
	switch ( content.type )
	{
	case XML_CTYPE_EMPTY:
		contentModel += "EMPTY";
		return;
	case XML_CTYPE_ANY:
		contentModel += "ANY";
		return;
	case XML_CTYPE_NAME:
		list.insert ( content.name );
		contentModel += content.name;
		break;
	case XML_CTYPE_CHOICE:
	case XML_CTYPE_SEQ:
	case XML_CTYPE_MIXED:
	default:
		std::string sep;
		sep = ( content.type == XML_CTYPE_CHOICE ) ? "|" : ",";//_T("|") : _T(",");
		contentModel += ( content.type == XML_CTYPE_MIXED ) ? "(#PCDATA|" : "(";
		for ( unsigned i = 0; i < content.numchildren; i++ )
		{
			if ( i > 0 )
				contentModel += sep;
			getContent ( content.children[i], contentModel, list);
		}
		contentModel += ")";//_T(")");
		break;
	}

	switch ( content.quant )
	{
	case XML_CQUANT_OPT:
		contentModel += "?";//_T("?");
		break;
	case XML_CQUANT_REP:
		contentModel += "*";//_T("*");
		break;
	case XML_CQUANT_PLUS:
		contentModel += "+";//_T("+");
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
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) data;

	std::set<std::string> attributeValues;
	if ( *att_type == '(' ) // change to exclude _known_ identifiers?
	{
		char *s, *word;
		s = ( char * ) att_type;

		do {
			s++;
			while ( wxIsspace( *s ) )
				s++;
			word = s;
			while ( *s != '|' && *s != ')' && !wxIsspace( *s ) )
				s++;

			std::string currentValue ( word, s - word );
			attributeValues.insert ( currentValue );

			while ( *s != '|' && *s != ')')
				s++;
		} while ( *s != ')' && *s );
	}

	if ( attributeValues.empty() )
	{
		d->attributeMap[elname][attname].insert ( "" );
		return;
	}
	std::set<std::string>::iterator it;
	for ( it = attributeValues.begin(); it != attributeValues.end(); it++ )
		d->attributeMap[elname][attname].insert ( *it );

	if ( isrequired )
	{
		d->requiredAttributeMap[elname].insert ( attname );
	}
}

int XMLCALL XmlPromptGenerator::externalentityrefhandler (
    XML_Parser p,
    const XML_Char *context,
    const XML_Char *base,
    const XML_Char *systemId,
    const XML_Char *publicId )
{
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) p; // arg is set to user data in c'tor

	std::string buffer;

	// auxPath req'd?
	if ( !systemId && !publicId )
	{
		ReadFile::run ( d->auxPath, buffer );
		if ( buffer.empty() )
		{
			return false;
		}

		std::string encoding = XmlEncodingHandler::get ( buffer );
		XML_Parser dtdParser = XML_ExternalEntityParserCreate ( d->p, context, encoding.c_str() );
		if ( !dtdParser )
			return false;
		XML_SetBase ( dtdParser, d->auxPath.c_str() );
		return XML_Parse ( dtdParser, buffer.c_str(), buffer.size(), true );
	}

	std::string stdPublicId;
	if ( publicId )
		stdPublicId = publicId;

	CatalogResolver cr ( d->catalogPath );
	std::string stdSystemId = cr.lookupPublicId ( stdPublicId );
	
	if ( !stdSystemId.empty() )
	{
		Replace::run ( stdSystemId, "file://", "", false );
		Replace::run ( stdSystemId, "%20", " ", false );

#ifdef __WXMSW__
       Replace::run ( stdSystemId, "//C:/", "C:\\", false );
       Replace::run ( stdSystemId, "/C:/", "C:\\", false );
       Replace::run ( stdSystemId, "/", "\\", false );
#endif
	}
	else
	{
		if ( systemId )
			stdSystemId = systemId;
		if ( base )
		{
			std::string test = PathResolver::run ( stdSystemId, base );
			if ( !test.empty() )
			{
				stdSystemId = test;
			}
		}
	}

	if ( !stdSystemId.empty() )
	{
		ReadFile::run ( stdSystemId, buffer );
	}

	std::string encoding = XmlEncodingHandler::get ( buffer );
	XML_Parser dtdParser = XML_ExternalEntityParserCreate ( d->p, context, encoding.c_str() );
	if ( !dtdParser )
		return false;

	wxString wideName, wideDir;
	wideName = wxString ( stdSystemId.c_str(), wxConvUTF8, stdSystemId.size() );
	wxFileName fn ( wideName );
	wideDir = fn.GetPath();
	XML_SetBase ( dtdParser, wideName.mb_str ( wxConvUTF8 ) );

	return XML_Parse ( dtdParser, buffer.c_str(), buffer.size(), true );
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
	PromptGeneratorData *d;
	d = ( PromptGeneratorData * ) data; // arg is set to user data in c'tor

	if (
	    entityName &&
	    !is_parameter_entity &&
	    !systemId &&
	    !publicId &&
	    !notationName )
	{
		d->entitySet.insert ( entityName );
	}
}

void XmlPromptGenerator::handleSchema (
    PromptGeneratorData *d,
    const XML_Char *el,
    const XML_Char **attr )
{
	// first check for XML Schema association
	XML_Char **schemaAttr = ( XML_Char ** ) attr; // now redundant; could use attr
	std::string path;
	for ( ; d->isRootElement && *schemaAttr; schemaAttr += 2 )
	{
		// no namespace
		if ( !strcmp ( ( const char * ) *schemaAttr, "xsi:noNamespaceSchemaLocation" ) )
		{
			path = ( const char * ) * ( schemaAttr + 1 );
			break;
		}
		// with namespace -- check if this works
		else if ( !strcmp ( ( const char * ) *schemaAttr, "xsi:schemaLocation" ) )
		{
			char *searchIterator;
			for ( searchIterator = ( char * ) * ( schemaAttr + 1 ); *searchIterator && *searchIterator != ' ' && *searchIterator != '\t' && *searchIterator != '\n'; searchIterator++ )
				;
			if ( *searchIterator )
			{
				path = ( const char * ) ( searchIterator + 1 );
				break;
			}
		}
	}

	if ( path.empty() )
	{
		return;
	}
	

	std::string schemaPath = PathResolver::run ( path, ( d->auxPath.empty() ) ? d->basePath : d->auxPath);


	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch ( const XMLException& toCatch )
	{
		XMLPlatformUtils::Terminate();
		return;
	}

	XercesDOMParser *parser = new XercesDOMParser();
	parser->setDoNamespaces ( true );
	parser->setDoSchema ( true );
	parser->setValidationSchemaFullChecking ( true );

	Grammar *rootGrammar = parser->loadGrammar ( schemaPath.c_str(), Grammar::SchemaGrammarType );
	if ( !rootGrammar )
	{
		delete parser;
		XMLPlatformUtils::Terminate();
        return;
	}

	SchemaGrammar* grammar = ( SchemaGrammar* ) rootGrammar;
	RefHash3KeysIdPoolEnumerator<SchemaElementDecl> elemEnum = grammar->getElemEnumerator();

	if ( !elemEnum.hasMoreElements() )
	{
		delete grammar;
		delete parser;
		XMLPlatformUtils::Terminate();		
		return;
	}

	char *s;
	while ( elemEnum.hasMoreElements() )
	{
		const SchemaElementDecl& curElem = elemEnum.nextElement();

		std::string element;
		std::set<std::string> children;

		const QName *qnm = curElem.getElementName();
		if ( qnm )
		{
			s = XMLString::transcode ( qnm->getRawName() ); // this includes any prefix:localname combinations
			element = s;
			XMLString::release( &s );
		}
		if ( element.empty() )
			continue;

		const XMLCh* fmtCntModel = curElem.getFormattedContentModel();
		if ( fmtCntModel != NULL ) // tbd: this does not yet pick up prefix:localname combinations
		{
			std::string structure;
			s = XMLString::transcode ( fmtCntModel );
			structure = s;
			XMLString::release( &s );
			d->elementStructureMap.insert ( make_pair ( element, structure ) );
		}
		const ContentSpecNode *spec = curElem.getContentSpec();
		if ( spec != NULL )
		{
			getContent ( spec, children );
			if ( !children.empty() )
				d->elementMap.insert ( make_pair ( element, children ) );
		}

		// fetch attributes
		if ( curElem.hasAttDefs() && ! ( curElem.getAttDefList().isEmpty() ) )
		{
			std::map<std::string, std::set<std::string> > attributeMap;

			XMLAttDefList& attIter = curElem.getAttDefList();
			for ( unsigned int i = 0; i < attIter.getAttDefCount(); i++ )
			{
				std::string attribute, attributeValue;
				std::set<std::string> attributeValueSet;

				XMLAttDef& attr = attIter.getAttDef ( i );
				XMLAttDef::DefAttTypes ty = attr.getDefaultType();
				if ( ty == XMLAttDef::Prohibited )
					continue;
				SchemaAttDef *pAttr = ( SchemaAttDef * ) &attr;

				const QName *qnm = pAttr->getAttName();
				if ( qnm )
				{
					s = XMLString::transcode ( qnm->getRawName() );
					attribute = s;
					XMLString::release( &s );
				}
				if ( attribute.empty() )
					continue;

				// Value
				if ( pAttr->getValue() )
				{
					s = XMLString::transcode ( pAttr->getValue() );
					attributeValue = s;
					XMLString::release( &s );
					attributeValueSet.insert ( attributeValue );
				}

				attributeMap.insert ( make_pair ( attribute, attributeValueSet ) );
			}
			if ( !attributeMap.empty() )
				d->attributeMap.insert( make_pair ( element, attributeMap ) );
		}
	}
    delete parser;
	XMLPlatformUtils::Terminate();
}

void XmlPromptGenerator::getContent (
    const ContentSpecNode *spec,
    std::set<std::string> &list )
{
	//if ( spec == NULL) return;

	const QName *qnm = spec->getElement();
	if ( qnm )
	{
		char *element = XMLString::transcode ( qnm->getRawName() );
		if ( element != NULL )
			list.insert( element );
		XMLString::release( &element );
	}

	if ( spec->getFirst() != NULL)
		getContent( spec->getFirst(), list );
	if ( spec->getSecond() != NULL)
		getContent( spec->getSecond(), list );
}
