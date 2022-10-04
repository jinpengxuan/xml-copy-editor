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

#ifndef WRAPREGEX_H
#define WRAPREGEX_H

#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif

#include <iostream>
#include <string>
#include <vector>
#include <pcre2.h>
#include <boost/utility.hpp>
#include "contexthandler.h"

using namespace std;

class WrapRegex : private boost::noncopyable
{
	public:
		WrapRegex (
		    const string& pattern,
		    bool matchCase,
		    const string& replaceParameter = "" );
		virtual ~WrapRegex();
		string replaceGlobal (
		    const string& buffer,
		    int *matchCount );
		int matchPatternGlobal (
		    string &buffer,
		    vector<ContextMatch> &match_vector,
		    unsigned elementCount,
		    int context = 0 );
	private:
		string replace;
		int returnValue;
		bool disabled;

		pcre2_code *patternCode;
		pcre2_match_data *patternMatchData;
		pcre2_match_context *patternMatchContext;

		string getInterpolatedString_ ( const char *buffer,
		    const char *source );
		string getSubpattern_ ( const char *buffer, unsigned subpattern );
		int matchPatternGlobal_ (
		    const char *buffer,
		    size_t buflen,
		    vector<ContextMatch> &matchVector,
		    unsigned elementCount,
		    int context );
};

#endif
