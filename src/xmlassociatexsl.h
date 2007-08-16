#ifndef XML_ASSOCIATE_XSL_H
#define XML_ASSOCIATE_XSL_H

#include <vector>
#include <expat.h>
#include <string>
#include "wrapexpat.h"

struct XslData : public ParserData
{
  std::string buffer, path;
  bool rootElementSeen;
};

class XmlAssociateXsl : public WrapExpat
{
  public:
    XmlAssociateXsl(
      const std::string& path = "",
      size_t size = BUFSIZ);
    virtual ~XmlAssociateXsl();
    std::string getBuffer() { return d->buffer; }
  private:
    std::auto_ptr<XslData> d;
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
    static void XMLCALL processinghandler(
      void *data,
      const XML_Char *target,
      const XML_Char *datastring);
};

#endif
