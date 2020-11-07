#include "pch.h"
#include "TwoWayCommunicator.h"
#include "NamedPipeClient.h"


TwoWayCommunicator::TwoWayCommunicator(const char* localPipeName, const char* remotePipeName, std::function<std::string(const char*)> callback) : mCallback(callback) {
    mNamedPipeServer = std::make_unique<NamedPipeServer>(localPipeName, this);
    mNamedPipeClient = std::make_unique<NamedPipeClient>(remotePipeName);
    mNamedPipeServer->Start();
}

TwoWayCommunicator::~TwoWayCommunicator() {
    mNamedPipeServer->Stop();
}

std::string TwoWayCommunicator::sendData(const char* data, bool waitForResponse) {
    std::cout << "Sending request to Client:" << data << std::endl;
    return mNamedPipeClient->sendRequest(data, waitForResponse);
}

std::string TwoWayCommunicator::handleRequest(const char* requestMessage) {
    std::cout << "Request received from the Client: " << requestMessage << std::endl;
    return mCallback(requestMessage);;
}

