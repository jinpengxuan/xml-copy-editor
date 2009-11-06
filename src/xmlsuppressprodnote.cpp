#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include <cstring>
#include "xmlsuppressprodnote.h"

XmlSuppressProdnote::XmlSuppressProdnote (
    bool parseDeclaration,
    bool expandInternalEntities,
    size_t size ) :
		d ( new SuppressProdnoteData() )
{
	d->buffer.reserve ( size );
	XML_SetUserData ( p, d.get() );

	// parse declaration?
	if ( parseDeclaration )
		XML_SetXmlDeclHandler ( p, xmldeclhandler );

	// internal entities
	if ( expandInternalEntities )
		XML_SetDefaultHandlerExpand ( p, defaulthandler );
	else
		XML_SetDefaultHandler ( p, defaulthandler );

	XML_SetElementHandler ( p, start, end );
	d->level = 100;
	d->skip = false;
}

XmlSuppressProdnote::~XmlSuppressProdnote()
{}

void XMLCALL XmlSuppressProdnote::xmldeclhandler (
    void *data,
    const XML_Char *version,
    const XML_Char *encoding,
    int standalone )
{
	SuppressProdnoteData *d;
	d = ( SuppressProdnoteData * ) data;

	d->encoding = ( encoding ) ? encoding : "UTF-8";

	d->buffer.append ( "<?xml version=\"" );
	d->buffer.append ( version );
	d->buffer.append ( "\" encoding=\"" );
	d->buffer.append ( d->encoding );
	d->buffer.append ( "\"" );

	if ( standalone != -1 )
	{
		d->buffer.append ( " standalone=\"" );
		d->buffer.append ( ( standalone == 1 ) ? "yes" : "no" );
		d->buffer.append ( "\"" );
	}
	d->buffer.append ( "?>" );
}

void XMLCALL XmlSuppressProdnote::defaulthandler (
    void *data,
    const XML_Char *s,
    int len )
{
	SuppressProdnoteData *d;
	d = ( SuppressProdnoteData * ) data;
	if ( ! ( d->skip ) )
	   d->buffer.append ( s, len );
}

void XMLCALL XmlSuppressProdnote::start ( void *data,
                                      const XML_Char *el,
                                      const XML_Char **attr )
{
	SuppressProdnoteData *pd;
	pd = ( SuppressProdnoteData * ) data;

    pd->level += 1;
    
    if ( pd->skip )
        return;

    std::string tag;
    
    if ( !strcmp ( el, "span" ) )
    {
        char **attrPreview = (char **)attr;
        for ( ; *attrPreview; attrPreview +=2 )
        {
            if (
                !strcmp ( *attrPreview, "class") &&
                !strcmp ( * ( attrPreview + 1 ), "optional-prodnote" )
            )
            {
                pd->skip = true;
                pd->level = 1;
                break;
            }
        }
    }
    
    if ( pd->skip )
        return;

    tag += "<";
    tag += el;
    for ( ; *attr; attr += 2 )
    {
        tag += " ";
        tag += *attr;
        tag += "=\"";
        tag += *(attr + 1);
        tag += "\"";
    }
    
    tag += ">";

    pd->buffer += tag;
}

void XMLCALL XmlSuppressProdnote::end ( void *data, const XML_Char *el )
{
	SuppressProdnoteData *pd;
	pd = ( SuppressProdnoteData * ) data;
    
    pd->level -= 1;
    
    if ( !(pd->level) && !strcmp ( el, "span" ) )
    {
        pd->level = 100;
        pd->skip = false;
        return;
    }

    if ( pd->skip )
        return;

    pd->buffer += "</";
    pd->buffer += el;
    pd->buffer += ">";
}
