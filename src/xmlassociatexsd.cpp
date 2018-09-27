/*
 * Copyright 2005-2007 Gerald Schmidt.
 *
 * This file is part of Xml Copy Editor.
 *
 * Xml Copy Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Xml Copy Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xml Copy Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <string>
#include <vector>
#include <stdexcept>
#include <expat.h>
#include <cstring>
#include "xmlassociatexsd.h"
#include "xmlparseschemans.h"
#include "readfile.h"
#include "replace.h"

XmlAssociateXsd::XmlAssociateXsd(const wxString &path, const char *encoding,
                                 size_t size)
    : WrapExpat(encoding), d(new AssociateXsdData()) {
  d->buffer.reserve(size);
  d->path = path;
  d->depth = 0;
  XML_SetElementHandler(p, start, end);
  XML_SetDefaultHandlerExpand(p, defaulthandler);
  XML_SetUserData(p, this);

  std::auto_ptr< XmlParseSchemaNs > parser(new XmlParseSchemaNs());
  std::string normalisedPath, buffer;
  normalisedPath = path.mb_str(wxConvLocal);
  if (!ReadFile::run(normalisedPath, buffer))
    return;
  parser->parse(buffer);
  std::vector< std::pair< std::string, std::string > > attributeVector;
  attributeVector = parser->getAttributeVector();
  std::vector< std::pair< std::string, std::string > >::iterator it;
  for (it = attributeVector.begin(); it != attributeVector.end(); ++it) {
    if (it->first == "targetNamespace") {
      namespaceMap.insert(make_pair("xmlns", it->second));
    } else if (it->second == "http://www.w3.org/2001/XMLSchema") {
      ; // skip
    } else if (it->first.find(':') != std::string::npos) {
      it->first.erase(0, it->first.find(':'));
      it->first.insert(0, "xmlns");
      namespaceMap.insert(make_pair(it->first, it->second));
    }
  }
  d->namespaceMap = namespaceMap;
}

XmlAssociateXsd::~XmlAssociateXsd() {}

void XMLCALL XmlAssociateXsd::defaulthandler(void *data, const XML_Char *s,
                                             int len) {
  if (!data || !s)
    return;
  XmlAssociateXsd *pThis = (XmlAssociateXsd *)data;
  AssociateXsdData *d = pThis->d.get();
  if (d)
    d->buffer.append(s, len);
}

void XMLCALL XmlAssociateXsd::start(void *data, const XML_Char *el,
                                    const XML_Char **attr) {
  if (!data)
    return;

  XmlAssociateXsd *pThis = (XmlAssociateXsd *)data;
  AssociateXsdData *d = pThis->d.get();

  d->depth++;
  if (d->depth > 1) {
    XML_DefaultCurrent(pThis->p);
    return;
  }

  d->buffer += "<";
  d->buffer += el;

  while (*attr) {
    if (strstr(*attr, "xmlns:") || !strcmp(*attr, "xmlns") ||
        !strcmp(*attr, "xsi:noNamespaceSchemaLocation") ||
        !strcmp(*attr, "xsi:schemaLocation")) {
    } else {
      d->buffer += " ";
      d->buffer += *attr;
      d->buffer += "=\"";
      d->buffer += xmliseAttribute(*(attr + 1));
      d->buffer += "\"";
    }
    attr += 2;
  }

  d->buffer += " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
  std::map< std::string, std::string >::iterator it;
  for (it = d->namespaceMap.begin(); it != d->namespaceMap.end(); ++it) {
    d->buffer += " ";
    d->buffer += it->first;
    d->buffer += "=\"";
    d->buffer += it->second;
    d->buffer += "\"";
  }

  d->buffer += " xsi:";
  bool withNamespace = (d->namespaceMap.find("xmlns") != d->namespaceMap.end());
  d->buffer += (withNamespace) ? "schemaLocation" : "noNamespaceSchemaLocation";
  d->buffer += "=\"";
  if (withNamespace) {
    d->buffer += d->namespaceMap["xmlns"];
    d->buffer += " ";
  }
  d->buffer += d->path.utf8_str(); // TODO: Apply the encoding of the parser
  d->buffer += "\"";

  d->buffer += ">";
}

void XMLCALL XmlAssociateXsd::end(void *data, const XML_Char *el) {
  if (!data)
    return;
  XmlAssociateXsd *pThis = (XmlAssociateXsd *)data;
  AssociateXsdData *d = pThis->d.get();
  d->depth--;
  if (d->depth)
    XML_DefaultCurrent(pThis->p);
  else {
    d->buffer += "</";
    d->buffer += el;
    d->buffer += ">";
  }
}
