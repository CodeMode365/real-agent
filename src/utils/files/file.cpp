#include "file.h"
#include <string>

namespace FileUtils
{

    char *getFileModeString(FileMode mode)
    {
        switch (mode)
        {
        case FileMode::READ:
            return "r";
        case FileMode::WRITE:
            return "w";
        case FileMode::APPEND:
            return "a";
        default:
            return "r";
        }
    }

}