#ifndef XML_ASSOCIATE_DTD_H
#define XML_ASSOCIATE_DTD_H

#include <vector>
#include <expat.h>
#include <string>
#include "wrapexpat.h"

struct DtdData : public ParserData
{
  std::string buffer, path, publicIdentifier;
  bool rootElementSeen, insideDtd;
};

class XmlAssociateDtd : public WrapExpat
{
  public:
    XmlAssociateDtd(
      const std::string& path = "",
      const std::string& publicIdentifier = "",
      size_t size = BUFSIZ);
    virtual ~XmlAssociateDtd();
    std::string getBuffer() { return d->buffer; }
  private:
    std::auto_ptr<DtdData> d;
    std::string encoding;
    static void XMLCALL start(
      void *data,
      const XML_Char *el,
      const XML_Char **attr);
    static void XMLCALL end(
      void *data,
      const XML_Char *el);
    static void XMLCALL defaulthandler(
      void *data,
      const XML_Char *s,
      int len);
    static void XMLCALL startdoctypehandler(
      void *userData,
      const XML_Char *doctypeName,
      const XML_Char *sysid,
      const XML_Char *pubid,
      int has_internal_subset);
    static void XMLCALL enddoctypehandler(void *userData);  
};

#endif
