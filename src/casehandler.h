#ifndef CASEHANDLER_H
#define CASEHANDLER_H

#include <string>

using namespace std;

class CaseHandler
{
  public:
    CaseHandler();
    ~CaseHandler();
    static string lowerCase(string &s);
    static void adjustCase(string &buffer, string &comparison);
};

#endif
