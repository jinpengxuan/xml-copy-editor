#ifndef XSL_LOCATOR_H
#define XSL_LOCATOR_H

#include <expat.h>
#include <string>
#include "wrapexpat.h"

struct XslLocatorData : public ParserData
{
  std::string xslLocation;
  XML_Parser parser;
};

class XslLocator : public WrapExpat
{
  public:
    XslLocator();
    virtual ~XslLocator();
    std::string getXslLocation();
  private:
    std::auto_ptr<XslLocatorData> d;
    static void XMLCALL starthandler(
      void *data,
      const XML_Char *el,
      const XML_Char **attr);
    static void XMLCALL processingInstructionHandler(
      void *userData,
      const XML_Char *target,
      const XML_Char *data);
};

#endif
