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

#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <set>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "contexthandler.h"
#include "stringset.h"

using namespace std;

class Spellcheck
{
public:
    Spellcheck (
        boost::shared_ptr<StringSet<char> > dictionaryParameter,
        boost::shared_ptr<StringSet<char> > passiveDictionaryParameter );
    ~Spellcheck();
    inline bool checkWord ( string &s );
    void checkString (
        string &s,
        vector<ContextMatch> &v,
        int contextRange );
    string getSuggestion ( string &s );
private:
    boost::shared_ptr<StringSet<char> > dictionary, passiveDictionary;
    bool checkWord ( char *s, size_t len );
    char *getWord ( char **s, size_t *len );
    inline bool isWordCharacter ( char *s, size_t *bytes );
};
#endif
