#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlencodingspy.h"

XmlEncodingSpy::XmlEncodingSpy() :
  d(new EncodingData())
{
  XML_SetUserData(p, d.get());
  XML_SetXmlDeclHandler(p, xmldeclhandler);
  XML_SetStartElementHandler(p, start);
  d->p = p;
}

XmlEncodingSpy::~XmlEncodingSpy()
{
}

void XMLCALL XmlEncodingSpy::xmldeclhandler(
  void *data, 
  const XML_Char *version, 
  const XML_Char *encoding,
  int standalone)
{
  EncodingData *d;
  d = (EncodingData *)data;
  d->encoding = (encoding) ? encoding : "UTF-8";
}

void XMLCALL XmlEncodingSpy::start(
  void *data,
  const XML_Char *el,
  const XML_Char **attr)
{
  EncodingData *d;
  d = (EncodingData *)data;

  if (!d->encoding.size())
    d->encoding = "UTF-8";

  XML_StopParser(d->p, false);
}
