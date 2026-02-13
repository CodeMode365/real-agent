#include <iostream>
#include <cstdlib>
#include <string.h>
#include "../utils/user/user.h"
#include "client.h"
using namespace std;

void Client::enterDetails()
{
    cout << "Enter Client Name: ";
    cin.getline(name, sizeof(name));
    cout << "Enter Client Address: ";
    cin.getline(address, sizeof(address));
    cout << "Enter Client Phone: ";
    cin.getline(phone, sizeof(phone));
    cout << "Enter Client Email: ";
    cin.getline(email, sizeof(email));
}

void Client::viewDetails()
{
    cout << "\nClient Details:\n";
    cout << "Name: " << name << "\n";
    cout << "Address: " << address << "\n";
    cout << "Phone: " << phone << "\n";
    cout << "Email: " << email << "\n";
}

void Client::menu()
{
    int choice;
    cout << "\n";
    cout << "||||||||||||||||||||||||||||||||||||||||||||||\n";
    cout << "||          Client Management Menu          ||\n";
    cout << "||||||||||||||||||||||||||||||||||||||||||||||\n";
    cout << "1. Add Client\n";
    cout << "2. View Client\n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter choice:";
    cin >> choice;
    fflush(stdin);
    switch (choice)
    {
    case 1:
        enterDetails();
        break;
    case 2:
        viewDetails();
        break;
    case 3:
        break;
    }
}
