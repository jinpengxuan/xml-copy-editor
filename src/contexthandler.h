#ifndef CONTEXT_HANDLER
#define CONTEXT_HANDLER

#include <string>

using namespace std;

class ContextMatch
{
public:
    string prelog;
    string match;
    string postlog;
    string replace;
    string report;
    size_t elementCount;
    int offset;
    bool tentative;
};

class ContextHandler
{
public:
    static ContextMatch getContext (
        char *s,
        int len,
        char *origin,
        int context );
};

#endif
