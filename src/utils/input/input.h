#pragma once
#include <string>

using namespace std;

namespace Input
{
    const string NA = "N/A";

    string trim(const string &s);
    string sanitize(const string &s);

    string readString(const string &prompt,
                      const string &defaultValue = NA);

    string readPassword(const string &prompt,
                        const string &defaultValue = NA);

    int readInt(const string &prompt, int defaultValue = 0);

    float readFloat(const string &prompt, float defaultValue = 0.0f);

    int readChoice(const string &prompt);

    void waitForEnter();

    void copyTo(char *dest, size_t destSize, const string &src);
}
