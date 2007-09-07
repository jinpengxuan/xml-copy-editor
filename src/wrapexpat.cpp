#include "wrapexpat.h"
#include "expat.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

WrapExpat::WrapExpat(bool nameSpaceAware)
{
 p = (nameSpaceAware) ? XML_ParserCreateNS(NULL, (XML_Char)':') : XML_ParserCreate(NULL);
 if (p == 0)
   throw runtime_error("WrapExpat::WrapExpat");
}

WrapExpat::~WrapExpat()
{
  if (p)
    XML_ParserFree(p);                       
}

bool WrapExpat::parse(const char *buffer, size_t size, bool isFinal)
{
  if (!p)
    return false;
  if (XML_Parse(p, buffer, size, isFinal) == XML_STATUS_ERROR)
    return false;
  return true;      
 
}

bool WrapExpat::parse(const string &buffer, bool isFinal)
{
  return parse(buffer.c_str(), buffer.size(), isFinal);

/*
  if (!p)
    return false;
  
  if (XML_Parse(p, buffer.c_str(), buffer.size(), isFinal) == XML_STATUS_ERROR)
    return false;
  return true;      
*/

}

pair<int, int> WrapExpat::getErrorPosition()
{
  return make_pair(
    XML_GetCurrentLineNumber(p),// - 1,
    XML_GetCurrentColumnNumber(p));
}

string WrapExpat::getLastError()
{
  if (!p)
    return "Unable to create parser instance";
    
  stringstream ss;
  ss << "Error at line ";
  ss << XML_GetCurrentLineNumber(p);
  ss << ", column " << XML_GetCurrentColumnNumber(p) + 1 << ":" << endl;
  ss << XML_ErrorString(XML_GetErrorCode(p));
  return ss.str();
}

string WrapExpat::xmliseTextNode(const string &textnode)
{
  size_t size = textnode.size();
  string output;
    
  for (size_t i = 0; i < size; ++i)
  {
    char c = textnode[i];
    switch (c)
    {
      case '<':
        output += "&lt;";
        break;
      case '>':
        output += "&gt;";
        break;
      case '&':
        output += "&amp;";
        break;
      default:
        output += textnode[i];
    }
  }
  return output;
}

string WrapExpat::xmliseAttribute(const string &attribute)
{
  string intermediate = xmliseTextNode(attribute);
  
  size_t size = intermediate.size();
  string output;
    
  for (size_t i = 0; i < size; ++i)
  {
    char c = intermediate[i];
    switch (c)
    {
      case '"':
        output += "&quot;";
        break;
      case '\'':
        output += "&apos;";
        break;
      default:
        output += intermediate[i];
    }
  }
  return output;
}

bool WrapExpat::isWhitespace(const string &textnode)
{
  size_t size = textnode.size();
  for (size_t i = 0; i < size; ++i)
    if (!isspace(textnode[i]))
      return false;
  return true;
}

bool WrapExpat::isEncodingError()
{
  if (!p)
    return false;

  int errorCode = XML_GetErrorCode(p);
  return (
    errorCode == XML_ERROR_UNKNOWN_ENCODING ||
    errorCode == XML_ERROR_INCORRECT_ENCODING) ?
      true : false;
}

