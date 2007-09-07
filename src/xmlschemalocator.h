#ifndef XML_SCHEMA_LOCATOR_H
#define XML_SCHEMA_LOCATOR_H

#include <expat.h>
#include <string>
#include "wrapexpat.h"

struct SchemaLocatorData
{
  std::string schemaLocation;
  XML_Parser parser;
};

class XmlSchemaLocator : public WrapExpat
{
  public:
    XmlSchemaLocator();
    virtual ~XmlSchemaLocator();
    std::string getSchemaLocation();
  private:
    std::auto_ptr<SchemaLocatorData> d;
    static void XMLCALL starthandler(
      void *data,
      const XML_Char *el,
      const XML_Char **attr);
};

#endif
