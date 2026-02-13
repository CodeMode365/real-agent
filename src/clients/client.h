#include "../utils/user/user.h"
#pragma once

class Client : public User
{

public:
    void menu();
    void enterDetails();
    void viewDetails();
    ~Client() {}
};
