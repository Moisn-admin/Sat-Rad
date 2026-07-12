#pragma once

#include <FS.h>

namespace satellite
{

struct TleRecord
{
    char name[32];
    char line1[80];
    char line2[80];
};

class TleReader
{
public:
    bool open(fs::FS& fs, const char* filename);
    bool next(TleRecord& record);
    void close();

private:
    File _file;

    bool readLine(char* buffer, size_t len);
};

}