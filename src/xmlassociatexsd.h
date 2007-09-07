#ifndef XML_ASSOCIATE_XSD_H
#define XML_ASSOCIATE_XSD_H

#include <vector>
#include <expat.h>
#include <string>
#include <map>
#include "wrapexpat.h"

struct AssociateXsdData : public ParserData
{
  std::string buffer, path, namespaceAttribute;
  bool rootElementSeen;
  std::map<std::string, std::string> namespaceMap;
};

class XmlAssociateXsd : public WrapExpat
{
  public:
    XmlAssociateXsd(
      const std::string& path = "",
      size_t size = BUFSIZ);
    virtual ~XmlAssociateXsd();
    std::string getBuffer() { return d->buffer; }
  private:
    std::auto_ptr<AssociateXsdData> d;
    std::string encoding;
    std::map<std::string, std::string> namespaceMap;
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
};

#endif
