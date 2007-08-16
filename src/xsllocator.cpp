#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xsllocator.h"

XslLocator::XslLocator() : d(new XslLocatorData())
{
  d->parser = p;
  XML_SetUserData(p, d.get());
  XML_SetProcessingInstructionHandler(p, processingInstructionHandler);
}

XslLocator::~XslLocator()
{
}

void XMLCALL XslLocator::processingInstructionHandler(
  void *userData,
  const XML_Char *target,
  const XML_Char *data)
{
  XslLocatorData *d;
  d = (XslLocatorData *)userData;

  if (strcmp(target, "xml-stylesheet"))
    return;

  char *value, *iterator;
  value = strstr((const char *)data, "href=");
  if (!value || strlen(value) < 7)
    return;
  value += 6;
  iterator = value;
  for (iterator = value; *iterator; ++iterator)
  {
    if (*iterator == '"' || *iterator == '\'')
    {
      *iterator = '\0';
      break;
    }
  }

  d->xslLocation = value;
  XML_StopParser(d->parser, false);
}


std::string XslLocator::getXslLocation()
{
  return d->xslLocation;
}
