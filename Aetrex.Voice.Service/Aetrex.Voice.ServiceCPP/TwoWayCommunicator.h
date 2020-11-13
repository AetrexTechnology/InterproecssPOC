#pragma once

#include "ScannerToVoiceService.h"

#include <string>
#include <functional>


class NamedPipeClient;


class TwoWayCommunicator final : public ScannerToVoiceService::RequestHandlerInterface {
public:
    TwoWayCommunicator(const char* localPipeName, const char* remotePipeName, std::function<std::string(const char*)> keywordDetectedCallback);
    TwoWayCommunicator(TwoWayCommunicator& other) = delete;
    void operator=(const TwoWayCommunicator&) = delete;
    virtual ~TwoWayCommunicator() override;

    std::string sendData(const char* data, bool waitForResponse);

private:
    std::string handleRequest(const char* requestMessage) override;

private:
    std::unique_ptr<ScannerToVoiceService> mScannerToVoiceService;
    std::unique_ptr<NamedPipeClient> mNamedPipeClient;
    std::function<std::string(const char *)> mCallback;
};
