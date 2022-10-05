/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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

#ifndef RULE_H
#define RULE_H

#include <string>
#include "wrapregex.h"

using std::string;

class Rule : public WrapRegex
{
	public:
		Rule (
		    const string& pattern,
		    bool matchCase,
		    const string& replace = "");
		bool getAdjustCaseAttribute();
		bool getTentativeAttribute();
		string getReport();
		void setReport ( string &s );
		void setAdjustCaseAttribute ( bool b );
		void setTentativeAttribute ( bool b );
	private:
		bool adjustCaseAttribute, tentativeAttribute;
		string reportString;
};

#endif
