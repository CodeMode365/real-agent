#include <iostream>
#include <string>
#include "admin.h"
#include "../utils/input/input.h"
#include "../utils/screen/screen.h"

using namespace std;

Admin::Admin()
{
    username = "admin";
    password = "1234";
}

bool Admin::login()
{
    cout << "\n--- Admin Login ---\n";
    string u = Input::readString("Username: ", "");
    string p = Input::readPassword("Password: ", "");
    return u == username && p == password;
}
