#include "tle_reader.h"

namespace satellite
{

bool TleReader::open(fs::FS& fs, const char* filename)
{
    close();
    _file = fs.open(filename, "r");
    return _file;
}

void TleReader::close()
{
    if (_file)
        _file.close();
}

bool TleReader::readLine(char* buffer, size_t len)
{
    if (!_file)
        return false;

    size_t i = 0;

    while (_file.available())
    {
        char c = _file.read();

        if (c == '\r')
            continue;

        if (c == '\n')
            break;

        if (i < len - 1)
            buffer[i++] = c;
    }

    buffer[i] = '\0';

    return i > 0;
}

bool TleReader::next(TleRecord& record)
{
    if (!readLine(record.name, sizeof(record.name)))
        return false;

    if (!readLine(record.line1, sizeof(record.line1)))
        return false;

    if (!readLine(record.line2, sizeof(record.line2)))
        return false;

    return true;
}

}