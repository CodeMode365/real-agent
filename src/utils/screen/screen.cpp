#include "screen.h"
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <unistd.h>

using namespace std;

namespace Screen
{
    const int RECORD_WIDTH = 50;
    const int KEY_WIDTH = 16;

    void invalidInput()
    {
        usleep(300000);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void clearScreen()
    {
#if defined(_WIN32) || defined(_WIN64)
        system("cls");
#else
        system("clear");
#endif
        cout << "\033[2J\033[H";
    }

    void printHeader(const string &title)
    {
        // draw the title centered inside a box, at least 40 chars wide
        int totalWidth = max(40, (int)title.length() + 4);
        int contentWidth = totalWidth - 2;
        int padding = contentWidth - (int)title.length();
        int leftPad = padding / 2;
        int rightPad = padding - leftPad; // give the extra space to the right side

        cout << "+" << string(contentWidth, '=') << "+\n";
        cout << "|" << string(leftPad, ' ') << title
             << string(rightPad, ' ') << "|\n";
        cout << "+" << string(contentWidth, '=') << "+\n";
    }

    void printRecordSeparator()
    {
        cout << string(RECORD_WIDTH, '-') << "\n";
    }

    void beginRecord()
    {
        printRecordSeparator();
    }

    void endRecord()
    {
        printRecordSeparator();
    }

    void printKeyValue(const string &key, const string &value)
    {
        cout << left << setw(KEY_WIDTH)
             << (key + ":") << value << "\n";
    }
}
