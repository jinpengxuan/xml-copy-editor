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

#ifndef XML_PROMPT_GENERATOR_H
#define XML_PROMPT_GENERATOR_H

//#include <expat.h>
#include <map>
#include <set>
#include <memory>
#include "wrapexpat.h"
#include "parserdata.h"

struct PromptGeneratorData : public ParserData
{
	std::map<std::string, std::map<std::string, std::set<std::string> > >
	attributeMap;
	std::map<std::string, std::set<std::string> > elementMap;
	std::map<std::string, std::set<std::string> > requiredAttributeMap;
	std::map<std::string, std::string> elementStructureMap;
	std::set<std::string> entitySet;
	std::string catalogPath, basePath, auxPath, rootElement;
	int elementDeclRecurseLevel;
	bool isRootElement, grammarFound;
	unsigned attributeValueCutoff;
	XML_Parser p;
};

class XmlPromptGenerator : public WrapExpat
{
	public:
		XmlPromptGenerator (
		    const std::string& catalogPath = "",
		    const std::string& basePath = "",
		    const std::string& auxPath = "" );
		virtual ~XmlPromptGenerator();
		void getAttributeMap (
		    std::map<std::string, std::map<std::string, std::set<std::string> > >
		    &attributeMap );
		void getRequiredAttributeMap (
		    std::map<std::string, std::set<std::string> > &requiredAttributeMap );
		void getElementMap (
		    std::map<std::string, std::set<std::string> > &elementMap );
		void getEntitySet (
		    std::set<std::string> &entitySet );
		bool getGrammarFound();
		void getElementStructureMap (
		    std::map<std::string, std::string> &elementStructureMap );
	private:
		std::auto_ptr<PromptGeneratorData> d;
		static void XMLCALL starthandler (
		    void *data,
		    const XML_Char *el,
		    const XML_Char **attr );
		static void XMLCALL endhandler (
		    void *data,
		    const XML_Char *el );
		static void XMLCALL doctypedeclstarthandler (
		    void *userData,
		    const XML_Char *doctypeName,
		    const XML_Char *sysid,
		    const XML_Char *pubid,
		    int has_internal_subset );
		static void XMLCALL doctypedeclendhandler ( void *userData );
		static void XMLCALL elementdeclhandler (
		    void *userData,
		    const XML_Char *name,
		    XML_Content *model );
		static void XMLCALL attlistdeclhandler (
		    void *userData,
		    const XML_Char *elname,
		    const XML_Char *attname,
		    const XML_Char *att_type,
		    const XML_Char *dflt,
		    int isrequired );
		static int XMLCALL externalentityrefhandler (
		    XML_Parser p,
		    const XML_Char *context,
		    const XML_Char *base,
		    const XML_Char *systemId,
		    const XML_Char *publicId );
		static void XMLCALL entitydeclhandler (
		    void *userData,
		    const XML_Char *entityName,
		    int is_parameter_entity,
		    const XML_Char *value,
		    int value_length,
		    const XML_Char *base,
		    const XML_Char *systemId,
		    const XML_Char *publicId,
		    const XML_Char *notationName );
		static void handleSchema (
		    PromptGeneratorData *d,
		    const XML_Char *el,
		    const XML_Char **attr );
};

#endif
