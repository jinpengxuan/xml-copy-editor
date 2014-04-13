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

#ifndef XML_SCHEMA_PARSER_H
#define XML_SCHEMA_PARSER_H

#include <expat.h>
#include <string>
#include "wrapexpat.h"
#include "xmlpromptgenerator.h"

struct SchemaParserData : public ParserData
{
    PromptGeneratorData *promptData;
    std::map<std::string, std::set<std::string> > referenceMap;
    std::string currentElement;
};

class XmlSchemaParser : public WrapExpat
{
public:
    XmlSchemaParser (
        PromptGeneratorData *data,
        const char *encoding,
        bool nameSpaceAware );
    virtual ~XmlSchemaParser();
    enum {
        STATE_UNKNOWN,
        STATE_ROOT,
        STATE_ELEMENT,
        STATE_SIMPLE_TYPE,
        STATE_COMPLEX_TYPE,
        STATE_SEQUENCE,
        STATE_CHOICE
    };
private:
    std::auto_ptr<SchemaParserData> d;
    static void XMLCALL starthandler (
        void *data,
        const XML_Char *el,
        const XML_Char **attr );
    static void XMLCALL endhandler (
        void *data,
        const XML_Char *el );
};

#endif
