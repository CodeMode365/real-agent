#include "screen.h"
#include <iostream>
#include <cstdlib>

namespace Screen
{
        void clearScreen()
        {
#ifdef _WIN32
                system("cls");
#else
                system("clear");
#endif
                std::cout << "\033[2J\033[H";
        }

        void printHeader(const std::string &title)
        {
                clearScreen();
                std::cout << "╔════════════════════════════════════════╗\n";
                int totalWidth = 40;
                int contentWidth = totalWidth - 2; // exclude borders
                int padding = contentWidth - title.length();
                int leftPad = padding / 2;
                int rightPad = padding - leftPad;
                std::cout << "║" << std::string(leftPad, ' ') << title << std::string(rightPad, ' ') << "║\n";
                std::cout << "╚════════════════════════════════════════╝\n";
        }
}
