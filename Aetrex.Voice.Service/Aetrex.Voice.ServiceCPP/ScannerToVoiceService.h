#pragma once

#include "Common.h"

#include <string>
#include <thread>
#include <atomic>
#include <windows.h>

class ScannerToVoiceService final {
public:
    struct RequestHandlerInterface {
        virtual ~RequestHandlerInterface() {};
        virtual std::string handleRequest(const char* requestMessage) = 0;
    };

public:
    ScannerToVoiceService(const char* pipeName, RequestHandlerInterface* requestHandler);
    ~ScannerToVoiceService();
    bool Start();
    bool Stop();
    void sendResponse(const char* responseMessage);

private:
    int Listen();
    DWORD WINAPI Read();

private:
    std::wstring mPipeName;
    std::thread mPipeThread;
    HANDLE mHPipe;
    RequestHandlerInterface* mRequestHandler;
    std::atomic<bool> mIsRun;
    std::shared_ptr<spdlog::logger> pLogger;
};

