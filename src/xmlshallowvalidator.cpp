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

#include <wx/wx.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include <map>
#include <set>
#include "xmlshallowvalidator.h"

XmlShallowValidator::XmlShallowValidator (
    std::map<wxString, std::set<wxString> > &elementMap,
    std::map<wxString, std::map<wxString, std::set<wxString> > >
    &attributeMap,
    std::map<wxString, std::set<wxString> > &requiredAttributeMap,
    std::set<wxString> &entitySet,
    int maxLine,
    bool segmentOnly ) : vd ( new XmlShallowValidatorData() )
{
	vd->elementMap = elementMap;
	vd->attributeMap = attributeMap;
	vd->requiredAttributeMap = requiredAttributeMap;
	vd->entitySet = entitySet;
	vd->isValid = true;
	vd->p = p;
	vd->depth = 0;
	vd->maxLine = maxLine;
	vd->segmentOnly = segmentOnly;
	vd->overrideFailure = false;

	XML_SetUserData ( p, vd.get() );
	XML_SetElementHandler ( p, start, end );
	XML_SetSkippedEntityHandler ( p, skippedentity );
	//XML_SetExternalEntityRefHandler ( p, externalentity );
}

XmlShallowValidator::~XmlShallowValidator()
{}

void XMLCALL XmlShallowValidator::start ( void *data,
        const XML_Char *el,
        const XML_Char **attr )
{
	XmlShallowValidatorData *vd;
	vd = ( XmlShallowValidatorData * ) data;

	if ( XML_GetCurrentLineNumber ( vd->p ) > ( unsigned ) ( vd->maxLine + 1 ) )
	{
		XML_StopParser ( vd->p, true );
	}

	vd->push ( el );
	++ ( vd->depth );

	//check element ok
	wxString parent ( vd->getParent().c_str(), wxConvUTF8 );
	if ( parent.empty() )
		return;

	if ( vd->elementMap.empty() )
		return;

	wxString element ( vd->getElement().c_str(), wxConvUTF8 );
	if ( !vd->elementMap[parent].count ( element ) )
	{
		vd->isValid = false;
		vd->positionVector.push_back (
		    make_pair ( XML_GetCurrentLineNumber ( vd->p ), XML_GetCurrentColumnNumber ( vd->p ) ) );
	}

	element = wxString ( el, wxConvUTF8 );

	std::map<wxString, std::set<wxString> > attributeMap;
	size_t requiredAttributeCount = vd->requiredAttributeMap[element].size();
	wxString currentAttribute;

	while ( *attr )
	{
		attributeMap = vd->attributeMap[element];

		// check for existence
		currentAttribute = wxString ( *attr, wxConvUTF8 );
		if ( !attributeMap.count ( currentAttribute ) )
		{
			vd->isValid = false;
			vd->positionVector.push_back ( make_pair (
			                                   XML_GetCurrentLineNumber ( vd->p ),
			                                   XML_GetCurrentColumnNumber ( vd->p ) ) );
		}
		// check for requirement
		if ( vd->requiredAttributeMap[element].find ( currentAttribute ) !=
		        vd->requiredAttributeMap[element].end() )
			--requiredAttributeCount;

		attr += 2;
	}
	if ( requiredAttributeCount != 0 )
	{
		vd->isValid = false;
		vd->positionVector.push_back ( make_pair (
		                                   XML_GetCurrentLineNumber ( vd->p ),
		                                   XML_GetCurrentColumnNumber ( vd->p ) ) );
	}
}

void XMLCALL XmlShallowValidator::end ( void *data, const XML_Char *el )
{
	XmlShallowValidatorData *vd;
	vd = ( XmlShallowValidatorData * ) data;
	vd->pop();
	-- ( vd->depth );

	// segments: stop at end tag of first element
	if ( vd->segmentOnly && vd->depth == 0 )
	{
		XML_StopParser ( vd->p, true );
		if ( vd->isValid )
			vd->overrideFailure = true;
	}
}

void XMLCALL XmlShallowValidator::skippedentity (
    void *data,
    const XML_Char *entityName,
    int is_parameter_entity )
{
	if ( is_parameter_entity )
		return;
	XmlShallowValidatorData *vd = ( XmlShallowValidatorData * ) data;
	wxString entity ( entityName, wxConvUTF8 );
	if ( vd->entitySet.find ( entity ) != vd->entitySet.end() )
		return;
	
	vd->isValid = false;
	vd->positionVector.push_back (
	    make_pair ( XML_GetCurrentLineNumber ( vd->p ), XML_GetCurrentColumnNumber ( vd->p ) ) );
}

/*
int XMLCALL XmlShallowValidator::externalentity (
	XML_Parser p,
	const XML_Char *context,
        const XML_Char *base,
        const XML_Char *systemId,
        const XML_Char *publicId)
{
	return XML_STATUS_OK;
}
*/


bool XmlShallowValidator::isValid()
{
	return vd->isValid;
}

std::vector<std::pair<int, int> > XmlShallowValidator::getPositionVector()
{
	return vd->positionVector;
}

bool XmlShallowValidator::getOverrideFailure()
{
	return vd->overrideFailure;
}
