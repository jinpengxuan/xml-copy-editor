#ifndef WRAP_XERCES
#define WRAP_XERCES
#define XERCES_TMPLSINC 

#include <string>
#include <utility>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

using namespace xercesc;

class WrapXerces
{
  public:
    WrapXerces();
    ~WrapXerces();
    bool validate(const std::string& fileName);
    std::string getLastError();
    std::pair<int, int> getErrorPosition();
  private:
    std::string lastError;
    std::pair<int, int> errorPosition;
};

class MySAX2Handler : public DefaultHandler
{
  public:
    void error(const SAXParseException& e) { throw e; }
    void warning(const SAXParseException& e) { throw e; }
};

#endif
