#ifndef WRAPREGEX_H
#define WRAPREGEX_H

#include <iostream>
#include <string>
#include <vector>
#include <pcre.h>
#include "contexthandler.h"

using namespace std;

class WrapRegex
{
public:
    WrapRegex (
        const string& pattern,
        bool matchCase,
        const string& replaceParameter = "",
        const int arrayLengthParameter = 60 );
    virtual ~WrapRegex();
    string replaceGlobal (
        const string& buffer,
        int *matchCount );
    int matchPatternGlobal (
        string &buffer,
        vector<ContextMatch> &match_vector,
        unsigned elementCount,
        int context = 0 );
private:
    string replace;
    const int arrayLength;
    int returnValue;
    bool disabled;

    pcre *patternStructure;
    pcre_extra *patternExtraStructure;
    int *matchArray;

    string getInterpolatedString_ ( char *buffer, char *source );
    string getSubpattern_ ( char *buffer, unsigned subpattern );
    int matchPatternGlobal_ (
        const char *buffer,
        size_t buflen,
        vector<ContextMatch> &matchVector,
        unsigned elementCount,
        int context );
};

#endif
