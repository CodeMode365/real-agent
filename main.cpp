//fork test
#include <iostream>
#include <cstdlib>
#include "src/menu/menu.h"
#include "src/utils/screen/screen.h"

using namespace std;

// ================= MAIN FUNCTION =================
int main()
{
  Screen::printHeader("REAL ESTATE MANAGEMENT SYSTEM");

  Menu::runMenu();

  return 0;
}
