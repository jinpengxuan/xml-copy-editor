#ifndef HOUSE_STYLE_READER_H
#define HOUSE_STYLE_READER_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <expat.h>
#include "wrapexpat.h"

using namespace std;

struct HouseStyleReaderData : public ParserData
{
  bool filterActive;
  string textnode;
  vector<pair<string, unsigned> > nodevector;
  unsigned depth, cutoffDepth;
  map<string, map<string, set<string> > > filterMap;
};

class HouseStyleReader : public WrapExpat
{
  public:
    HouseStyleReader(
      map<string, map<string, set<string> > > &m
    );
    virtual ~HouseStyleReader();
    void getNodeVector(vector<pair<string, unsigned> > &v);
  private:
    std::auto_ptr<HouseStyleReaderData> ud;
    enum {
      STATE_UNKNOWN,
      STATE_ON_START,
      STATE_ON_END,
      STATE_ON_CDATA_START,
      STATE_ON_CDATA_END
    };

    static void XMLCALL start(void *data, const XML_Char *el, const XML_Char **attr);
    static void XMLCALL end(void *data, const XML_Char *el);
    static void XMLCALL characterdata(void *userData, const XML_Char *s, int len);
    static void XMLCALL cdatastart(void *userData);
    static void XMLCALL cdataend(void *userData);
};
      
#endif
