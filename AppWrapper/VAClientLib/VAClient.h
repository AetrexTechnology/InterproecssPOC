#pragma once
#include <functional>
#include <string>
#include <map>


class VAClient {

public:
    VAClient();
    virtual ~VAClient();
    bool Start();
    bool Stop();

    void SetSendCommandCallback(std::function<bool(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId)> sendCommandCallback);
    void SetSendStateCallback(std::function<bool(const char* state)> sendStateCallback);
    void SetSendKeywordDetectedCallback(std::function<bool(const char* wakePhrase, float confidence)> sendKeywordDetectedCallback);

    bool GetState();
    bool ChangeMicrophone(int microphoneIndex);

    void SendStateToScanner();
    void SendKeywordDetected(const char* wakePhrase, float confidence);

private:
    void listMap(const std::map<int, std::string>& map);
    std::string getValueFromMap(const std::map<int, std::string>& map, int key);

private:
    std::function<bool(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId)> mSendCommandCallback = nullptr;
    std::function<bool(const char* state)> mSendStateCallback = nullptr;
    std::function<bool(const char* wakePhrase, float confidence)> mSendKeywordDetectedCallback = nullptr;
    std::string mCurrentState;

    static const std::map<int, std::string> mCommands;
    static const std::map<int, std::string> mStates;
};
