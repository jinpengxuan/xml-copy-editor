#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include "xmlassociatedtd.h"

XmlAssociateDtd::XmlAssociateDtd(
  const std::string& path,
  const std::string& publicIdentifier,
  size_t size) :
  d(new DtdData())
{
  d->buffer.reserve(size);
  d->path = path;
  d->publicIdentifier = publicIdentifier;
  d->rootElementSeen = false;
  d->insideDtd = false;
  XML_SetUserData(p, d.get());
  XML_SetElementHandler(p, start, end);
  XML_SetDoctypeDeclHandler(p, startdoctypehandler, enddoctypehandler);
  XML_SetDefaultHandlerExpand(p, defaulthandler);
}

XmlAssociateDtd::~XmlAssociateDtd()
{
}

void XMLCALL XmlAssociateDtd::defaulthandler(
  void *data,
  const XML_Char *s,
  int len)
{
  DtdData *d;
  d = (DtdData *)data;
  if (!d->insideDtd)
    d->buffer.append(s, len);
}

void XMLCALL XmlAssociateDtd::start(void *data,
  const XML_Char *el,
  const XML_Char **attr)
{
  DtdData *d;
  d = (DtdData *)data;
  
  if (!d->rootElementSeen)
  {
    if (d->publicIdentifier.empty())
    {
      d->buffer += "<!DOCTYPE ";
      d->buffer += el;
      d->buffer += " SYSTEM \"";
      d->buffer += d->path;
      d->buffer += "\">\n";
    }
    else
    {
      d->buffer += "<!DOCTYPE ";
      d->buffer += el;
      d->buffer += " PUBLIC \"";
      d->buffer += d->publicIdentifier;
      d->buffer += "\" \"";
      d->buffer += d->path;
      d->buffer += "\">\n";
    }
    d->rootElementSeen = true; 
  }

  d->buffer += "<";
  d->buffer += el;

  while (*attr)
  {
    d->buffer += " ";
    d->buffer += *attr;
    d->buffer += "=\"";
    d->buffer += xmliseAttribute(*(attr + 1));
    d->buffer += "\"";
    attr += 2;
  }
  d->buffer += ">";
}

void XMLCALL XmlAssociateDtd::end(void *data, const XML_Char *el)
{
  DtdData *d;
  d = (DtdData *)data;
  d->buffer += "</";
  d->buffer += el;
  d->buffer += ">";
}

void XMLCALL XmlAssociateDtd::startdoctypehandler(
  void *data,
  const XML_Char *doctypeName,
  const XML_Char *sysid,
  const XML_Char *pubid,
  int has_internal_subset)
{
  DtdData *d;
  d = (DtdData *)data;
  d->insideDtd = true;    
}

void XMLCALL XmlAssociateDtd::enddoctypehandler(void *data)
{
  DtdData *d;
  d = (DtdData *)data;
  d->insideDtd = false;
}
