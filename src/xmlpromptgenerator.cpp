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
#include <wx/filename.h>
#include <stdexcept>
#include "xmlpromptgenerator.h"
#include "xmlencodinghandler.h"
#include "readfile.h"
#include "replace.h"
#include "getword.h"
#include "pathresolver.h"
#include "catalogresolver.h"
#include "xmlschemaparser.h"

XmlPromptGenerator::XmlPromptGenerator (
    const std::string& catalogPath,
    const std::string& basePath,
    const std::string& auxPath ) : d ( new PromptGeneratorData() )
{
    XML_SetUserData ( p, d.get() );
    d->p = p;
    d->catalogPath = catalogPath;
    d->auxPath = auxPath;
    d->elementDeclRecurseLevel = 0;
    d->rootElement = true;
    d->dtdFound = false;
    XML_SetParamEntityParsing ( p, XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE );
    XML_SetElementHandler ( p, starthandler, endhandler );
    XML_SetDoctypeDeclHandler ( p, doctypedeclstarthandler, doctypedeclendhandler );
    XML_SetElementDeclHandler ( p, elementdeclhandler );
    XML_SetAttlistDeclHandler ( p, attlistdeclhandler );
    XML_SetEntityDeclHandler ( p, entitydeclhandler );
    XML_SetExternalEntityRefHandlerArg ( p, d.get() );
    XML_SetExternalEntityRefHandler ( p, externalentityrefhandler );
    XML_SetBase ( p, basePath.c_str() );

    if ( !auxPath.empty() )
        XML_UseForeignDTD ( p, true );
}

XmlPromptGenerator::~XmlPromptGenerator()
{}

void XMLCALL XmlPromptGenerator::starthandler (
    void *data,
    const XML_Char *el,
    const XML_Char **attr )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) data;

    d->push ( el );

    std::string parent, element;
    parent = d->getParent();
    element = el;

    // update elementMap
    if ( d->elementMap.find ( parent ) == d->elementMap.end() )
    {
        std::set<std::string> childSet;
        childSet.insert ( element );
        d->elementMap.insert ( make_pair ( parent, childSet ) );
    }
    else
        d->elementMap[parent].insert ( element );

    std::string attributeName, attributeValue;

    // update attributeMap
    // case 1: element unknown, no attributes
    if ( ! ( *attr ) && d->attributeMap.find ( element ) == d->attributeMap.end() )
    {
        std::map<std::string, std::set<std::string> > currentAttributeMap;
        d->attributeMap.insert ( make_pair ( element, currentAttributeMap ) );
    }

    for ( ; *attr; attr += 2 )
    {
        attributeName = *attr;
        attributeValue = * ( attr + 1 );

        d->attributeMap[element][attributeName].insert ( attributeValue );

        /*
            // TBD: may not be xsi: check for http://www.w3.org/2001/XMLSchema-instance
            while (d->rootElement && strstr((const char *)attr, "xsi:noNamespaceSchemaLocation"))
            {
              std::string schemaPath = PathResolver::run(attributeValue, d->auxPath);
              std::string buffer;
              if (!ReadFile::run(schemaPath, buffer))
                break;
              XmlSchemaParser xsp(d, true);
              if (!xsp.parse(buffer))
                break;
              XML_StopParser(d->p, false);
              return;
            }
        */
    }
    d->rootElement = false;
}

void XMLCALL XmlPromptGenerator::endhandler ( void *data, const XML_Char *el )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) data;
    d->pop();
}

bool XmlPromptGenerator::getDtdFound()
{
    return d->dtdFound;
}

void XmlPromptGenerator::getAttributeMap (
    std::map<std::string, std::map<std::string, std::set<std::string> > >
        &attributeMap )
{
    attributeMap = d->attributeMap;
}

void XmlPromptGenerator::getRequiredAttributeMap (
    std::map<std::string, std::set<std::string> >& requiredAttributeMap )
{
    requiredAttributeMap = d->requiredAttributeMap;
}

void XmlPromptGenerator::getElementMap (
    std::map<std::string, std::set<std::string> > &elementMap )
{
    elementMap = d->elementMap;
}

void XmlPromptGenerator::getEntitySet (
    std::set<std::string> &entitySet )
{
    entitySet = d->entitySet;
}

// handlers for DOCTYPE handling

void XMLCALL XmlPromptGenerator::doctypedeclstarthandler (
    void *data,
    const XML_Char *doctypeName,
    const XML_Char *sysid,
    const XML_Char *pubid,
    int has_internal_subset )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) data;
}

void XMLCALL XmlPromptGenerator::doctypedeclendhandler ( void *data )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) data;
    if ( !d->elementMap.empty() )
    {
        d->dtdFound = true;
        XML_StopParser ( d->p, false );
    }
}

void XMLCALL XmlPromptGenerator::elementdeclhandler (
    void *data,
    const XML_Char *name,
    XML_Content *model )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) data;

    d->elementDeclRecurseLevel += 1;

    std::string myElement = name;
    unsigned num = model->numchildren;

    for ( unsigned i = 0; i < num; i++ )
    {
        XML_Content myContent = model->children[i];
        XML_Char *myName = myContent.name;
        if ( myName )
            d->elementMap[myElement].insert ( ( const char * ) myName );
        else
        {
            // recurse
            XmlPromptGenerator::elementdeclhandler ( ( void * ) d, name, &myContent );
        }
    }
    d->elementDeclRecurseLevel -= 1;

    // only one call to XML_FreeContentModel per content tree
    if ( d->elementDeclRecurseLevel == 0 )
    {
        XML_FreeContentModel ( d->p, model );
    }
}

void XMLCALL XmlPromptGenerator::attlistdeclhandler (
    void *data,
    const XML_Char *elname,
    const XML_Char *attname,
    const XML_Char *att_type,
    const XML_Char *dflt,
    int isrequired )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) data;

    std::set<std::string> attributeValues;
    if ( *att_type == '(' ) // change to exclude _known_ identifiers?
    {
        size_t len;
        char *s, *word;
        s = ( char * ) att_type;

        while ( ( word = GetWord::run ( &s, &len ) ) != NULL )
        {
            std::string currentValue ( word, len );
            attributeValues.insert ( currentValue );
        }
    }

    if ( attributeValues.empty() )
    {
        d->attributeMap[elname][attname].insert ( "" );
        return;
    }
    std::set<std::string>::iterator it;
    for ( it = attributeValues.begin(); it != attributeValues.end(); it++ )
        d->attributeMap[elname][attname].insert ( *it );

    if ( isrequired )
    {
        d->requiredAttributeMap[elname].insert ( attname );
    }
}

int XMLCALL XmlPromptGenerator::externalentityrefhandler (
    XML_Parser p,
    const XML_Char *context,
    const XML_Char *base,
    const XML_Char *systemId,
    const XML_Char *publicId )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) p; // arg is set to user data in c'tor

    std::string buffer;

    // auxPath req'd?
    if ( !systemId && !publicId )
    {
        ReadFile::run ( d->auxPath, buffer );
        if ( buffer.empty() )
        {
            return false;
        }

        std::string encoding = XmlEncodingHandler::get ( buffer );
        XML_Parser dtdParser = XML_ExternalEntityParserCreate ( d->p, context, encoding.c_str() );
        if ( !dtdParser )
            return false;
        XML_SetBase ( dtdParser, d->auxPath.c_str() );
        return XML_Parse ( dtdParser, buffer.c_str(), buffer.size(), true );
    }

    std::string stdPublicId;
    if ( publicId )
        stdPublicId = publicId;

    std::string stdSystemId = CatalogResolver::lookupPublicId ( stdPublicId, d->catalogPath );
    if ( stdSystemId.empty() && systemId )
        stdSystemId = systemId;

    Replace::run ( stdSystemId, "file:///", "", false );
    Replace::run ( stdSystemId, "%20", " ", false );

    if ( base )
    {
        std::string test = PathResolver::run ( stdSystemId, base );
        if ( !test.empty() )
        {
            stdSystemId = test;
        }
    }

    if ( !stdSystemId.empty() )
    {
        ReadFile::run ( stdSystemId, buffer );
    }

    std::string encoding = XmlEncodingHandler::get ( buffer );
    XML_Parser dtdParser = XML_ExternalEntityParserCreate ( d->p, context, encoding.c_str() );//"UTF-8");
    if ( !dtdParser )
        return false;

    wxString wideName, wideDir;
    wideName = wxString ( stdSystemId.c_str(), wxConvUTF8, stdSystemId.size() );
    wxFileName fn ( wideName );
    wideDir = fn.GetPath();
    XML_SetBase ( dtdParser, wideName.mb_str ( wxConvUTF8 ) );

    return XML_Parse ( dtdParser, buffer.c_str(), buffer.size(), true );
}

void XMLCALL XmlPromptGenerator::entitydeclhandler (
    void *data,
    const XML_Char *entityName,
    int is_parameter_entity,
    const XML_Char *value,
    int value_length,
    const XML_Char *base,
    const XML_Char *systemId,
    const XML_Char *publicId,
    const XML_Char *notationName )
{
    PromptGeneratorData *d;
    d = ( PromptGeneratorData * ) data; // arg is set to user data in c'tor

    if (
        entityName &&
        !is_parameter_entity &&
        !systemId &&
        !publicId &&
        !notationName )
    {
        d->entitySet.insert ( entityName );
    }
}
