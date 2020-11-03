#pragma once

#include "Common.h"
#include <windows.h>


extern "C" {
    class DLL_EXPORT NamedPipeClient final {
    public:
        NamedPipeClient(const char* pipeName);
        std::string sendRequest(const char* requestMessage, bool waitForResponse);
    private:
        std::wstring mPipeName;
        HANDLE mHPipe;
    };
}
