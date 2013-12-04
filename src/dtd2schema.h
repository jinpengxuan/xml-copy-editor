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

#ifndef DTD2SCHEMA_H_
#define DTD2SCHEMA_H_

#include <map>
#include <xercesc/validators/DTD/DTDGrammar.hpp>

using namespace xercesc;

class Dtd2Schema
{
public:
	Dtd2Schema();
	virtual ~Dtd2Schema();

	const wxString &convert ( const wxString &dtdFile );
	const wxString &getErrors() { return mErrors; }

	void reset ();

protected:
	wxString convertElement ( const DTDElementDecl &element, size_t nIndent );
	wxString convertContent ( const ContentSpecNode *content, size_t nIndent,
			const ContentSpecNode *parent = NULL );
	wxString convertOccurrence (const ContentSpecNode *content );
	wxString convertAttribute ( const XMLAttDef &att, size_t nIndent,
			const wxString &element );
	wxString convertAttType ( const XMLAttDef &att, size_t nIndent );
	wxString convertAttValue ( const XMLAttDef &att );
	wxString convertEntity ( const DTDEntityDecl &entity, size_t nIndent );
	wxString convertNotation ( const XMLNotationDecl &notation, size_t nIndent );

protected:
	wxString mSchema;
	wxString mErrors;
	wxString mTargetNameSpace;
	std::map<wxString, wxString> mNameSpaceMap;
};

#endif /* DTD2SCHEMA_H_ */
