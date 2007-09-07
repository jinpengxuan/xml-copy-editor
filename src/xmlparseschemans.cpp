#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlparseschemans.h"

XmlParseSchemaNs::XmlParseSchemaNs() : WrapExpat ( false ), d ( new ParseSchemaNsData() )
{
    d->p = p; // set parser for XML_StopParser call
    XML_SetUserData ( p, d.get() );
    XML_SetStartElementHandler ( p, start );
}

XmlParseSchemaNs::~XmlParseSchemaNs()
{}

void XMLCALL XmlParseSchemaNs::start ( void *data,
                                       const XML_Char *el,
                                       const XML_Char **attr )
{
    ParseSchemaNsData *d = ( ParseSchemaNsData * ) data;
    if ( !d )
        return;

    while ( *attr )
    {
        d->attributeVector.push_back ( make_pair ( *attr, * ( attr + 1 ) ) );
        attr += 2;
    }
    XML_StopParser ( d->p, false );
}
