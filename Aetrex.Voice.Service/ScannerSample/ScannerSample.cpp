#include "ScannerConnector.h"

#include <iostream>


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

int main()
{
    std::cout << "Please input command." << std::endl;
    std::cout << "'g' - send GetStatus request to Client." << std::endl;
    std::cout << "'s' - set audio capture device index." << std::endl;
    std::cout << "'q' - exit." << std::endl;
    std::string line;
    int microphoneIndex = 0;
    ScannerConnector scanner;
    for (TCHAR c; c = getch();) {
        if (c == 's') {
            std::cout << "Please select microphone index (0,1,2,3,...,8,9) and press Enter." << std::endl;
            std::getline(std::cin, line);
            try {
                microphoneIndex = std::stoi(line);
            } catch (const std::exception&) {}

            bool result = scanner.SendScannerToEnova("SetAudioCaptureDeviceIndex", microphoneIndex);
            std::cout << "Request SetAudioCaptureDeviceIndex is sent to the client, result: " << result << std::endl;
        } else if (c == 'g') {
            bool result = scanner.SendScannerToEnova("GetState", microphoneIndex);
            std::cout << "Request GetStatus is sent to the client, result: " << result << std::endl;
        } else if (c == 'q') {
            break;
        } else {
            std::wstring w(&c);
            auto s = std::string(w.begin(), w.end());
            std::cout << "Unknown command: " << s << std::endl;
        }
    }
}
