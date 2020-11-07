#pragma once
#include <functional>

#ifdef VACLIENTLIB_EXPORTS
#define VACLIENTLIB_API __declspec(dllexport)
#else
#define VACLIENTLIB_API __declspec(dllimport)
#endif


extern "C" VACLIENTLIB_API bool start();
extern "C" VACLIENTLIB_API bool stop();
extern "C" VACLIENTLIB_API bool SetSendCommandCallback(std::function<bool(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId)> sendCommandCallback);
extern "C" VACLIENTLIB_API bool SetSendStateCallback(std::function<bool(const char* state)> sendStateCallback);

extern "C" VACLIENTLIB_API bool GetState();
extern "C" VACLIENTLIB_API bool ChangeMicrophone(int microphoneIndex);

//This is a test function. Remove it later.
extern "C" VACLIENTLIB_API bool TestSendCommand();
