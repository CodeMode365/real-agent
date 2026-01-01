#include <iostream>
#include <cstdlib>
#include <string>
#include "../utils/user/user.h"
using namespace std;

class Client : public User
{
public:
    void menu()
    {
        cout << "\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║     Client Management Menu             ║\n";
        cout << "╚════════════════════════════════════════╝\n";
        cout << "1. Add Client\n";
        cout << "2. View Client\n";
        cout << "3. Back to Main Menu\n";
    }

    void enterDetails()
    {
        cout << "Enter Client Name: ";
        getline(cin, name);
        cout << "Enter Client Address: ";
        getline(cin, address);
        cout << "Enter Client Phone: ";
        getline(cin, phone);
        cout << "Enter Client Email: ";
        getline(cin, email);
    }

    void viewDetails()
    {
        cout << "\nClient Details:\n";
        cout << "Name: " << name << "\n";
        cout << "Address: " << address << "\n";
        cout << "Phone: " << phone << "\n";
        cout << "Email: " << email << "\n";
    }

    ~Client() {}
};
