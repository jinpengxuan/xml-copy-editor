#include <sstream>
#include "readfile.h"

bool ReadFile::run ( std::string fname, std::string &buffer )
{
    std::ifstream ifs ( fname.c_str(), std::ios::binary|std::ios::in );
    if ( !ifs.is_open() )
        return false;

    ifs.seekg ( 0, std::ios::end );
    size_t size = ifs.tellg();
    ifs.seekg ( 0, std::ios::beg );

    buffer.reserve ( size + 1 );

    std::stringstream iss;
    iss << ifs.rdbuf();
    buffer = iss.str();

    return true;
}
