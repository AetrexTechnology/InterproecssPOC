#include "ScannerCommunicationTest.h"

#include <iostream>


const std::map<int, std::string> ScannerCommunicationTest::mCommands = {
    {0, "BeginScanMaleCommand"},
    {1, "BeginScanFemaleCommand"},
    {2, "StartCommand"},
    {3, "StopCommand"},
    {4, "PlayCommand"},
    {5, "GoHomeCommand"},
    {6, "GoBackCommand"},
    {7, "ScrollDownCommand"},
    {8, "ScrollUpCommand"},
    {9, "GoToPressureCommand"},
    {10, "GoToGaitAnalysisCommand"},
    {11, "GoTo3DmeasurementsCommand"},
    {12, "GoToOrthoticsCommand"},
    {13, "WhatIs00Command"},
    {14, "WhatIs05Command"},
    {15, "WhatIs20Command"},
    {16, "WhatIs25Command"},
    {17, "GoToFitHQCommand"},
    {18, "GoToLearningCenterCommand"},
    {19, "GoToEmailCommand"},
    {20, "NoThankYouCommand"},
    {21, "ComparisonTestCommand"},
    {22, "RescanPressureTestCommand"},
    {23, "3DprintingZonesCommand"},
    {24, "RescanDynamicTestCommand"},
    {25, "WithOrthoticsCommand"},
    {26, "ShowPreviousScanCommand"}
};


const std::map<int, std::string> ScannerCommunicationTest::mStates = {
    {0, "InitializingState"},
    {1, "ListenWWState"},
    {2, "RecordingState"},
    {3, "ProcessingState"},
    {4, "SpeakingState"}
};

ScannerCommunicationTest::ScannerCommunicationTest() {
    std::function<void(void)> getStateCallback = std::bind(&ScannerCommunicationTest::getStateCallback, this);
    std::function<void(int)> changeMicrophoneCallback = std::bind(&ScannerCommunicationTest::changeMicrophoneCallback, this, std::placeholders::_1);
    mScannerCommunication = std::make_unique<ScannerCommunication>();
    mScannerCommunication->SetGetStateCallback(getStateCallback);
    mScannerCommunication->SetChangeMicrophoneCallback(changeMicrophoneCallback);
    mCurrentState = mStates.at(0);
}

void ScannerCommunicationTest::SendStateToScanner() {
    std::cout << "Please select command from the list below:" << std::endl;
    listMap(mCommands);
    std::string line;
    std::getline(std::cin, line);
    int i = -1;
    try {
        i = std::stoi(line);
    } catch (const std::exception&) {}
    auto command = getValueFromMap(mCommands, i);
    if (command != "") {
        std::cout << "Please select current Client state from the list below:" << std::endl;
        listMap(mStates);
        std::getline(std::cin, line);
        int i = -1;
        try {
            i = std::stoi(line);
        } catch (const std::exception&) {}
        auto state = getValueFromMap(mStates, i);
        if (state != "") {
            mCurrentState = state;
            bool result = mScannerCommunication->Command(command.c_str(), state.c_str(), "bla-bla???", "bla-bla-bla!!!", "INTENT_1", "ANSWER_1_3");
            std::cout << "Command: " << command << " is sent to the client with state: " << state << ", result: " << result << std::endl;
        } else {
            std::cout << "Unknown state: " << line << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << line << std::endl;
    }
}

void ScannerCommunicationTest::getStateCallback() {
    std::cout << "getStateCallback" << std::endl;
    mScannerCommunication->State(mCurrentState.c_str());
}

void ScannerCommunicationTest::changeMicrophoneCallback(int microphoneIndex) {
    std::cout << "changeMicrophoneCallback:" << std::to_string(microphoneIndex) << std::endl;
}

void ScannerCommunicationTest::listMap(const std::map<int, std::string>& map) {
    for (auto it = map.begin(); it != map.end(); it++) {
        std::cout << std::to_string(it->first) << " : " << it->second << std::endl;
    }
}

std::string ScannerCommunicationTest::getValueFromMap(const std::map<int, std::string>& map, int key) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    } else {
        return "";
    }
}
