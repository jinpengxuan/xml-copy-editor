#ifndef GET_WORD_H
#define GET_WORD_H

#include <cstdlib>

class GetWord
{
  public:
    static char *run(char **s, size_t *len);
  private:
    static bool isWordCharacter(char *s, size_t *bytes);
};

#endif
