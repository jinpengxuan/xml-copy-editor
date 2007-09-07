#include "wrapxerces.h"
#define XERCES_TMPLSINC 

//#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <sstream>
#include <utility>
#include <stdexcept>

using namespace xercesc;

WrapXerces::WrapXerces()
{
  try {
    XMLPlatformUtils::Initialize();
  }
  catch (XMLException& e)
  {
    throw std::runtime_error("Cannot initialize Xerces");
  }
  errorPosition = std::make_pair(1, 1);
}

WrapXerces::~WrapXerces()
{
  XMLPlatformUtils::Terminate();
}

bool WrapXerces::validate(const std::string& fileName)
{
  SAX2XMLReader *parser = XMLReaderFactory::createXMLReader();

  parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  parser->setFeature(XMLUni::fgXercesDynamic, false);
  parser->setFeature(XMLUni::fgXercesSchema, true);
  parser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
  parser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

  DefaultHandler handler;
  MySAX2Handler mySAX2Handler;
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&mySAX2Handler);
  parser->setEntityResolver(&handler);

  try {
    parser->parse(fileName.c_str());
  }
  catch (XMLException& e)
  {
    delete parser;
    char *err = XMLString::transcode(e.getMessage());
    lastError = err;
    XMLString::release(&err);
    return false;
  }
  catch (SAXParseException& e)
  {
    delete parser;
    char *err = XMLString::transcode(e.getMessage());
    std::stringstream ss;
    ss << "Validation stopped at line " << e.getLineNumber() << ", column " << e.getColumnNumber() << ": " << err;
    lastError = ss.str();
    errorPosition = std::make_pair(e.getLineNumber(), e.getColumnNumber());
    XMLString::release(&err);
    return false;
  }
  catch (...)
  {
    delete parser;
    lastError = "Unexpected validation error";
    return false;
  }
  delete parser;
  return true;
}

std::string WrapXerces::getLastError()
{
  return lastError; 
}

std::pair<int, int> WrapXerces::getErrorPosition()
{
  return errorPosition;
}
