#pragma once

#include "Common.h"
#include <windows.h>


class VoiceServiceToScanner final {
public:
    VoiceServiceToScanner(const char* pipeName);
    std::string sendRequest(const char* requestMessage, bool waitForResponse);
private:
    std::wstring mPipeName;
    HANDLE mHPipe;
    std::shared_ptr<spdlog::logger> pLogger;
};