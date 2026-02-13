#pragma once
#include <string.h>
//#include <string>
#include <iostream>

using namespace std;


class User
{
protected:
    char name[50];
    char address[70];
    char phone[14];
    char email[70];

public:
    virtual void menu() = 0; // Pure virtual function
    virtual void enterDetails() = 0;
    virtual void viewDetails() = 0;
    virtual ~User() {}
};
