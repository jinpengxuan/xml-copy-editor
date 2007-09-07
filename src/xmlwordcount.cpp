#include <string>
#include <stdexcept>
#include "xmlwordcount.h"
#include "getword.h"

XmlWordCount::XmlWordCount() : wcd ( new WordCountData() )
{
    wcd->wordCount = 0;
    XML_SetUserData ( p, wcd.get() );
    XML_SetCharacterDataHandler ( p, characterdata );
    XML_SetEndElementHandler ( p, end );
}

XmlWordCount::~XmlWordCount()
{}

int XmlWordCount::getWordCount()
{
    return wcd->wordCount;
}

void XMLCALL XmlWordCount::characterdata (
    void *data,
    const XML_Char *s,
    int len )
{
    WordCountData *wcd;
    wcd = ( WordCountData * ) data;
    wcd->buffer.append ( s, len );
}

void XMLCALL XmlWordCount::end ( void *data, const XML_Char *el )
{
    WordCountData *wcd;
    wcd = ( WordCountData * ) data;

    char *s = ( char * ) wcd->buffer.c_str();
    size_t len, myCount;
    myCount = 0;
    while ( GetWord::run ( &s, &len ) )
        ++myCount;
    wcd->wordCount += myCount;
    wcd->buffer = "";
}
