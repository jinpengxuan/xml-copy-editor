#ifndef RULE_H
#define RULE_H

#include <string>
#include "wrapregex.h"

using namespace std;

class Rule : public WrapRegex
{
public:
    Rule (
        const string& pattern,
        bool matchCase,
        const string& replace = "",
        const int arrayLength = 60 );
    bool getAdjustCaseAttribute();
    bool getTentativeAttribute();
    string getReport();
    void setReport ( string &s );
    void setAdjustCaseAttribute ( bool b );
    void setTentativeAttribute ( bool b );
private:
    bool adjustCaseAttribute, tentativeAttribute;
    string reportString;
};

#endif
