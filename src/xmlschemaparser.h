#ifndef XML_SCHEMA_PARSER_H
#define XML_SCHEMA_PARSER_H

#include <expat.h>
#include <string>
#include "wrapexpat.h"
#include "xmlpromptgenerator.h"

struct SchemaParserData : public ParserData
{
  PromptGeneratorData *promptData;
  std::map<std::string, std::set<std::string> > referenceMap;
  std::string currentElement;
};

class XmlSchemaParser : public WrapExpat
{
  public:
    XmlSchemaParser(PromptGeneratorData *data, bool nameSpaceAware);
    virtual ~XmlSchemaParser();
    enum {
      STATE_UNKNOWN,
      STATE_ROOT,
      STATE_ELEMENT,
      STATE_SIMPLE_TYPE,
      STATE_COMPLEX_TYPE,
      STATE_SEQUENCE,
      STATE_CHOICE
    };
  private:
    std::auto_ptr<SchemaParserData> d;
    static void XMLCALL starthandler(
      void *data,
      const XML_Char *el,
      const XML_Char **attr);
    static void XMLCALL endhandler(
      void *data,
      const XML_Char *el);
};

#endif
