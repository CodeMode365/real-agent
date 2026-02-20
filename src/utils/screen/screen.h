#pragma once
#include <string>

namespace Screen
{
    void clearScreen();
    void invalidInput();
    void printHeader(const std::string &title); // ✅ matches cpp
}
