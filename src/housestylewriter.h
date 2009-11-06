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

#ifndef HOUSE_STYLE_WRITER_H
#define HOUSE_STYLE_WRITER_H

#include <set>
#include <string>
#include <memory>
#include <expat.h>
#include "wrapexpat.h"
#include "wrapregex.h"
#include "contexthandler.h"

using namespace std;

struct HouseStyleWriterData : public ParserData
{
	set<size_t> elementSet;
	string output, textnode;
	vector<ContextMatch> v;
};

class HouseStyleWriter : public WrapExpat
{
	public:
		HouseStyleWriter ( vector<ContextMatch> &v );
		~HouseStyleWriter();
		string getOutput();
	private:
		std::auto_ptr<HouseStyleWriterData> hswd;
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
		static void XMLCALL xmldecl ( void *userData, const XML_Char *version, const XML_Char *encoding, int standalone );
		static void XMLCALL cdatastart ( void *userData );
		static void XMLCALL cdataend ( void *userData );
		static void XMLCALL defaulthandler ( void *userData, const XML_Char *s, int len );
		static void modify ( vector<ContextMatch> &v, string &buffer, unsigned elementCount );
};


#endif
