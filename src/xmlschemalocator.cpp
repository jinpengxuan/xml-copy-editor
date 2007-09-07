#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlschemalocator.h"

XmlSchemaLocator::XmlSchemaLocator() :
  WrapExpat(true), d(new SchemaLocatorData())
{
  d->parser = p;
  XML_SetUserData(p, d.get());
  XML_SetStartElementHandler(p, starthandler);
}

XmlSchemaLocator::~XmlSchemaLocator()
{
}

void XMLCALL XmlSchemaLocator::starthandler(
  void *data,
  const XML_Char *el,
  const XML_Char **attr)
{
  SchemaLocatorData *d;
  d = (SchemaLocatorData *)data;

  while (*attr)
  {
    if (
      !strcmp(*attr, "http://www.w3.org/2001/XMLSchema-instance:noNamespaceSchemaLocation") ||
      !strcmp(*attr, "http://www.w3.org/2001/XMLSchema-instance:schemaLocation"))
    {
      d->schemaLocation = *(attr + 1);
      XML_StopParser(d->parser, false);
    }
    attr += 2;
  }
}

std::string XmlSchemaLocator::getSchemaLocation()
{
  return d->schemaLocation;
}
