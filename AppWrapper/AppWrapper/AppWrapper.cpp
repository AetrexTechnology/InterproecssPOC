#include <iostream>
#include <Windows.h>
#include "ScannerCommunication.h"
#include "VAClientLib.h"


class AppWrapperSample {
public:
    AppWrapperSample();

    void launch();
    TCHAR getch();
private:
    void getStateCallback();
    void changeMicrophoneCallback(int microphoneIndex);

    bool SendCommandCallbck(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId);
    bool SendStateCallbck(const char* state);

private:
    std::unique_ptr<ScannerCommunication> mScannerCommunication;
};

AppWrapperSample::AppWrapperSample() {
    std::function<void(void)> getStateCallback = std::bind(&AppWrapperSample::getStateCallback, this);
    std::function<void(int)> changeMicrophoneCallback = std::bind(&AppWrapperSample::changeMicrophoneCallback, this, std::placeholders::_1);
    mScannerCommunication = std::make_unique<ScannerCommunication>();
    mScannerCommunication->SetGetStateCallback(getStateCallback);
    mScannerCommunication->SetChangeMicrophoneCallback(changeMicrophoneCallback);

    start();
    std::function<bool(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId)> sendCommandCallbck =
        std::bind(&AppWrapperSample::SendCommandCallbck, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
    std::function<bool(const char* state)> sendStateCallbck = std::bind(&AppWrapperSample::SendStateCallbck, this, std::placeholders::_1);
    SetSendCommandCallback(sendCommandCallbck);
    SetSendStateCallback(sendStateCallbck);
}

void AppWrapperSample::getStateCallback() {
    GetState();
}

void AppWrapperSample::changeMicrophoneCallback(int microphoneIndex) {
    ChangeMicrophone(microphoneIndex);
}

bool AppWrapperSample::SendCommandCallbck(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId) {
    return mScannerCommunication->Command(state, command, sttUtterance, answer, intentId, answerId);
}
bool AppWrapperSample::SendStateCallbck(const char* state) {
    return mScannerCommunication->State(state);
}

TCHAR AppWrapperSample::getch() {
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

void AppWrapperSample::launch() {
    std::cout << "Please select action." << std::endl;
    std::cout << "'s' - send Command to Scanner." << std::endl;
    std::cout << "'q' - exit." << std::endl;

    for (TCHAR c; c = getch();) {
        if (c == 's') {
            TestSendCommand();
        }
        else if (c == 'q') {
            break;
        }
        else {
            std::wstring w(&c);
            auto s = std::string(w.begin(), w.end());
            std::cout << "Unknown action: " << s << std::endl;
        }
    }
    stop();
}

int main() {
    AppWrapperSample appSample;
    appSample.launch();
}
