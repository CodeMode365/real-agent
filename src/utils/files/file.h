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

    const char *getFileModeString(FileMode mode);
}
