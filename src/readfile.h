#ifndef READ_FILE_H
#define READ_FILE_H

#include <string>
#include <iostream>
#include <fstream>

class ReadFile
{
  public:
    static bool run(std::string fname, std::string &buffer);
};

#endif
