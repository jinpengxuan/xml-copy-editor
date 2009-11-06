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

#ifndef XML_FILTER_READER_H
#define XML_FILTER_READER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <expat.h>
#include "wrapexpat.h"

using namespace std;

struct FilterData : public ParserData
{
	vector<string> excludeVector;
	vector<string> includeVector;
	map<string, map<string, set<string> > > filterMap;
	map<string, set<string> > attributeMap;
	set<string> attributeValueSet;
	int filterCount;
	string elementString, attributeKeyString, attributeValueString;
	string includeString, excludeString;
};

class XmlFilterReader : public WrapExpat
{
	public:
		XmlFilterReader();
		virtual ~XmlFilterReader();

		enum
		{
			STATE_UNKNOWN,
			STATE_IN_ELEMENT,
			STATE_IN_ATTRIBUTE,
			STATE_IN_ATTRIBUTE_KEY,
			STATE_IN_ATTRIBUTE_VALUE,
			STATE_IN_EXCLUDE,
			STATE_IN_INCLUDE
		};
		int getFilterCount();
		void getExcludeVector ( vector<string> &v );
		void getIncludeVector ( vector<string> &v );
		void getFilterMap ( map<string, map<string, set<string> > > &m );
	private:
		std::auto_ptr<FilterData> fd;
		static void XMLCALL start ( void *data, const XML_Char *el, const XML_Char **attr );
		static void XMLCALL end ( void *data, const XML_Char *el );
		static void XMLCALL characterdata ( void *data, const XML_Char *s, int len );
};

#endif
