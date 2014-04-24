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

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <map>
#include <set>
#include <memory>
#include <string>
#include "wrapexpat.h"
#include "parserdata.h"
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/schema/SchemaGrammar.hpp>

DECLARE_EVENT_TYPE ( myEVT_NOTIFY_PROMPT_GENERATED, wxID_ANY );

struct PromptGeneratorData : public ParserData
{
	std::map<wxString, std::map<wxString, std::set<wxString> > >
	attributeMap;
	std::map<wxString, std::set<wxString> > elementMap;
	std::map<wxString, std::set<wxString> > requiredAttributeMap;
	std::map<wxString, wxString> elementStructureMap;
	std::set<wxString> entitySet;
	wxString basePath, auxPath;
	std::string encoding, rootElement;
	bool isRootElement, grammarFound;
	unsigned attributeValueCutoff;
	XML_Parser p;
};

typedef std::map<const xercesc::SchemaElementDecl *, std::set<wxString> >
        SubstitutionMap;

class XmlPromptGenerator : public WrapExpat, public wxThread
{
	public:
		XmlPromptGenerator (
		    wxEvtHandler *handler,
		    const char *buffer,
		    size_t bufferSize,
		    const wxString& basePath = wxEmptyString,
		    const wxString& auxPath = wxEmptyString,
		    const char *encoding = NULL );
		virtual ~XmlPromptGenerator();

		virtual void *Entry();
		void PendingDelete();
		// Since we can't call wxThread::m_internal->Cancel(), the original
		// TestDestroy() is useless. Here is the work around.
		virtual void Cancel() { mStopping = true; }
		virtual bool TestDestroy() { return mStopping || wxThread::TestDestroy(); }

		const std::map<wxString, std::map<wxString, std::set<wxString> > >
		    &getAttributeMap()
		{
			return d->attributeMap;
		}
		const std::map<wxString, std::set<wxString> >
		    &getRequiredAttributeMap ()
		{
			return d->requiredAttributeMap;
		}
		const std::map<wxString, std::set<wxString> >
		    &getElementMap()
		{
			return d->elementMap;
		}
		const std::set<wxString>
		    &getEntitySet()
		{
			return d->entitySet;
		}
		bool getGrammarFound()
		{
			return d->grammarFound;
		}
		const std::map<wxString, wxString>
		    &getElementStructureMap()
		{
			return d->elementStructureMap;
		}
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
		void getContent (
		    const XML_Content &content,
		    wxString &contentModel,
		    std::set<wxString> &list );
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
		void handleSchema (
		    PromptGeneratorData *d,
		    const XML_Char *el,
		    const XML_Char **attr );
		void buildSubstitutionMap (
		    SubstitutionMap &substitutions,
		    const xercesc::SchemaGrammar &grammar );
		void buildElementPrompt (
		    PromptGeneratorData *d,
		    const xercesc::XMLElementDecl *element,
		    SubstitutionMap &substitutions );
		void getContent (
		    std::set<wxString> &list,
		    const xercesc::ContentSpecNode *spec,
		    SubstitutionMap &substitutions );
protected:
		wxEvtHandler *mEventHandler;
		std::string mBuffer;
		bool mStopping;
};

#endif
