#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlschemaparser.h"

XmlSchemaParser::XmlSchemaParser(PromptGeneratorData *data, bool nameSpaceAware) :
  WrapExpat(nameSpaceAware), d(new SchemaParserData)
{
  d->promptData = data;
  d->setState(STATE_UNKNOWN);
  XML_SetUserData(p, d.get());
  XML_SetElementHandler(p, starthandler, endhandler);
}

XmlSchemaParser::~XmlSchemaParser()
{
}

void XMLCALL XmlSchemaParser::starthandler(
  void *data,
  const XML_Char *el,
  const XML_Char **attr)
{
  SchemaParserData *d;
  d = (SchemaParserData *)data;
  if (!strcmp(el, "http://www.w3.org/2001/XMLSchema:complexType"))
  {
    d->setState(STATE_COMPLEX_TYPE); 
  }

  while (*attr)
  {
    attr += 2;
  }
}

void XMLCALL XmlSchemaParser::endhandler(void *data, const XML_Char *el)
{
  return;
}
