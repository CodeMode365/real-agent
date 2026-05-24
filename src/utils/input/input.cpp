#include "input.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace Input
{
    string trim(const string &s)
    {
        const char *whitespace = " \t\r\n\v\f";
        const auto start = s.find_first_not_of(whitespace);
        if (start == string::npos)
        {
            return "";
        }
        const auto end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    }

    string sanitize(const string &s)
    {
        string out = trim(s);
        for (char &c : out)
        {
            if (c == '|' || c == '\n' || c == '\r')
            {
                c = ' ';
            }
        }
        return out;
    }

    static string readLine()
    {
        string line;
        if (!getline(cin, line))
        {
            cout << "\n";
            exit(0);
        }
        return line;
    }

    string readString(const string &prompt, const string &defaultValue)
    {
        cout << prompt;
        string line = sanitize(readLine());
        return line.empty() ? defaultValue : line;
    }

    int readInt(const string &prompt, int defaultValue)
    {
        cout << prompt;
        string line = trim(readLine());
        if (line.empty())
        {
            return defaultValue;
        }
        try
        {
            size_t pos = 0;
            const int value = stoi(line, &pos);
            return pos == line.size() ? value : defaultValue;
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    float readFloat(const string &prompt, float defaultValue)
    {
        cout << prompt;
        string line = trim(readLine());
        if (line.empty())
        {
            return defaultValue;
        }
        try
        {
            size_t pos = 0;
            const float value = stof(line, &pos);
            return pos == line.size() ? value : defaultValue;
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    int readChoice(const string &prompt)
    {
        return readInt(prompt, -1);
    }

    void waitForEnter()
    {
        cout << "Press Enter to continue...";
        string discard;
        getline(cin, discard);
    }

    void copyTo(char *dest, size_t destSize, const string &src)
    {
        if (dest == nullptr || destSize == 0)
        {
            return;
        }
        const size_t copyLen = min(src.size(), destSize - 1);
        memcpy(dest, src.data(), copyLen);
        dest[copyLen] = '\0';
    }
}
