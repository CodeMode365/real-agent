#include "screen.h"
#include <iostream>
#include <cstdlib>



namespace Screen
{
       void clearScreen()
        {
#if defined(_WIN32)|| defined(_WIN64)
                system("cls");
#else
                system("clear");
#endif
                std::cout << "\033[2J\033[H";	
        }

        void printHeader(const std::string &title)
        {
                clearScreen();
                int totalWidth = std::max(40, static_cast<int>(title.length()) + 4); // 2 for borders, 2 for padding
                int contentWidth = totalWidth - 2;                                   // exclude borders
                int padding = contentWidth - static_cast<int>(title.length());
                int leftPad = padding / 2;
                int rightPad = padding - leftPad;
                std::cout << "╔" << std::string(contentWidth, '=') << "╗\n";
                std::cout << "║" << std::string(leftPad, ' ') << title << std::string(rightPad, ' ') << "║\n";
                std::cout << "╚" << std::string(contentWidth, '=') << "╝\n";

                // std::cout << "╔" << std::string(contentWidth, '═') << "╗\n";
                // std::cout << "║" << std::string(leftPad, ' ') << title << std::string(rightPad, ' ') << "║\n";
                // std::cout << "╚" << std::string(contentWidth, '═') << "╝\n";
        }
}
