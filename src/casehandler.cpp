#include <string>
#include "casehandler.h"

CaseHandler::CaseHandler()
{ }

CaseHandler::~CaseHandler()
{ }

void CaseHandler::adjustCase ( string &buffer, string &comparison )
{
    string::iterator it;
    int countBuffer;
    bool initialCapital;

    countBuffer = 0;
    initialCapital = false;

    // preserve all caps
    for ( it = buffer.begin(); it != buffer.end(); ++it )
        if ( isalpha ( *it ) && isupper ( *it ) )
            ++countBuffer;

    if ( countBuffer > 2 )
    {
        for ( it = buffer.begin(); it != buffer.end(); ++it )
            if ( isalpha ( *it ) )
                *it = toupper ( *it );
        return;
    }

    // preserve initial capital
    it = comparison.begin();
    if ( isalpha ( *it ) && isupper ( *it ) )
        initialCapital = true;

    for ( ++it; it != comparison.end(); ++it )
        if ( isalpha ( *it ) && isupper ( *it ) )
            return;

    if ( initialCapital )
    {
        it = buffer.begin();
        if ( isalpha ( *it ) )
            *it = toupper ( *it );
        for ( ++it; it != buffer.end(); ++it )
            if ( isalpha ( *it ) && isupper ( *it ) )
                *it = tolower ( *it );
    }
}

string CaseHandler::lowerCase ( string &s )
{
    string::iterator it;
    string localString ( s );
    for (
        it = localString.begin();
        it != localString.end();
        it++ )
    {
        if ( *it < 0 )
            continue;
        *it = tolower ( *it );
    }
    return localString;
}
