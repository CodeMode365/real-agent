#pragma once
#include <string>

namespace Input
{
    const std::string NA = "N/A";

    std::string trim(const std::string &s);
    std::string sanitize(const std::string &s);

    std::string readString(const std::string &prompt,
                           const std::string &defaultValue = NA);

    int readInt(const std::string &prompt, int defaultValue = 0);

    float readFloat(const std::string &prompt, float defaultValue = 0.0f);

    int readChoice(const std::string &prompt);

    void waitForEnter();

    void copyTo(char *dest, std::size_t destSize, const std::string &src);
}
