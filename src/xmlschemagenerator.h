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

#ifndef XMLSCHEMAGENERATOR_H_
#define XMLSCHEMAGENERATOR_H_

#include <wx/wx.h>
#include <wx/textfile.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/validators/common/Grammar.hpp>

using namespace xercesc;

class XmlSchemaGenerator
{
public:
	XmlSchemaGenerator ( bool inlineSimpleType = true );
	virtual ~XmlSchemaGenerator();

	const wxString &generate ( Grammar::GrammarType grammarType,
			const wxString &filepath, const char *buffer, size_t len,
			const wxString &encoding );
	const wxString &getLastError() { return mLastError; }

	static void addIndent ( wxString &str, size_t nIndent )
	{
		for ( size_t i = nIndent; i-- > 0; )
			str << _T("  ");
	}
	static const wxChar *getEOL() { return wxTextFile::GetEOL(); }

protected:
	class ChildData
	{
	public:
		ChildData() : minOccurs ( 1 ), maxOccurs ( 1 ) {}
		size_t minOccurs, maxOccurs;
		std::set<wxString> precedence;
	};
	class ElmtData
	{
	public:
		ElmtData() : useSequence ( true ), mixed ( false ) { }

		// All occurs
		std::set<const DOMElement*> nodes;

		// Node name. Also used to indicate if the following data are valid
		wxString name;
		// These are not needed when we don't need to inline empty elements.
		// The schema can be created right after we have all the data.
		std::map<wxString, ChildData> children;
		// Sequence of children
		std::vector<wxString> sequence;
		bool useSequence; // Use xs:sequence or xs:choice
		// Specifies whether character data is allowed to appear between the
		// child elements of this complexType element
		bool mixed;
		// Attribute name and default value
		std::map<wxString, const XMLCh *> attrMap;
		// Optional attributes
		std::set<wxString> optAttrs;

		wxString schema;
	};

	void findAllElements ( const DOMElement &element, size_t nIndent = 0 );
	void generateData ( const DOMElement &element, size_t nIndent = 0 );
	void generateData ( const wxString &elementName, size_t nIndent = 0 );
	void outputSchema ( const DOMElement &element );
	void generateSchema ( ElmtData &data, size_t nIndent );
	void generateDTD ( ElmtData &data, size_t nIndent );
	// Returns false if there is a loop dependence, which means that
	// <xs:choice> has to be used.
	bool getSequence ( std::vector<wxString> &sequence,
			const std::map<wxString, ChildData> &elmtMap );

protected:
	bool mInlineSimpleType;
	Grammar::GrammarType mGrammarType;
	std::map<wxString, ElmtData> mElements;
	wxString mSchema, mLastError;
};

#endif /* XMLSCHEMAGENERATOR_H_ */
