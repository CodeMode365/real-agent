#pragma once
#include <string>

using namespace std;

namespace Screen
{
    void clearScreen();
    void invalidInput();
    void printHeader(const string &title);

    void printKeyValue(const string &key, const string &value);
    void printRecordSeparator();
    void beginRecord();
    void endRecord();
}
