#include "getword.h"

char *GetWord::run ( char **s, size_t *len )
{
    size_t bytes;
    char *t, *u;
    int offset;

    t = *s;

    while ( *t )
    {
        if ( GetWord::isWordCharacter ( t, &bytes ) )
        {
            for ( u = t; *u; )
            {
                if ( !GetWord::isWordCharacter ( u, &bytes ) )
                {
                    *len = u - t;
                    offset = t - *s;
                    *s += *len + offset + bytes;
                    return t;
                }
                else
                    u += bytes;
            }
            if ( *t && ! ( *u ) )
            {
                *len = u - t;
                offset = t - *s;
                *s = u;
                return t;
            }
            t = u;
        }
        else
            t += bytes;
    }
    return NULL;
}

bool GetWord::isWordCharacter ( char *s, size_t *bytes )
{
    *bytes = 0;

    unsigned char *us = ( unsigned char * ) s;
    if (
        *us < 65 ||
        ( *us > 90 && *us < 97 ) ||
        ( *us > 123 && *us < 128 )
    )
    {
        *bytes = 1;
        return false;
    }

    // check for UTF-8 byte sequences
    else if ( *us > 127 )
    {
        // determine length
        unsigned char *it = us;
        for ( ; *it > 127; ++it )
            ++ ( *bytes );

        // Unicode punctuation marks
        // Based on http://www1.tip.nl/~t876506/utf8tbl.html
        return (
                   *us == 226 && * ( us + 1 ) == 128 ||
                   *us == 194 ||
                   *us == 203
               ) ? false : true;
    }
    else
    {
        *bytes = 1;
        return true;
    }
}
