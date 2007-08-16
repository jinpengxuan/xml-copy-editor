#ifndef WRAP_EXPAT
#define WRAP_EXPAT

#include <expat.h>
#include <string>
#include <vector>
#include <utility>
#include "parserdata.h"

using namespace std;

class WrapExpat
{
  public:
        WrapExpat(bool nameSpaceAware = false);
        virtual ~WrapExpat();
        bool parse(const string &buffer, bool isFinal = true);
        bool parse(const char *buffer, size_t size, bool isFinal = true);
        pair<int, int> getErrorPosition();
        string getLastError();
        bool isEncodingError();
        static string xmliseTextNode(const string &textnode);
        static string xmliseAttribute(const string &attribute);
        static bool isWhitespace(const string &textnode);
  protected:
        XML_Parser p;
};

#endif
