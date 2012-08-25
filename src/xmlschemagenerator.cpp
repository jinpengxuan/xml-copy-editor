/*
 * Copyright 2012 Zane U. Ji.
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

#include "xmlschemagenerator.h"
#include "wrapxerces.h"
#include <memory>
#include <algorithm>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>

XmlSchemaGenerator::XmlSchemaGenerator ( bool inlineSimpleType /*= true*/)
		: mInlineSimpleType ( inlineSimpleType )
		, mGrammarType ( Grammar::SchemaGrammarType )
{
	WrapXerces::Init();
}

XmlSchemaGenerator::~XmlSchemaGenerator()
{
}

const wxString &XmlSchemaGenerator::generate ( Grammar::GrammarType grammarType,
		const wxString &filepath, const char *buffer, size_t len )
{
	mGrammarType = grammarType;
	mElements.clear();
	mSchema.Clear();

	std::auto_ptr<XercesDOMParser> parser ( new XercesDOMParser() );
	parser->setDoNamespaces ( true );
	parser->setDoSchema ( true );
	parser->setValidationSchemaFullChecking ( false );

	MemBufInputSource source ( ( const XMLByte * ) buffer, len,
			filepath.mb_str( wxConvLocal ) );
	try {
		//XMLPlatformUtils::fgSSE2ok = false;
		parser->parse ( source );
	}
	catch ( XMLException& e )
	{
		mLastError = WrapXerces::toString ( e.getMessage() );
		return mSchema;
	}

	DOMDocument *doc = parser->getDocument();
	if ( doc == NULL )
	{
		mLastError = _ ("Failed to load xml file.");
		return mSchema;
	}

	mSchema << _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") << getEOL();
	if ( mGrammarType == Grammar::SchemaGrammarType )
		mSchema << _T("<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" elementFormDefault=\"qualified\">")
				<< getEOL();

	DOMElement *root = doc->getDocumentElement();
	if ( root != NULL )
	{
		findAllElements ( *root );
		generateData ( *root, 1 );
		if ( mInlineSimpleType && mGrammarType == Grammar::SchemaGrammarType )
			outputSchema ( *root );
	}

	if ( mGrammarType == Grammar::SchemaGrammarType )
		mSchema << _T("</xs:schema>") << getEOL();

	return mSchema;
}

void XmlSchemaGenerator::findAllElements ( const DOMElement &element,
		size_t nIndent /*= 0*/)
{
	wxString tagName = WrapXerces::toString ( element.getTagName() );
	mElements[tagName].nodes.insert ( &element );

	DOMElement *child = element.getFirstElementChild();
	for ( ; child != NULL; child = child->getNextElementSibling() )
	{
		findAllElements ( *child, nIndent );
	}
}

void XmlSchemaGenerator::generateData ( const DOMElement &element,
		size_t nIndent /*= 0*/)
{
	wxString name = WrapXerces::toString ( element.getTagName() );
	if ( mElements[name].name.empty() )
	{ // Only generate data once
		generateData ( name, nIndent );
	}

	DOMElement *child = element.getFirstElementChild();
	for ( ; child != NULL; child = child->getNextElementSibling() )
	{
		generateData ( *child, nIndent );
	}
}

void XmlSchemaGenerator::generateData ( const wxString &elementName,
		size_t nIndent /*= 0*/)
{
	ElmtData &data = mElements[elementName];
	std::set<const DOMElement *>::iterator elmtItr;

	data.name = elementName;

	//Content
	std::map<wxString, ChildData> &childMap = data.children;
	std::map<wxString, ChildData>::iterator itr;
	std::set<wxString> previous;
	elmtItr = data.nodes.begin();
	for ( ; elmtItr != data.nodes.end(); elmtItr++ )
	{
		previous.clear();

		std::map<wxString, size_t> countMap;
		DOMElement *child = ( **elmtItr ).getFirstElementChild();
		for ( ; child != NULL; child = child->getNextElementSibling() )
		{
			wxString name = WrapXerces::toString ( child->getTagName() );
			childMap[name].prevSiblings.insert ( previous.begin(), previous.end() );
			childMap[name].prevSiblings.erase ( name ); // Don't depend on oneself
			previous.insert ( name );
			countMap[name] += 1;
		}
		std::map<wxString, size_t>::iterator countItr = countMap.begin();
		for ( ; countItr != countMap.end(); countItr++ )
		{
			if ( childMap[countItr->first].maxOccurs < countItr->second )
				childMap[countItr->first].maxOccurs = countItr->second;
		}
		if ( childMap.size() == countMap.size() )
			continue;
		for ( itr = childMap.begin(); itr != childMap.end(); itr++ )
		{
			if ( countMap.find ( itr->first ) != countMap.end() )
				continue;
			itr->second.minOccurs = 0;
		}
	}
	// Attribute
	std::map<wxString, const XMLCh *> &attrMap = data.attrMap;
	std::set<wxString> &optAttrs = data.optAttrs;
	std::map<wxString, const XMLCh *>::iterator attrItr;
	elmtItr = data.nodes.begin();
	for ( ; elmtItr != data.nodes.end(); elmtItr++ )
	{
		if ( ! ( **elmtItr ).hasAttributes() )
			continue;

		wxString name;
		DOMAttr *attr;
		DOMNamedNodeMap *attrs = ( **elmtItr ).getAttributes();
		size_t i = attrs->getLength();
		while ( i-- > 0 )
		{
			attr = ( DOMAttr* ) attrs->item ( i );
			name = WrapXerces::toString ( attr->getName() );
			if ( attr->getPrefix() != NULL )
			{
				wxLogDebug ( _T("Ignore: %s"), name.c_str() );
				continue;
			}
			if ( attr->getSpecified() )
				attrMap[name]; // Initialize attribute map
			else
				attrMap[name] = attr->getValue();
		}
		if ( attrMap.size() == optAttrs.size() )
			continue;
		for ( attrItr = attrMap.begin(); attrItr != attrMap.end(); attrItr++ )
		{
			const static wxMBConvUTF16 conv;
			if ( attrs->getNamedItem ( ( const XMLCh * ) ( const char * )
					attrItr->first.mb_str ( conv ) ) == NULL )
			{
				optAttrs.insert ( attrItr->first );
			}
		}
	}

	// Deal with sequence
	wxLogDebug ( _T("%s:"), elementName.c_str() );
	data.useSequence = getSequence ( data.sequence, childMap );

	// Now we have the data of the element
	if ( mGrammarType == Grammar::DTDGrammarType )
	{
		generateDTD ( data, nIndent );
		mSchema << data.schema;
	}
	else if ( !mInlineSimpleType )
	{ // Or wait until all data are available
		generateSchema ( data, nIndent );
		mSchema << data.schema;
	}
}

void XmlSchemaGenerator::outputSchema ( const DOMElement &element )
{
	wxString tagName = WrapXerces::toString ( element.getTagName() );
	ElmtData &data = mElements[tagName];
	if ( data.schema.empty() )
	{
		if ( mGrammarType == Grammar::SchemaGrammarType )
			generateSchema ( data, 1 );
		else
			generateDTD ( data, 1 );
		mSchema << data.schema;
	}

	DOMElement *child = element.getFirstElementChild();
	for ( ; child != NULL; child = child->getNextElementSibling() )
	{
		outputSchema ( *child );
	}
}

void XmlSchemaGenerator::generateSchema ( ElmtData &data, size_t nIndent )
{
	wxString &schema = data.schema;

	if ( data.children.size() == 0 && data.attrMap.size() == 0 )
	{
		if ( !mInlineSimpleType )
		{
			addIndent ( schema, nIndent );
			schema << _T("<xs:element name=\"") << data.name
					<< _T("\" type=\"xs:string\"/>") << getEOL();
		}
		return;
	}
	addIndent ( schema, nIndent++ );
	schema << _T("<xs:element name=\"") << data.name << _T("\">") << getEOL();
	if ( data.children.size() > 0 )
	{
		addIndent ( schema, nIndent++ );
		schema << _T("<xs:complexType>") << getEOL();
		addIndent ( schema, nIndent++ );

		size_t minOccurs = 1, maxOccurs = 1, minTotal = 0;
		std::map<wxString, ChildData>::const_iterator itr;
		for ( itr = data.children.begin(); itr != data.children.end(); itr++ )
		{
			if ( itr->second.minOccurs < minOccurs )
				minOccurs = itr->second.minOccurs;
			if ( itr->second.maxOccurs > maxOccurs )
				maxOccurs = itr->second.maxOccurs;
			minTotal += itr->second.minOccurs;
		}
		if ( data.useSequence )
		{
			schema << _T("<xs:sequence");
		}
		else
		{
			schema << _T("<xs:choice maxOccurs=\"unbounded\"");
		}
		//if (minOccurs != 1) schema << _T(" minOccurs=\"") << minOccurs << _T("\"");
		//if (maxOccurs > 1) schema << _T(" maxOccurs=\"unbounded\"");
		if ( minTotal == 0 ) schema << _T(" minOccurs=\"0\"");
		schema << _T(">") << getEOL();

		std::vector<wxString>::const_iterator seqItr;
		seqItr = data.sequence.begin();
		for ( ; seqItr != data.sequence.end(); seqItr++ )
		{
			const ChildData &child = data.children[*seqItr];
			addIndent ( schema, nIndent );
			if ( mInlineSimpleType )
			{ // Check if it's a simple type
				const ElmtData *childElmt = &mElements[*seqItr];
				if ( childElmt->children.size() == 0
						&& childElmt->attrMap.size() == 0 )
				{
					schema << _T("<xs:element name=\"") << *seqItr
							<< _T("\" type=\"xs:string\"/>") << getEOL();
					continue;
				}
			}
			schema << _T("<xs:element ref=\"") << *seqItr << _T("\"");
			if ( data.useSequence )
			{
				if ( child.minOccurs == 0 )
				{
					schema << _T(" minOccurs=\"0\"");
				}
				if ( child.maxOccurs > 1 )
				{
					schema << _T(" maxOccurs=\"unbounded\"");
				}
			}
			schema << _T("/>") << getEOL();
		}

		addIndent ( schema, --nIndent );
		if ( data.useSequence )
		{
			schema << _T("</xs:sequence>") << getEOL();
		}
		else
		{
			schema << _T("</xs:choice>") << getEOL();
		}
	}
	else if ( data.attrMap.size() != 0 )
	{
		addIndent ( schema, nIndent++ );
		schema << _T("<xs:complexType>") << getEOL();
	}
	std::map<wxString, const XMLCh *>::const_iterator attrItr;
	attrItr = data.attrMap.begin();
	for ( ; attrItr != data.attrMap.end(); attrItr++ )
	{
		addIndent ( schema, nIndent );
		schema << _T("<xs:attribute name=\"") << attrItr->first
				<< _T("\" type=\"xs:string\"");
		if ( attrItr->second != NULL )
		{
			schema << _T(" default=\"")
					<< WrapXerces::toString ( attrItr->second ) << _T("\"");
		}
		else if ( data.optAttrs.find ( attrItr->first )
				== data.optAttrs.end() )
		{
			schema << _T(" use=\"required\"");
		}
		schema << _T("/>") << getEOL();
	}

	addIndent ( schema, --nIndent );
	schema << _T("</xs:complexType>") << getEOL();
	addIndent ( schema, --nIndent );
	schema << _T("</xs:element>") << getEOL();
}

void XmlSchemaGenerator::generateDTD ( ElmtData &data, size_t WXUNUSED ( nIndent ) )
{
	wxString &schema = data.schema;

	schema << _T("<!ELEMENT ") << data.name;
	if (data.sequence.empty())
	{
		schema << _T(" (#PCDATA)");
	}
	else
	{
		const wxChar *separator = _T(" (");
		std::vector<wxString>::const_iterator seqItr;
		seqItr = data.sequence.begin();
		if (data.useSequence)
		{
			for ( ; seqItr != data.sequence.end(); seqItr++ )
			{
				schema << separator << *seqItr;
				separator = _T(", ");
				const ChildData &child = data.children[*seqItr];
				if ( child.minOccurs == 0 )
					schema << ( child.maxOccurs > 1 ? _T("*") : _T("?") );
				else if ( child.maxOccurs > 1 )
					schema << _T("+");
			}
			schema << _T(")");
		}
		else
		{
			size_t minTotal = 0;
			for ( ; seqItr != data.sequence.end(); seqItr++ )
			{
				schema << separator << *seqItr;
				separator = _T(" | ");
				minTotal += data.children[*seqItr].maxOccurs;
			}
			schema << ( minTotal > 0 ? _T(")+") : _T(")*") );
		}
	}
	schema << _T(">") << getEOL();

	if ( !data.attrMap.empty() )
	{
		const static wxString indent =
				wxString ( getEOL() ) + _T("          ");
		schema << _T("<!ATTLIST ") << data.name;

		std::map<wxString, const XMLCh *>::const_iterator attrItr;
		attrItr = data.attrMap.begin();
		for ( ; attrItr != data.attrMap.end(); attrItr++ )
		{
			schema << indent << attrItr->first << _T(" CDATA");
			if ( attrItr->second != NULL ) // Has default value
				schema << _T(" \"") << WrapXerces::toString ( attrItr->second ) << _T("\"");
			else if ( data.optAttrs.find ( attrItr->first ) == data.optAttrs.end() )
				schema << _T(" #REQUIRED");
			else
				schema << _T(" #IMPLIED");
		}
		schema << _T(">") << getEOL();
	}
}

bool XmlSchemaGenerator::getSequence ( std::vector<wxString> &sequence,
		const std::map<wxString, ChildData> &elmtMap )
{
	bool deadlock = false;

	sequence.clear();

	std::vector<wxString>::iterator seqItr, seqFindItr;
	std::set<wxString>::const_iterator prevItr, prevEnd;
	std::map<wxString, ChildData>::const_iterator itr;

	bool retry;
	do
	{
		retry = false;
		for ( itr = elmtMap.begin(); itr != elmtMap.end(); itr++ )
		{
			seqFindItr = std::find ( sequence.begin(), sequence.end(),
					itr->first );
			if ( seqFindItr != sequence.end() )
				continue;

			seqItr = sequence.begin();
			prevItr = itr->second.prevSiblings.begin();
			prevEnd = itr->second.prevSiblings.end();
			for ( ; prevItr != prevEnd; prevItr++ )
			{ // Find last index of dependent elements
				seqFindItr = std::find ( sequence.begin(), sequence.end(),
						*prevItr );
				if ( seqFindItr != sequence.end() )
				{
					if ( seqItr < seqFindItr )
					{
						seqItr = seqFindItr;
					}
					continue;
				}
				const std::set<wxString> &previous =
						elmtMap.find ( *prevItr )->second.prevSiblings;
				if ( previous.find ( itr->first ) == previous.end() )
				{ // Not a deadlock
					retry = true;
					break;
				}
				else
				{
					deadlock = true;
				}
			}
			if ( prevItr != prevEnd )
				continue; // The preceding doesn't exist

			if ( seqItr != sequence.end() )
			{
				seqItr++;
			}
			sequence.insert ( seqItr, itr->first );
			wxLogDebug ( _T(" %s"), itr->first.c_str() );
		}
	} while ( retry );

	return !deadlock;
}
