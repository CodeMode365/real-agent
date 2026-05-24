#include "file.h"

namespace FileUtils
{
    const char *getFileModeString(FileMode mode)
    {
        switch (mode)
        {
        case FileMode::READ:
            return "r";
        case FileMode::WRITE:
            return "w";
        case FileMode::APPEND:
            return "a";
        case FileMode::READ_WRITE:
            return "r+";
        case FileMode::WRITE_READ:
            return "w+";
        case FileMode::APPEND_READ:
            return "a+";
        default:
            return "r";
        }
    }
}
