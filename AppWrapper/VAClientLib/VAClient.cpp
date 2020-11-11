
#include "pch.h"

#include "VAClient.h"
#include "VAClientLib.h"

#include <iostream>


const std::map<int, std::string> VAClient::mCommands = {
    {0, "Command_0"},
    {1, "Command_1"},
    {2, "Command_2"},
    {3, "Command_3"},
    {4, "Command_4"},
    {5, "Command_5"},
    {6, "Command_6"},
    {7, "Command_7"},
    {8, "Command_8"},
    {9, "Command_9"}
};


const std::map<int, std::string> VAClient::mStates = {
    {0, "InitializingState"},
    {1, "ListenWWState"},
    {2, "RecordingState"},
    {3, "ProcessingState"},
    {4, "SpeakingState"}
};


static VAClient* pVAClient = nullptr;

bool start() {
    bool res = false;
    if (pVAClient == nullptr) {
        pVAClient = new VAClient();
        res = pVAClient->Start();
    }
    return res;
}


bool stop() {
    bool res = false;
    if (pVAClient) {
        res = pVAClient->Stop();
        delete pVAClient;
        pVAClient = nullptr;
    }
    return res;
}


bool SetSendCommandCallback(std::function<bool(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId)> sendCommandCallback) {
    if (pVAClient) {
        pVAClient->SetSendCommandCallback(sendCommandCallback);
        return true;
    }
    return false;
}

bool SetSendStateCallback(std::function<bool(const char* state)> sendStateCallback) {
    if (pVAClient) {
        pVAClient->SetSendStateCallback(sendStateCallback);
        return true;
    }
    return false;
}

bool SetSendKeywordDetectedCallback(std::function<bool(const char* wakePhrase, float confidence)> sendKeywordDetectedCallback) {
    if (pVAClient) {
        pVAClient->SetSendKeywordDetectedCallback(sendKeywordDetectedCallback);
        return true;
    }
    return false;
}

bool GetState() {
    if (pVAClient) {
        return pVAClient->GetState();
    }
    return false;
}

bool ChangeMicrophone(int microphoneIndex) {
    if (pVAClient) {
        return pVAClient->ChangeMicrophone(microphoneIndex);
    }
    return false;
}

bool TestSendCommand() {
    if (pVAClient) {
        pVAClient->SendStateToScanner();
        return true;
    }
    return false;
}

bool TestSendKeywordDetected(const char* wakePhrase, float confidence) {
    if (pVAClient) {
        pVAClient->SendKeywordDetected(wakePhrase, confidence);
        return true;
    }
    return false;
}

VAClient::VAClient() {
    mCurrentState = mStates.at(0);
    std::cout << "VAClient is created in DLL." << std::endl;
}

VAClient::~VAClient() {
    std::cout << "VAClient is destroyed in DLL." << std::endl;
}

bool VAClient::Start() {
    std::cout << "VAClient is started in DLL." << std::endl;
    return true;
}

bool VAClient::Stop() {
    std::cout << "VAClient is stopped in DLL." << std::endl;
    return true;
}

void VAClient::SetSendCommandCallback(std::function<bool(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId)> sendCommandCallback) {
    mSendCommandCallback = sendCommandCallback;
}


void VAClient::SetSendStateCallback(std::function<bool(const char* state)> sendStateCallback) {
    mSendStateCallback = sendStateCallback;
}

void VAClient::SetSendKeywordDetectedCallback(std::function<bool(const char* wakePhrase, float confidence)> sendKeywordDetectedCallback) {
    mSendKeywordDetectedCallback = sendKeywordDetectedCallback;
}

bool VAClient::GetState() {
    if (mSendStateCallback != nullptr) {
        mSendStateCallback(mCurrentState.c_str());
        return true;
    }
    return false;
}

bool VAClient::ChangeMicrophone(int microphoneIndex) {
    std::cout << "Microphone Index is changed to: " << std::to_string(microphoneIndex) << std::endl;
    return true;
}

void VAClient::SendStateToScanner() {
    std::cout << "Please select command from the list below:" << std::endl;
    listMap(mCommands);
    std::string line;
    std::getline(std::cin, line);
    int i = -1;
    try {
        i = std::stoi(line);
    }
    catch (const std::exception&) {}
    auto command = getValueFromMap(mCommands, i);
    if (command != "") {
        std::cout << "Please select current Client state from the list below:" << std::endl;
        listMap(mStates);
        std::getline(std::cin, line);
        int i = -1;
        try {
            i = std::stoi(line);
        }
        catch (const std::exception&) {}
        auto state = getValueFromMap(mStates, i);
        if (state != "") {
            mCurrentState = state;
            bool result = mSendCommandCallback(state.c_str(), command.c_str(), "bla-bla???", "bla-bla-bla!!!", "INTENT_1", "ANSWER_1_3");
            std::cout << "Command: " << command << " is sent to the client with state: " << state << ", result: " << result << std::endl;
        } else {
            std::cout << "Unknown state: " << line << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << line << std::endl;
    }
}

void VAClient::SendKeywordDetected(const char* wakePhrase, float confidence) {
    mSendKeywordDetectedCallback(wakePhrase, confidence);
}

void VAClient::listMap(const std::map<int, std::string>& map) {
    for (auto it = map.begin(); it != map.end(); it++) {
        std::cout << std::to_string(it->first) << " : " << it->second << std::endl;
    }
}

std::string VAClient::getValueFromMap(const std::map<int, std::string>& map, int key) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    } else {
        return "";
    }
}
