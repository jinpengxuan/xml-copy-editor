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
            }

XmlShallowValidator::~XmlShallowValidator()
{}

void XMLCALL XmlShallowValidator::start ( void *data,
        const XML_Char *el,
        const XML_Char **attr )
{
    XmlShallowValidatorData *vd;
    vd = ( XmlShallowValidatorData * ) data;

#ifdef __WXMSW__
    if ( XML_GetCurrentLineNumber ( vd->p ) > ( unsigned ) ( vd->maxLine + 1 ) )
#else
    if ( XML_GetCurrentLineNumber ( vd->p ) > ( vd->maxLine + 1 ) )
#endif
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
