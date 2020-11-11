#pragma once

#include "common.h"
#include "TwoWayCommunicator.h"

#include <string>


class NamedPipeClient;


class ScannerCommunication final {
public:
    ScannerCommunication();
    ScannerCommunication(ScannerCommunication& other) = delete;
    void operator=(const ScannerCommunication&) = delete;

    void SetGetStateCallback(std::function<void(void)> getStateCallback);
    void SetChangeMicrophoneCallback(std::function<void(int)> changeMicrophoneCallback);

    bool Command(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId);
    bool State(const char* state);
    bool KeywordDetected(const char* wakePhrase, float confidence);

private:
    std::string IncomingMessageCallback(const char* requestMessage);
    std::string GetMandatoryJsonPath(const std::string& messageType);

private:
    std::unique_ptr<TwoWayCommunicator> mCommunicator;
    std::function<void(void)> mGetStateCallback = nullptr;
    std::function<void(int)> mChangeMicrophoneCallback = nullptr;
};
