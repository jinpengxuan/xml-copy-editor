#ifndef REPLACE_H
#define REPLACE_H

#include <string>

class Replace
{
  public:
    static int run(
      std::string& buffer,
      const std::string& find,
      const std::string& replace,
      bool matchCase);
};

#endif
