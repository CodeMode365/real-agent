#include <iostream>
#include <cstdlib>
#include <string>
#include "admin.h"
#include "../utils/screen/screen.h"

using namespace std;

Admin::Admin()
{
    username = "admin";
    password = "1234";
}

bool Admin::login()
{
    string u, p;
    cout << "\n--- Admin Login ---\n";
    system("cls");
    cout << "Username: ";
    cin >> u;
    cout << "Password: ";
    cin >> p;
    if (u == username && p == password)
        return true;
    else
        return false;
}
