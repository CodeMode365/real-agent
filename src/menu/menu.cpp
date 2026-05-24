#include <iostream>
#include "../admin/admin.h"
#include "../property/property.h"
#include "../utils/input/input.h"
#include "../utils/screen/screen.h"
#include "../clients/client.h"
#include "../seller/seller.h"
#include "menu.h"

using namespace std;

namespace Menu
{
    void runMenu()
    {
        Admin admin;

        while (!admin.login())
        {
            Screen::clearScreen();
            cout << "Invalid login! Try again.\n";
        }
        Screen::clearScreen();

        cout << "Login successful!\n";

        showMenu();
    }

    int showMenu()
    {
        int choice = 0;
        Property property;

        do
        {
            Screen::clearScreen();
            Screen::printHeader("Main Menu");
            cout << "1. Property Menu\n";
            cout << "2. Seller Menu\n";
            cout << "3. Client Menu\n";
            cout << "4. Search Property for Client\n";
            cout << "5. Logout\n";
            choice = Input::readChoice("Enter choice: ");

            switch (choice)
            {
            case 1:
            {
                property.menu();
                break;
            }
            case 2:
            {
                Seller s;
                s.menu();
                break;
            }
            case 3:
            {
                Client c;
                c.menu();
                break;
            }
            case 4:
                property.searchPropertyByRequirements();
                break;
            case 5:
                cout << "Logged out successfully.\n";
                cout << "Program ended.\n";
                return 0;

            default:
                Screen::clearScreen();
                cout << "Invalid choice!\n";
                Input::waitForEnter();
                break;
            }
        } while (true);
    }
}
