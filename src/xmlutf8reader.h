#ifndef XML_UTF_READER_H
#define XML_UTF_READER_H

#include <vector>
#include <expat.h>
#include <string>
#include "wrapexpat.h"

struct UtfData : public ParserData
{
  std::string buffer;
  std::string encoding;
};

class XmlUtf8Reader : public WrapExpat
{
  public:
    XmlUtf8Reader(
      bool parseDeclaration = false,
      bool expandInternalEntities = true,
      size_t size = BUFSIZ
  );
    virtual ~XmlUtf8Reader();
    std::string getBuffer() { return d->buffer; }
    std::string getEncoding() { return d->encoding; }
  private:
    std::auto_ptr<UtfData> d;
    std::string encoding;
    static void XMLCALL xmldeclhandler(
      void *data, 
      const XML_Char *version, 
      const XML_Char *encoding,
      int standalone);
  static void XMLCALL defaulthandler(
      void *data,
      const XML_Char *s,
      int len);         
};

#endif
