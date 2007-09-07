#ifndef HOUSE_STYLE_WRITER_H
#define HOUSE_STYLE_WRITER_H

#include <set>
#include <string>
#include <expat.h>
#include "wrapexpat.h"
#include "wrapregex.h"
#include "contexthandler.h"

using namespace std;

struct HouseStyleWriterData : public ParserData
{
    set<size_t> elementSet;
    string output, textnode;
    vector<ContextMatch> v;
};

class HouseStyleWriter : public WrapExpat
{
public:
    HouseStyleWriter ( vector<ContextMatch> &v );
    ~HouseStyleWriter();
    string getOutput();
private:
    std::auto_ptr<HouseStyleWriterData> hswd;
    enum {
        STATE_UNKNOWN,
        STATE_ON_START,
        STATE_ON_END,
        STATE_ON_CDATA_START,
        STATE_ON_CDATA_END
    };
    static void XMLCALL start ( void *data, const XML_Char *el, const XML_Char **attr );
    static void XMLCALL end ( void *data, const XML_Char *el );
    static void XMLCALL characterdata ( void *userData, const XML_Char *s, int len );
    static void XMLCALL xmldecl ( void *userData, const XML_Char *version, const XML_Char *encoding, int standalone );
    static void XMLCALL cdatastart ( void *userData );
    static void XMLCALL cdataend ( void *userData );
    static void XMLCALL defaulthandler ( void *userData, const XML_Char *s, int len );
    static void modify ( vector<ContextMatch> &v, string &buffer, unsigned elementCount );
};


#endif
