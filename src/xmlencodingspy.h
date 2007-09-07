#ifndef XML_ENCODING_SPY_H
#define XML_ENCODING_SPY_H

#include <expat.h>
#include <string>
#include "wrapexpat.h"

struct EncodingData : public ParserData
{
    std::string encoding;
    XML_Parser p;
};

class XmlEncodingSpy : public WrapExpat
{
public:
    XmlEncodingSpy();
    virtual ~XmlEncodingSpy();
    std::string getEncoding()
    {
        return d->encoding;
    }
private:
    std::auto_ptr<EncodingData> d;
    static void XMLCALL xmldeclhandler (
        void *data,
        const XML_Char *version,
        const XML_Char *encoding,
        int standalone );
    static void XMLCALL start (
        void *data,
        const XML_Char *el,
        const XML_Char **attr );
};

#endif
