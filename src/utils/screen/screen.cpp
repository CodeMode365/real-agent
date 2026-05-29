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
    namespace
    {
        constexpr int RECORD_WIDTH = 50;
        constexpr int KEY_WIDTH = 16;
    }

    void invalidInput()
    {
        usleep(300000);
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void clearScreen()
    {
#if defined(_WIN32) || defined(_WIN64)
        std::system("cls");
#else
        std::system("clear");
#endif
        std::cout << "\033[2J\033[H";
    }

    void printHeader(const std::string &title)
    {
        const int totalWidth = std::max(40, static_cast<int>(title.length()) + 4);
        const int contentWidth = totalWidth - 2;
        const int padding = contentWidth - static_cast<int>(title.length());
        const int leftPad = padding / 2;
        const int rightPad = padding - leftPad;

        std::cout << "+" << std::string(contentWidth, '=') << "+\n";
        std::cout << "|" << std::string(leftPad, ' ') << title
                  << std::string(rightPad, ' ') << "|\n";
        std::cout << "+" << std::string(contentWidth, '=') << "+\n";
    }

    void printRecordSeparator()
    {
        std::cout << std::string(RECORD_WIDTH, '-') << "\n";
    }

    void beginRecord()
    {
        printRecordSeparator();
    }

    void endRecord()
    {
        printRecordSeparator();
    }

    void printKeyValue(const std::string &key, const std::string &value)
    {
        std::cout << std::left << std::setw(KEY_WIDTH)
                  << (key + ":") << value << "\n";
    }
}
