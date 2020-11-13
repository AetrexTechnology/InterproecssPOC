#include "pch.h"
#include "TwoWayCommunicator.h"
#include "VoiceServiceToScanner.h"


TwoWayCommunicator::TwoWayCommunicator(const char* localPipeName, const char* remotePipeName, std::function<std::string(const char*)> callback) : mCallback(callback) {
    mScannerToVoiceService = std::make_unique<ScannerToVoiceService>(localPipeName, this);
    mVoiceServiceToScanner = std::make_unique<VoiceServiceToScanner>(remotePipeName);
    mScannerToVoiceService->Start();
}

TwoWayCommunicator::~TwoWayCommunicator() {
    mScannerToVoiceService->Stop();
}

std::string TwoWayCommunicator::sendData(const char* data, bool waitForResponse) {
    std::cout << "Sending request to Client:" << data << std::endl;
    return mVoiceServiceToScanner->sendRequest(data, waitForResponse);
}

std::string TwoWayCommunicator::handleRequest(const char* requestMessage) {
    std::cout << "Request received from the Client: " << requestMessage << std::endl;
    return mCallback(requestMessage);;
}

