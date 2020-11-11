#pragma once
#include <functional>

#ifdef VACLIENTLIB_EXPORTS
#define VACLIENTLIB_API __declspec(dllexport)
#else
#define VACLIENTLIB_API __declspec(dllimport)
#endif

extern "C" {
    VACLIENTLIB_API bool start();
    VACLIENTLIB_API bool stop();
    VACLIENTLIB_API bool SetSendCommandCallback(std::function<bool(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId)> sendCommandCallback);
    VACLIENTLIB_API bool SetSendStateCallback(std::function<bool(const char* state)> sendStateCallback);
    VACLIENTLIB_API bool SetSendKeywordDetectedCallback(std::function<bool(const char* wakePhrase, float confidence)> sendKeywordDetectedCallback);

    VACLIENTLIB_API bool GetState();
    VACLIENTLIB_API bool ChangeMicrophone(int microphoneIndex);

    //These are test functions. Remove them later.
    VACLIENTLIB_API bool TestSendCommand();
    VACLIENTLIB_API bool TestSendKeywordDetected(const char* wakePhrase, float confidence);
}