#ifndef BINARYFILE_H
#define BINARYFILE_H

#include <cstdio>
#include <cstdlib>

class BinaryFile
{
public:
    BinaryFile ( const char *fname );
    ~BinaryFile();
    const char *getData();
    size_t getDataLen();
private:
    char *m_data;
    size_t m_dataLen;
};

#endif
