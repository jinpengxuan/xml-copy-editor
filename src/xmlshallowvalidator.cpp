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

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include <map>
#include <set>
#include "xmlshallowvalidator.h"

XmlShallowValidator::XmlShallowValidator (
    std::map<std::string, std::set<std::string> > &elementMap,
    std::map<std::string, std::map<std::string, std::set<std::string> > >
    &attributeMap,
    std::map<std::string, std::set<std::string> > &requiredAttributeMap,
    std::set<std::string> &entitySet,
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
	std::string parent = vd->getParent();
	if ( parent.empty() )
		return;

	if ( vd->elementMap.empty() )
		return;

	if ( !vd->elementMap[parent].count ( vd->getElement() ) )
	{
		vd->isValid = false;
		vd->positionVector.push_back (
		    make_pair ( XML_GetCurrentLineNumber ( vd->p ), XML_GetCurrentColumnNumber ( vd->p ) ) );
	}

	std::map<std::string, std::set<std::string> > attributeMap;
	size_t requiredAttributeCount = vd->requiredAttributeMap[el].size();
	std::string currentAttribute;

	while ( *attr )
	{
		attributeMap = vd->attributeMap[el];

		// check for existence
		if ( !attributeMap.count ( *attr ) )
		{
			vd->isValid = false;
			vd->positionVector.push_back ( make_pair (
			                                   XML_GetCurrentLineNumber ( vd->p ),
			                                   XML_GetCurrentColumnNumber ( vd->p ) ) );
		}
		// check for requirement
		currentAttribute = ( const char * ) *attr;
		if ( vd->requiredAttributeMap[el].find ( currentAttribute ) !=
		        vd->requiredAttributeMap[el].end() )
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
	XmlShallowValidatorData *vd;
	vd = ( XmlShallowValidatorData * ) data;
	if ( vd->entitySet.find ( entityName ) != vd->entitySet.end() )
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
