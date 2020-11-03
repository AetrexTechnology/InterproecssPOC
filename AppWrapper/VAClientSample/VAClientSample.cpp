#include "ScannerCommunicationTest.h"

#include <windows.h>
#include <iostream>
#include <map>


TCHAR getch() {
    DWORD mode, cc;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

    if (h == NULL) {
        return 0;
    }

    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    TCHAR c = 0;
    ReadConsole(h, &c, 1, &cc, NULL);
    SetConsoleMode(h, mode);
    return c;
}

int main(int argc, char* argv[]) {
    ScannerCommunicationTest scannerCom;
    std::cout << "Please select action." << std::endl;
    std::cout << "'s' - send Command to Scanner." << std::endl;
    std::cout << "'q' - exit." << std::endl;

    for (TCHAR c; c = getch();) {
        if (c == 's') {
            scannerCom.SendStateToScanner();
        } else if (c == 'q') {
            break;
        } else {
            std::wstring w(&c);
            auto s = std::string(w.begin(), w.end());
            std::cout << "Unknown action: " << s << std::endl;
        }
    }

    return 0;
}
