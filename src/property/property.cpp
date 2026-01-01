#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "property.h"
#include "../utils/screen/screen.h"

using namespace std;

Property::Property()
{
    cout << "Property menu selected!\n";
}

void Property::menu()
{
    Screen::clearScreen();
    Screen::printHeader("Property Menu");
    cout << "1. Add Property\n";
    cout << "2. View Properties\n";
    cout << "3. Back to Main Menu\n";
}