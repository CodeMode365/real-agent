#include <string>

#pragma once

namespace FileUtils
{
    enum class FileMode
    {
        READ,
        WRITE,
        APPEND,
    };

    char *getFileModeString(FileMode mode);

}