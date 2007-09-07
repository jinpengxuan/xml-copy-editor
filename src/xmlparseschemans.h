#ifndef XML_PARSE_SCHEMA_NS_H
#define XML_PARSE_SCHEMA_NS_H

#include <vector>
#include <expat.h>
#include <string>
#include "wrapexpat.h"

struct ParseSchemaNsData : public ParserData
{
  std::vector<std::pair<std::string, std::string> > attributeVector;
  XML_Parser p;
};

class XmlParseSchemaNs : public WrapExpat
{
  public:
    XmlParseSchemaNs();
    virtual ~XmlParseSchemaNs();
    std::vector<std::pair<std::string, std::string> > getAttributeVector() { return d->attributeVector; }
  private:
    std::auto_ptr<ParseSchemaNsData> d;
    static void XMLCALL start(
      void *data,
      const XML_Char *el,
      const XML_Char **attr);
};

#endif
