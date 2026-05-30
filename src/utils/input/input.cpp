#include "input.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <termios.h>

using namespace std;

namespace Input
{
    string trim(const string &s)
    {
        const char *whitespace = " \t\r\n\v\f";
        size_t start = s.find_first_not_of(whitespace);
        if (start == string::npos)
        {
            return "";
        }
        size_t end = s.find_last_not_of(whitespace);
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

    // reads a password without showing it - prints a * for each key instead.
    // this only works on a real terminal, so if input is piped (tests) just read a line.
    string readPassword(const string &prompt, const string &defaultValue)
    {
        cout << prompt;
        cout.flush();

        if (!isatty(STDIN_FILENO))
        {
            string line = sanitize(readLine());
            return line.empty() ? defaultValue : line;
        }

        termios oldTerm{};
        if (tcgetattr(STDIN_FILENO, &oldTerm) != 0)
        {
            string line = sanitize(readLine());
            return line.empty() ? defaultValue : line;
        }
        termios newTerm = oldTerm;
        newTerm.c_lflag &= ~(ECHO | ICANON); // turn off echo + line mode so we get keys one at a time
        newTerm.c_cc[VMIN] = 1;
        newTerm.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

        string buffer;
        char ch = 0;
        while (read(STDIN_FILENO, &ch, 1) == 1)
        {
            if (ch == '\n' || ch == '\r')
            {
                break;
            }
            if (ch == 127 || ch == 8) // backspace / delete
            {
                if (!buffer.empty())
                {
                    buffer.pop_back();
                    cout << "\b \b"; // wipe the last * off the screen
                    cout.flush();
                }
                continue;
            }
            if (ch == 3) // Ctrl-C
            {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
                cout << "\n";
                exit(0);
            }
            if (ch < 32) // ignore other control chars
            {
                continue;
            }
            buffer.push_back(ch);
            cout << '*';
            cout.flush();
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
        cout << "\n";

        string line = sanitize(buffer);
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
            int value = stoi(line, &pos);
            // pos must reach the end, otherwise stuff like "12abc" would sneak through
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
            float value = stof(line, &pos);
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
        strncpy(dest, src.c_str(), destSize - 1);
        dest[destSize - 1] = '\0';
    }
}
