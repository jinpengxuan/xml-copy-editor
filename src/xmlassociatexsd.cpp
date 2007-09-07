#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlassociatexsd.h"
#include "xmlparseschemans.h"
#include "readfile.h"
#include "replace.h"

XmlAssociateXsd::XmlAssociateXsd (
    const std::string& path,
    size_t size ) :
        d ( new AssociateXsdData() )
{
    d->buffer.reserve ( size );
    d->path = path;
    d->rootElementSeen = false;
    XML_SetElementHandler ( p, start, end );
    XML_SetDefaultHandlerExpand ( p, defaulthandler );

    std::auto_ptr<XmlParseSchemaNs> parser ( new XmlParseSchemaNs() );
    std::string normalisedPath, buffer;
    normalisedPath = path;
    Replace::run ( normalisedPath, "%20", " ", true );
    if ( !ReadFile::run ( normalisedPath, buffer ) )
        return;
    parser->parse ( buffer );
    std::vector<std::pair<std::string, std::string> > attributeVector;
    attributeVector = parser->getAttributeVector();
    std::vector<std::pair<std::string, std::string> >::iterator it;
    for ( it = attributeVector.begin(); it != attributeVector.end(); it++ )
    {
        if ( it->first == "targetNamespace" )
        {
            namespaceMap.insert ( make_pair ( "xmlns", it->second ) );
        }
        else if ( it->second == "http://www.w3.org/2001/XMLSchema" )
        {
            ; // skip
        }
        else if ( it->first.find ( ':' ) != std::string::npos )
        {
            it->first.erase ( 0, it->first.find ( ':' ) );
            it->first.insert ( 0, "xmlns" );
            namespaceMap.insert ( make_pair ( it->first, it->second ) );
        }
    }
    d->namespaceMap = namespaceMap;
    XML_SetUserData ( p, d.get() );
}

XmlAssociateXsd::~XmlAssociateXsd()
{}

void XMLCALL XmlAssociateXsd::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
    AssociateXsdData *d;
    d = ( AssociateXsdData * ) data;
    d->buffer.append ( s, len );
}

void XMLCALL XmlAssociateXsd::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
    AssociateXsdData *d;
    d = ( AssociateXsdData * ) data;

    d->buffer += "<";
    d->buffer += el;

    while ( *attr )
    {
        if ( strstr ( *attr, "xmlns:" ) ||
                !strcmp ( *attr, "xmlns" ) ||
                !strcmp ( *attr, "xsi:noNamespaceSchemaLocation" ) ||
                !strcmp ( *attr, "xsi:schemaLocation" ) )
            ;
        else
        {
            d->buffer += " ";
            d->buffer += *attr;
            d->buffer += "=\"";
            d->buffer += xmliseAttribute ( * ( attr + 1 ) );
            d->buffer += "\"";
        }
        attr += 2;
    }
    if ( !d->rootElementSeen )
    {
        d->buffer += " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
        std::map<std::string, std::string>::iterator it;
        for ( it = d->namespaceMap.begin(); it != d->namespaceMap.end(); it++ )
        {
            d->buffer += " ";
            d->buffer += it->first;
            d->buffer += "=\"";
            d->buffer += it->second;
            d->buffer += "\"";
        }

        d->buffer += " xsi:";
        bool withNamespace =
            ( d->namespaceMap.find ( "xmlns" ) != d->namespaceMap.end() );
        d->buffer += ( withNamespace ) ? "schemaLocation" : "noNamespaceSchemaLocation";
        d->buffer += "=\"";
        if ( withNamespace )
        {
            d->buffer += d->namespaceMap["xmlns"];
            d->buffer += " ";
        }
        d->buffer += d->path;
        d->buffer += "\"";
        d->rootElementSeen = true;
    }
    d->buffer += ">";
}

void XMLCALL XmlAssociateXsd::end ( void *data, const XML_Char *el )
{
    AssociateXsdData *d;
    d = ( AssociateXsdData * ) data;
    d->buffer += "</";
    d->buffer += el;
    d->buffer += ">";
}
