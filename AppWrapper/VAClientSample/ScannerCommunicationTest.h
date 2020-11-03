#pragma once

#include <ScannerCommunication.h>

#include <string>
#include <map>


class ScannerCommunicationTest {
public:
    ScannerCommunicationTest();
    void SendStateToScanner();

private:
    void listMap(const std::map<int, std::string>& map);
    std::string getValueFromMap(const std::map<int, std::string>& map, int key);

    void getStateCallback();
    void changeMicrophoneCallback(int microphoneIndex);

private:
    std::unique_ptr<ScannerCommunication> mScannerCommunication;

    static const std::map<int, std::string> mCommands;
    static const std::map<int, std::string> mStates;
    std::string mCurrentState;
};

