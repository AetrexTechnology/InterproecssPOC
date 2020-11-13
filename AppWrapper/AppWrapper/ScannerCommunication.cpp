#include "pch.h"
#include "ScannerCommunication.h"
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;

static const std::string scannerPipeName("\\\\.\\pipe\\AetrexVoiceServiceToScanner");
static const std::string eNovaPipeName("\\\\.\\pipe\\AetrexScannerToVoiceService");

ScannerCommunication::ScannerCommunication() {
    //pLogger = spdlog::rotating_logger_mt("ScannerCommunication", "logs/Aetrex.Voice.Service.log", 200000, 3);
    ////https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    ////DateTime, Process id, Thread id, log level, logger name, log message
    //pLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][PID=%P][Thd=%t] <%l> %n - %v");
    pLogger = Utils::initLogger("ScannerCommunication");

    std::function<std::string(const char*)> f = std::bind(&ScannerCommunication::IncomingMessageCallback, this, std::placeholders::_1);
    mCommunicator = std::make_unique<TwoWayCommunicator>(eNovaPipeName.c_str(), scannerPipeName.c_str(), f);
}

void ScannerCommunication::SetGetStateCallback(std::function<void(void)> getStateCallback) {
    mGetStateCallback = getStateCallback;
}

void ScannerCommunication::SetChangeMicrophoneCallback(std::function<void(int)> changeMicrophoneCallback) {
    mChangeMicrophoneCallback = changeMicrophoneCallback;
}

std::string ScannerCommunication::GetMandatoryJsonPath(const std::string& messageType) {
    std::string dateTime = Utils::getISO8601();
    return "{\"time\": \"" + dateTime + "\", \"message_type\": \"" + messageType + "\"";
}

bool ScannerCommunication::Command(const char* state, const char* command, const char* sttUtterance, const char* answer, const char* intentId, const char* answerId) {
    if (mCommunicator) {
        try 
        {            
            json jCommand = {
                {"time", Utils::getISO8601()},
                {"message_type", command},
                {"stt_utterance", sttUtterance},
                {"answer_text", answer},
                {"intent_id", intentId},
                {"answer_id", answerId}
            };
            std::string commandJsonStr = jCommand.dump();
            std::string logMessage = fmt::format("Voice Activity:{}", commandJsonStr);
            pLogger->info(logMessage);
            mCommunicator->sendData(commandJsonStr.c_str(), false);
        }
        catch (std::runtime_error& e) {
            std::string errorMessage = fmt::format("Error during sending command from ScannerCommunication: {}",e.what());
            std::cout << errorMessage << std::endl;
            pLogger->error(errorMessage);
            return false;
        }
        return true;
    }
    else {
        return false;
    }
}

bool ScannerCommunication::State(const char* state) {
    if (mCommunicator) {
        try 
        {
            json jCommand = {
                {"time", Utils::getISO8601()},
                {"message_type", "State"},
                {"state", state}
            };
            mCommunicator->sendData(jCommand.dump().c_str(), false);
        }
        catch (std::runtime_error& e) {
            std::string errorMessage = fmt::format("Error during sending state from ScannerCommunication: {}", e.what());
            std::cout << errorMessage << std::endl;
            pLogger->error(errorMessage);

            return false;
        }
        return true;
    }
    else {
        return false;
    }
}

bool ScannerCommunication::KeywordDetected(const char* wakePhrase, float confidence) {
    if (mCommunicator) {
        try {
            std::string mandatoryJsonPath = GetMandatoryJsonPath("KeywordDetected");
            std::stringstream confidenceStream;
            confidenceStream << std::fixed << std::setprecision(2) << confidence;
            std::string confidenceStr = confidenceStream.str();
            std::string tmp = mandatoryJsonPath + ", \"wake_phrase\": \"" + wakePhrase + "\", \"confidence\": " + confidenceStr + "}";
            json jWakeword = {
                {"time", Utils::getISO8601()},
                {"message_type", "WakewordDetected"},
                {"wake_phrase", wakePhrase},
                {"confidence", confidence}
            };
            std::string wakewordJsonStr = jWakeword.dump();
            std::string logMessage = fmt::format("Wakeword Activity:{}", wakewordJsonStr);
            pLogger->info(logMessage);

            mCommunicator->sendData(wakewordJsonStr.c_str(), false);
        }
        catch (std::runtime_error& e) {
            std::string errorMessage = fmt::format("Error during sending WakewordDetected from ScannerCommunication: {}", e.what());
            std::cout << errorMessage << std::endl;
            pLogger->error(errorMessage);

            return false;
        }
        return true;
    }
    else {
        return false;
    }
}

std::string ScannerCommunication::IncomingMessageCallback(const char* requestMessage) {
    try
    {
        auto request = json::parse(requestMessage);        
        std::string instruction = request["instruction"];
        std::cout << "ScannerCommunication::IncomingMessageCallback instruction=" << instruction << std::endl;
        std::string logMessage = fmt::format("IncomingMessageCallback instruction:{}", instruction);
        pLogger->info(logMessage);

        if (instruction.compare("SetAudioCaptureDeviceIndex") == 0)
        {
            int microphoneIndex = request["microphoneIndex"];
            
            if (mChangeMicrophoneCallback != nullptr) 
            {
                mChangeMicrophoneCallback(microphoneIndex);
            }
            else 
            {
                std::string errorMessage = fmt::format("IncomingMessageCallback() ChangeMicrophoneCallback isn't set.");
                std::cout << errorMessage << std::endl;
                pLogger->error(errorMessage);
            }
        }
        
        if (instruction.compare("GetState") == 0)
        {
            if (mGetStateCallback != nullptr) {
                mGetStateCallback();
            } else {
                std::string errorMessage = fmt::format("IncomingMessageCallback() GetStateCallback isn't set.");
                std::cout << errorMessage << std::endl;
                pLogger->error(errorMessage);

            }
        }
    }
    catch (std::runtime_error& e) {
        std::string errorMessage = fmt::format("IncomingMessageCallback failed to parse incoming request json: {}", e.what());
        std::cout << errorMessage << std::endl;
        pLogger->error(errorMessage);

        return "";
    }

    return "";
}
