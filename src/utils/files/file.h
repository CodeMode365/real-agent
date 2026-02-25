#include <string>

#pragma once

namespace FileUtils
{
    enum class FileMode
    {
        READ,
        WRITE,
        APPEND,
        READ_WRITE,
        WRITE_READ,
        APPEND_READ,
    };

    char *getFileModeString(FileMode mode);

}