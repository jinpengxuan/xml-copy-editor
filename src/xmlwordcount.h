#ifndef XML_WORD_COUNT_H
#define XML_WORD_COUNT_H

#include <string>
#include "wrapexpat.h"

struct WordCountData : public ParserData
{
    std::string buffer;
    size_t wordCount;
};

class XmlWordCount : public WrapExpat
{
public:
    XmlWordCount();
    virtual ~XmlWordCount();

    int getWordCount();
private:
    std::auto_ptr<WordCountData> wcd;
    static void XMLCALL characterdata ( void *data, const XML_Char *s, int len );
    static void XMLCALL end ( void *data, const XML_Char *el );
};

#endif
