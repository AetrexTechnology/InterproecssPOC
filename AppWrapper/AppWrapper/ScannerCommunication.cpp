#include "pch.h"
#include "ScannerCommunication.h"


static const std::string scannerPipeName("\\\\.\\pipe\\AetrexScannerOS2VoiceActivation");
static const std::string eNovaPipeName("\\\\.\\pipe\\eNovaClient");

ScannerCommunication::ScannerCommunication() {
    std::function<std::string(const char*)> f = std::bind(&ScannerCommunication::IncomingMessageCallback, this, std::placeholders::_1);
    mCommunicator = std::make_unique<TwoWayCommunicator>(eNovaPipeName.c_str(), scannerPipeName.c_str(), f);
}

void ScannerCommunication::SetGetStateCallback(std::function<void(void)> getStateCallback) {
    mGetStateCallback = getStateCallback;
}

void ScannerCommunication::SetChangeMicrophoneCallback(std::function<void(int)> changeMicrophoneCallback) {
    mChangeMicrophoneCallback = changeMicrophoneCallback;
}

bool ScannerCommunication::Command(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId) {
    if (mCommunicator) {
        try {
            std::string dateTime = Utils::getISO8601();
            std::string tmp = "{\"time\": \"" + dateTime + "\", \"state\": \"" + state + "\", \"command\": \"" + command + "\", \"stt_utterance\": \"" + sttUtterance + "\", \"answer_text\": \"" + answer + "\", \"intent_id\": \"" + intentId + "\", \"answer_id\": \"" + answerId + "\"}";
            mCommunicator->sendData(tmp.c_str(), false);
        } catch (std::runtime_error& e) {
            std::cout << "Error during sending request from ScannerCommunication: " << e.what() << std::endl;
            return false;
        }
        return true;
    } else {
        return false;
    }
}

bool ScannerCommunication::State(const char* state) {
    return Command(state, "", "", "", "", "");
}

std::string ScannerCommunication::IncomingMessageCallback(const char* requestMessage) {
    if (strlen(requestMessage) > 57 && !strncmp(requestMessage + 49, "GetState", 8)) {
        if (mGetStateCallback != nullptr) {
            mGetStateCallback();
        } else {
            std::cout << "Error: GetStateCallback isn't set." << std::endl;
        }
    } else if (strlen(requestMessage) > 75 && !strncmp(requestMessage + 49, "SetAudioCaptureDeviceIndex", 26)) {
        int microphoneIndex = -1;
        try {
            if (strlen(requestMessage) > 98) {
                char buf[8] = {0};
                strncpy_s(buf, requestMessage + 98, 1);
                microphoneIndex = std::stoi(buf);
            }
        } catch (const std::exception&) {}
        if (mChangeMicrophoneCallback != nullptr) {
            mChangeMicrophoneCallback(microphoneIndex);
        } else {
            std::cout << "Error: ChangeMicrophoneCallback isn't set." << std::endl;
        }
    } else {
        std::cout << "Unknown command received by ScannerCommunication." << std::endl;
    }
    return "";
}
