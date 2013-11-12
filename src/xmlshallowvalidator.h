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

#ifndef XML_SHALLOW_VALIDATOR_H
#define XML_SHALLOW_VALIDATOR_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <memory>
#include <expat.h>
#include "wrapexpat.h"

struct XmlShallowValidatorData : public ParserData
{
	XmlShallowValidatorData()
	{
		isValid = segmentOnly = false;
		depth = maxLine = 0;
		p = 0;
		overrideFailure = false;
	}
	std::map<wxString, std::set<wxString> > elementMap;
	std::map<wxString, std::map<wxString, std::set<wxString> > >
	attributeMap;
	std::map<wxString, std::set<wxString> > requiredAttributeMap;
	std::set<wxString> entitySet;
	std::vector<std::pair<int, int> > positionVector;
	bool isValid, segmentOnly;
	int depth, maxLine;
	XML_Parser p;
	bool overrideFailure;
};

class XmlShallowValidator : public WrapExpat
{
	public:
		XmlShallowValidator (
		    std::map<wxString, std::set<wxString> > &elementMap,
		    std::map<wxString, std::map<wxString, std::set<wxString> > >
		    &attributeMap,
		    std::map<wxString, std::set<wxString> > &requiredAttributeMap,
		    std::set<wxString> &entitySet,
		    int maxLine = 0,
		    bool segmentOnly = false );
		virtual ~XmlShallowValidator();
		bool isValid();
		std::vector<std::pair<int, int> > getPositionVector();
		bool getOverrideFailure();
	private:
		std::auto_ptr<XmlShallowValidatorData> vd;
		static void XMLCALL start ( void *data, const XML_Char *el, const XML_Char **attr );
		static void XMLCALL end ( void *data, const XML_Char *el );
		static void XMLCALL skippedentity ( void *data, const XML_Char *entityName,
		        int is_parameter_entity );
		
/*
	static int XMLCALL externalentity ( XML_Parser p,
			const XML_Char *context,
                        const XML_Char *base,
                        const XML_Char *systemId,
                        const XML_Char *publicId);
*/
};

#endif
