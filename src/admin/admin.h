#pragma once
#include <string>
using namespace std;

class Admin
{
private:
    string username;
    string password;

public:
    Admin();
    bool login();
};