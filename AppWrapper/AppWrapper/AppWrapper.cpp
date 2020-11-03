// AppWrapper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "VAClientLib.h"


bool isKeyPress()
{
    bool res = false;
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD sInputRecord;
    DWORD        dwInputEvents;
    DWORD        dwNumberOfEvents;

    if (GetNumberOfConsoleInputEvents(hStdIn, &dwNumberOfEvents)) {
        if (dwNumberOfEvents > 0) {
            ReadConsoleInput(hStdIn, &sInputRecord, 1, &dwInputEvents);
            res = ((sInputRecord.EventType == KEY_EVENT) && (sInputRecord.Event.KeyEvent.bKeyDown));
        }
    }
    return res;
}

int main()
{
    start();
    std::cout << "Application is started." << std::endl;

    std::cout << "Press any key to stop." << std::endl;
    while (!isKeyPress()) {

    }

    stop();
    std::cout << "Application is stopped." << std::endl;
}
