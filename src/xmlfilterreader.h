#ifndef XML_FILTER_READER_H
#define XML_FILTER_READER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <expat.h>
#include "wrapexpat.h"

using namespace std;

struct FilterData : public ParserData
{
  vector<string> excludeVector;
  vector<string> includeVector;
  map<string, map<string, set<string> > > filterMap;
  map<string, set<string> > attributeMap;
  set<string> attributeValueSet;
  int filterCount;
  string elementString, attributeKeyString, attributeValueString;
  string includeString, excludeString;
};

class XmlFilterReader : public WrapExpat
{
  public:
    XmlFilterReader();
    virtual ~XmlFilterReader();
    
    enum {
      STATE_UNKNOWN,
      STATE_IN_ELEMENT,
      STATE_IN_ATTRIBUTE,
      STATE_IN_ATTRIBUTE_KEY,
      STATE_IN_ATTRIBUTE_VALUE,
      STATE_IN_EXCLUDE,
      STATE_IN_INCLUDE
    };
    int getFilterCount();
    void getExcludeVector(vector<string> &v);
    void getIncludeVector(vector<string> &v);
    void getFilterMap(map<string, map<string, set<string> > > &m);
  private:
    std::auto_ptr<FilterData> fd;
    static void XMLCALL start(void *data, const XML_Char *el, const XML_Char **attr);
    static void XMLCALL end(void *data, const XML_Char *el);
    static void XMLCALL characterdata(void *data, const XML_Char *s, int len);
};
      
#endif
