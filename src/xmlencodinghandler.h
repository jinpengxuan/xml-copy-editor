#ifndef XML_ENCODING_HANDLER_H
#define XML_ENCODING_HANDLER_H

#include <string>
#include <utility>
#include "casehandler.h"

class XmlEncodingHandler
{
public:
    static std::string get (
            const std::string& utf8 );
    static bool setUtf8 ( std::string& utf8, bool ignoreCurrentEncoding = false );
    static bool set ( std::string& buffer, std::string& encoding );
    static bool hasDeclaration ( const std::string& utf8 );
    static std::pair<int, int> getEncodingValueLimits ( const std::string& utf8 );
};

#endif
