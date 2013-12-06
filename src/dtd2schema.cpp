/*
 * Copyright 2013 Zane U. Ji
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

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/tokenzr.h>
#endif

#include "dtd2schema.h"

#include "wrapxerces.h"
#include "xmlschemagenerator.h" // for addIndent
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>


Dtd2Schema::Dtd2Schema()
{
}

Dtd2Schema::~Dtd2Schema()
{
}

const wxString &Dtd2Schema::convert ( const wxString &dtdFile )
{
	XercesDOMParser parser;
	MySAX2Handler errorHandler;
	DTDGrammar *grammar;
	try {
		parser.setErrorHandler ( &errorHandler );
		grammar = ( DTDGrammar * ) parser.loadGrammar
				( ( const XMLCh * ) WrapXerces::toString ( dtdFile ).GetData()
				, Grammar::DTDGrammarType
				);
	}
	catch ( const SAXParseException &e )
	{
		mErrors << wxString::Format ( _("Line %lld column %lld: %s[br]"),
				e.getLineNumber(), e.getColumnNumber(),
				WrapXerces::toString ( e.getMessage() ).c_str() );
		return mSchema;
	}

	mSchema << _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
			<< wxTextFile::GetEOL()
			<< _T("<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"");

	wxString elements;
	const size_t nIndent = 1;
	NameIdPoolEnumerator<DTDElementDecl>
		elemEnum = grammar->getElemEnumerator();
	while ( elemEnum.hasMoreElements() )
	{
		elements << convertElement ( elemEnum.nextElement(), nIndent );
	}

	const XMLCh *targetNameSpace = grammar->getTargetNamespace();
	if ( targetNameSpace != NULL && *targetNameSpace != 0 )
	{
		wxString targetNS = WrapXerces::toString ( targetNameSpace );
		if ( mTargetNameSpace.empty() )
			mTargetNameSpace = targetNS;
		else if ( targetNS != mTargetNameSpace )
			mErrors << wxString::Format (
					_("Target namespace is redefined: %s -> %s[br]"),
					targetNS.c_str(), mTargetNameSpace.c_str() );
	}
	if ( !mTargetNameSpace.empty() )
		mSchema << wxTextFile::GetEOL()
				<< _T("    targetNamespace=\"") << mTargetNameSpace << _T("\"")
				<< wxTextFile::GetEOL()
				<< _T("    xmlns=\"") << mTargetNameSpace << _T("\"");
	wxString importNS;
	std::map<wxString, wxString>::iterator itr;
	itr = mNameSpaceMap.find ( _T("xml") );
	if ( itr != mNameSpaceMap.end() && itr->second.empty() )
		itr->second = wxString::FromUTF8 ( (const char *) XML_XML_NAMESPACE );
	for ( itr = mNameSpaceMap.begin(); itr != mNameSpaceMap.end(); ++itr )
	{
		mSchema << wxTextFile::GetEOL()
				<< _T("    xmlns:") << itr->first << _T("=\"")
				<< itr->second << _T("\"");
		importNS << _T("  <xs:import namespace=\"")
				<< ( itr->second.empty() ? itr->first : itr->second )
				<< _T("\"/>")
				<< wxTextFile::GetEOL();
	}
	mSchema << _T(">")
			<< wxTextFile::GetEOL()
			<< importNS
			<< wxTextFile::GetEOL()
			<< elements;

#if 0
	NameIdPoolEnumerator<DTDEntityDecl>
		entityEnum = grammar->getEntityEnumerator();
	while ( entityEnum.hasMoreElements() )
	{
		mSchema << convertEntity ( entityEnum.nextElement(), nIndent );
	}
#endif

	NameIdPoolEnumerator<XMLNotationDecl>
		notationEnum = grammar->getNotationEnumerator();
	while ( notationEnum.hasMoreElements() )
	{
		mSchema << convertNotation ( notationEnum.nextElement(), nIndent );
	}

	mSchema << _T("</xs:schema>") << wxTextFile::GetEOL();

	return mSchema;
}

wxString Dtd2Schema::convertElement
	( const DTDElementDecl &element
	, size_t nIndent
	)
{
	wxString schema;
	wxString elementName = WrapXerces::toString ( element.getBaseName() );
	XmlSchemaGenerator::addIndent ( schema, nIndent );
	schema << _T("<xs:element name=\"") << elementName;

	// Check if it's a #PCDATA element
	const ContentSpecNode *contentSpec = element.getContentSpec();
	if ( !element.hasAttDefs() )
	{
		bool pcdata = false;
		if ( contentSpec == NULL )
			pcdata = true;
		else if ( contentSpec->getType() == ContentSpecNode::Leaf )
		{
			const QName *qname = contentSpec->getElement();
			if ( qname == NULL )
				pcdata = true;
			else
			{
				const XMLCh *name = qname->getRawName();
				pcdata = ( name == NULL || *name == 0 );
			}
		}
		if ( pcdata )
		{
			schema << _T("\" type=\"xs:string\"/>")
					<< wxTextFile::GetEOL()
					<< wxTextFile::GetEOL();
			return schema;
		}
	}

	// else Complex type
	schema << _T("\">") << wxTextFile::GetEOL();

	// Content
	XmlSchemaGenerator::addIndent ( schema, nIndent + 1 );
	schema << _T("<xs:complexType>")
			<< wxTextFile::GetEOL()
			<< convertContent ( contentSpec, nIndent + 2 );

	// Attributes
	const XMLAttDefList &attList = element.getAttDefList();
	size_t i, count;
	count = attList.getAttDefCount();
	for ( i = 0; i < count; i++ )
	{
		schema << convertAttribute (
				attList.getAttDef ( i ), nIndent + 2, elementName );
	}

	XmlSchemaGenerator::addIndent ( schema, nIndent + 1);
	schema << _T("</xs:complexType>") << wxTextFile::GetEOL();

	XmlSchemaGenerator::addIndent ( schema, nIndent );
	schema << _T("</xs:element>")
			<< wxTextFile::GetEOL()
			<< wxTextFile::GetEOL();

	return schema;
}

wxString Dtd2Schema::convertContent
	( const ContentSpecNode *content
	, size_t nIndent
	, const ContentSpecNode *parent /* = NULL */
	)
{
	wxString schema, suffix;
	if ( content == NULL )
		return schema;

	ContentSpecNode::NodeTypes type = content->getType();
	switch ( type )
	{
	case ContentSpecNode::ZeroOrOne:
		if ( parent != NULL && parent->getType() == type )
		{
			--nIndent;
			break;
		}
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:sequence minOccurs=\"0\">") << wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( suffix, nIndent );
		suffix << _T("</xs:sequence>") << wxTextFile::GetEOL();
		break;

	case ContentSpecNode::ZeroOrMore:
		if ( parent != NULL && parent->getType() == type )
		{
			--nIndent;
			break;
		}
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:sequence minOccurs=\"0\" maxOccurs=\"unbounded\">")
				<< wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( suffix, nIndent );
		suffix << _T("</xs:sequence>") << wxTextFile::GetEOL();
		break;

	case ContentSpecNode::OneOrMore:
		if ( parent != NULL && parent->getType() == type )
		{
			--nIndent;
			break;
		}
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:sequence maxOccurs=\"unbounded\">")
				<< wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( suffix, nIndent );
		suffix << _T("</xs:sequence>") << wxTextFile::GetEOL();
		break;

	case ContentSpecNode::Choice:
		if ( parent != NULL && parent->getType() == type
			&& parent->getMinOccurs() == content->getMinOccurs()
			&& parent->getMaxOccurs() == content->getMaxOccurs()
			)
		{
			--nIndent;
			break;
		}
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:choice") << convertOccurrence ( content ) << _T(">")
				<< wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( suffix, nIndent );
		suffix << _T("</xs:choice>") << wxTextFile::GetEOL();
		break;

	case ContentSpecNode::Sequence:
		if ( parent != NULL && parent->getType() == type
			&& parent->getMinOccurs() == content->getMinOccurs()
			&& parent->getMaxOccurs() == content->getMaxOccurs()
			)
		{
			--nIndent;
			break;
		}
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:sequence") << convertOccurrence ( content )
				<< _T(">") << wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( suffix, nIndent );
		suffix << _T("</xs:sequence>") << wxTextFile::GetEOL();
		break;

	case ContentSpecNode::Any:
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:any") << convertOccurrence ( content ) << _T(">")
				<< wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( suffix, nIndent );
		suffix << _T("</xs:any>") << wxTextFile::GetEOL();
		break;

	case ContentSpecNode::All:
		if ( parent != NULL && parent->getType() == type
			&& parent->getMinOccurs() == content->getMinOccurs()
			&& parent->getMaxOccurs() == content->getMaxOccurs()
			)
		{
			--nIndent;
			break;
		}
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:all") << convertOccurrence ( content ) << _T(">")
				<< wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( suffix, nIndent );
		suffix << _T("</xs:all>") << wxTextFile::GetEOL();
		break;

	default:
	{
		const QName *qnm = content->getElement();
		if ( qnm == NULL )
		{
			mErrors << _("Ignored content type: ") << type << _T("[br]");
			break;
		}

		wxString name = WrapXerces::toString ( qnm->getRawName() );
		if ( name.empty() )
		{// #PCDATA
			wxASSERT ( type == ContentSpecNode::Leaf );
			break;
		}

		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:element ref=\"") << name << _T("\"")
				<< convertOccurrence ( content )
				<< _T("/>")
				<< wxTextFile::GetEOL();

		size_t index = name.Index ( ':' );
		if ( index != wxString::npos )
		{ // Validate the name space
			wxString nameSpace = name.Left ( index );
			if ( mNameSpaceMap.find ( nameSpace ) == mNameSpaceMap.end() )
			{
				if ( nameSpace != _T("xml") )
					mErrors << wxString::Format (
						_("Unknown namespace: %s[br]"), nameSpace.c_str() );
				mNameSpaceMap [ nameSpace ]; // Initialize it.
			}
		}

		break;
	} // default:
	} // switch

	schema << convertContent ( content->getFirst(), nIndent + 1, content )
		<< convertContent ( content->getSecond(), nIndent + 1, content )
		<< suffix;

	return schema;
}

wxString Dtd2Schema::convertOccurrence ( const ContentSpecNode *content )
{
	wxString schema;
	int count = content->getMinOccurs();
	if (count < 0)
		schema << _T(" minOccurs=\"unbounded\"");
	else if ( count != 1 )
		schema << _T(" minOccurs=\"") << count << _T("\"");
	count = content->getMaxOccurs();
	if ( count < 0 )
		schema << _T(" maxOccurs=\"unbounded\"");
	else if ( count != 1 )
		schema << _T(" maxOccurs=\"") << count << _T("\"");
	return schema;
}

wxString Dtd2Schema::convertAttribute
	( const XMLAttDef &att
	, size_t nIndent
	, const wxString &element
	)
{
	wxString schema;
	if ( att.getType() == XMLAttDef::Any_Any )
	{
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:anyAttribute/>") << wxTextFile::GetEOL();
		return schema;
	}

	wxString name = WrapXerces::toString ( att.getFullName() );
	// targetNamespace?
	if ( name == _T("xmlns") )
	{
		wxString ns = WrapXerces::toString ( att.getValue() );
		if ( mTargetNameSpace.empty() )
			mTargetNameSpace = ns;
		else if ( mTargetNameSpace != ns )
			mErrors << wxString::Format (
					_("Ignored namespace of %s: %s[br]"),
					element.c_str(), ns.c_str() );
		return wxEmptyString;
	}

	// Reference?
	size_t index = name.Index ( ':' );
	if ( index != wxString::npos )
	{
		wxString ns = name.Left ( index );
		if ( ns == _T("xmlns") ) // A new name space is defined
		{
			wxString url = WrapXerces::toString ( att.getValue() );
			ns = name.substr ( index + 1 );
			// A new name space or it's been referenced in a element content
			if ( mNameSpaceMap [ ns ].empty() )
				mNameSpaceMap [ ns ] = url;
			else if ( mNameSpaceMap [ ns ] != url )
				mErrors << wxString::Format (
						_("Namespace redefined: %s -> %s[br]"),
						mNameSpaceMap [ ns ].c_str(), url.c_str() );
			return wxEmptyString;
		}
		else if ( mNameSpaceMap.find ( ns ) == mNameSpaceMap.end() )
		{
			if ( ns != _T("xml") )
				mErrors << wxString::Format (
						_("Unknown namespace: %s[br]"), ns.c_str() );
			mNameSpaceMap [ ns ]; // Initialize it
		}
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("<xs:attribute ref=\"") << name << _T("\"/>")
				<< wxTextFile::GetEOL();
		return schema;
	}

	XmlSchemaGenerator::addIndent ( schema, nIndent );
	schema << _T("<xs:attribute name=\"") << name << _T("\"");
	// Enumerations are prefixed with a type in docbook DTDs
	if ( att.getEnumeration() != NULL )
	//if ( att.getType() == XMLAttDef::Enumeration )
	{
		schema << convertAttValue ( att )
			<< _T(">")
			<< wxTextFile::GetEOL()
			<< convertAttType ( att, nIndent + 1 );
		XmlSchemaGenerator::addIndent ( schema, nIndent );
		schema << _T("</xs:attribute>") << wxTextFile::GetEOL();
	}
	else
	{
		schema << convertAttType ( att, nIndent + 1 )
				<< convertAttValue ( att )
				<< _T("/>")
				<< wxTextFile::GetEOL();
	}

	return schema;
}

wxString Dtd2Schema::convertAttType ( const XMLAttDef &att, size_t nIndent )
{
	wxString type;
	wxASSERT ( att.getType() != XMLAttDef::Any_Any );
	// Enumerations are prefixed with a type in docbook DTDs
	if ( att.getEnumeration() != NULL )
	//case XMLAttDef::Enumeration:
	{
		XmlSchemaGenerator::addIndent ( type, nIndent );
		type << _T("<xs:simpleType>") << wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( type, nIndent + 1 );
		type << _T("<xs:restriction base=\"xs:string\">")
				<< wxTextFile::GetEOL();

		wxStringTokenizer tokens (
				WrapXerces::toString ( att.getEnumeration() ), _T(" ") );
		while ( tokens.HasMoreTokens() )
		{
			XmlSchemaGenerator::addIndent ( type, nIndent + 2);
			type << _T("<xs:enumeration value=\"")
					<< tokens.GetNextToken()
					<< _T("\"/>")
					<< wxTextFile::GetEOL();
		}

		XmlSchemaGenerator::addIndent ( type, nIndent + 1 );
		type << _T("</xs:restriction>") << wxTextFile::GetEOL();
		XmlSchemaGenerator::addIndent ( type, nIndent );
		type << _T("</xs:simpleType>") << wxTextFile::GetEOL();
		return type;
	}

	switch ( att.getType() )
	{
	case XMLAttDef::CData:
		type << _T(" type=\"xs:string\"");
		break;

	case XMLAttDef::ID:
		type << _T(" type=\"xs:ID\"");
		break;

	case XMLAttDef::IDRef:
		type << _T(" type=\"xs:IDREF\"");
		break;

	case XMLAttDef::IDRefs:
		type << _T(" type=\"xs:IDREFS\"");
		break;

	case XMLAttDef::Entity:
		type << _T(" type=\"xs:ENTITY\"");
		break;

	case XMLAttDef::Entities:
		type << _T(" type=\"xs:ENTITIES\"");
		break;

	case XMLAttDef::NmToken:
		type << _T(" type=\"xs:NMTOKEN\"");
		break;

	case XMLAttDef::NmTokens:
		type << _T(" type=\"xs:NMTOKENS\"");
		break;

	case XMLAttDef::Notation:
		type << _T(" type=\"xs:NOTATION\"");
		break;

	default:
		const XMLCh *type;
		type = att.getAttTypeString( att.getType() );
		mErrors << wxString::Format (
				_("Ignored attribute \"%s\"'s type: %s[br]"),
				WrapXerces::toString ( att.getFullName() ).c_str(),
				WrapXerces::toString ( type ).c_str() );
		break;
	}

	return type;
}

wxString Dtd2Schema::convertAttValue ( const XMLAttDef &att)
{
	wxString value;
	if ( att.getValue() != NULL )
	{
		if ( att.getDefaultType() == XMLAttDef::Fixed )
			value << _T(" fixed=\"");
		else
			value << _T(" default=\"");
		value << WrapXerces::toString ( att.getValue() ) << _T("\"");
	}
	switch ( att.getDefaultType() )
	{
	case XMLAttDef::Default:
	case XMLAttDef::Implied:
	case XMLAttDef::Fixed:
		break;

	case XMLAttDef::Required:
		value << _T(" use=\"required\"");
		break;

	case XMLAttDef::Prohibited:
		value << _T(" use=\"prohibited\"");
		break;

	default:
		const XMLCh *defaultType;
		defaultType = XMLAttDef::getDefAttTypeString( att.getDefaultType() );
		mErrors << wxString::Format (
				_("Unknown default type of attribute \"%s\": %s[br]"),
				WrapXerces::toString ( att.getFullName() ).c_str(),
				WrapXerces::toString ( defaultType ).c_str() );
		break;
	}

	return value;
}

wxString Dtd2Schema::convertEntity
	( const DTDEntityDecl &entity
	, size_t nIndent
	)
{
	wxString schema;

	return schema;
}

wxString Dtd2Schema::convertNotation
	( const XMLNotationDecl &notation
	, size_t nIndent
	)
{
	wxString schema;
	XmlSchemaGenerator::addIndent ( schema, nIndent );
	schema << _T("<xs:notation name=\"")
			<< WrapXerces::toString ( notation.getName() );

	const XMLCh *id = notation.getPublicId();
	if ( id != NULL )
		schema << _T("\" public=\"")
				<< WrapXerces::toString ( id );
	id = notation.getSystemId();
	if ( id != NULL )
		schema << _T("\" system=\"")
				<< WrapXerces::toString ( id );
	schema << _T("\"/>") << wxTextFile::GetEOL() << wxTextFile::GetEOL();

	return schema;
}

void Dtd2Schema::reset()
{
	mErrors.clear();
	mSchema.clear();
	mTargetNameSpace.clear();
	mNameSpaceMap.clear();
}
