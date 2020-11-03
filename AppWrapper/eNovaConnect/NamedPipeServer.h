#pragma once

#include "Common.h"

#include <string>
#include <thread>
#include <atomic>
#include <windows.h>


extern "C" {
    class DLL_EXPORT NamedPipeServer final {
    public:
        struct RequestHandlerInterface {
            virtual ~RequestHandlerInterface() {};
            virtual std::string handleRequest(const char* requestMessage) = 0;
        };

    public:
        NamedPipeServer(const char* pipeName, RequestHandlerInterface* requestHandler);
        ~NamedPipeServer();
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
    };
}

