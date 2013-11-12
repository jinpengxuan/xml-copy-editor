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

#ifndef WRAPREGEX_H
#define WRAPREGEX_H

#include <iostream>
#include <string>
#include <vector>
#include <pcre.h>
#include <boost/utility.hpp>
#include "contexthandler.h"

using namespace std;

class WrapRegex : private boost::noncopyable
{
	public:
		WrapRegex (
		    const string& pattern,
		    bool matchCase,
		    const string& replaceParameter = "",
		    const int arrayLengthParameter = 60 );
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
		const int arrayLength;
		int returnValue;
		bool disabled;

		pcre *patternStructure;
		pcre_extra *patternExtraStructure;
		int *matchArray;

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
