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

#ifndef HOUSE_STYLE_READER_H
#define HOUSE_STYLE_READER_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <expat.h>
#include "wrapexpat.h"

using namespace std;

struct HouseStyleReaderData : public ParserData
{
	bool filterActive;
	string textnode;
	vector<pair<string, unsigned> > nodevector;
	unsigned depth, cutoffDepth;
	map<string, map<string, set<string> > > filterMap;
};

class HouseStyleReader : public WrapExpat
{
	public:
		HouseStyleReader (
		    map<string, map<string, set<string> > > &m,
		    const char *encoding = NULL
		);
		virtual ~HouseStyleReader();
		void getNodeVector ( vector<pair<string, unsigned> > &v );
	private:
		std::auto_ptr<HouseStyleReaderData> ud;
		enum
		{
			STATE_UNKNOWN,
			STATE_ON_START,
			STATE_ON_END,
			STATE_ON_CDATA_START,
			STATE_ON_CDATA_END
		};

		static void XMLCALL start ( void *data, const XML_Char *el, const XML_Char **attr );
		static void XMLCALL end ( void *data, const XML_Char *el );
		static void XMLCALL characterdata ( void *userData, const XML_Char *s, int len );
		static void XMLCALL cdatastart ( void *userData );
		static void XMLCALL cdataend ( void *userData );
};

#endif
