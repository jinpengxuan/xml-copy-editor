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
#include <expat.h>
#include "wrapexpat.h"

struct XmlShallowValidatorData : public ParserData
{
    XmlShallowValidatorData()
    {}
    std::map<std::string, std::set<std::string> > elementMap;
    std::map<std::string, std::map<std::string, std::set<std::string> > >
        attributeMap;
    std::map<std::string, std::set<std::string> > requiredAttributeMap;
    std::set<std::string> entitySet;
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
        std::map<std::string, std::set<std::string> > &elementMap,
            std::map<std::string, std::map<std::string, std::set<std::string> > >
                &attributeMap,
                std::map<std::string, std::set<std::string> > &requiredAttributeMap,
                    std::set<std::string> &entitySet,
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
};

#endif
