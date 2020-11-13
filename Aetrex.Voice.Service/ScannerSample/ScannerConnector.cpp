#include "ScannerConnector.h"
#include "NamedPipeClient.h"
#include "Common.h"

static const std::string scannerPipeName("\\\\.\\pipe\\AetrexScannerOS2VoiceActivation");
static const std::string eNovaPipeName("\\\\.\\pipe\\eNovaClient");


ScannerConnector::ScannerConnector() {
    mNamedPipeServer = std::make_unique<NamedPipeServer>(scannerPipeName.c_str(), this);
    mNamedPipeClient = std::make_unique<NamedPipeClient>(eNovaPipeName.c_str());
    Initialize();
}

ScannerConnector::~ScannerConnector() {
    Deinitialize();
}

bool ScannerConnector::Initialize() {
    if (mNamedPipeServer) {
        return mNamedPipeServer->Start();
    } else {
        return false;
    }
}

bool ScannerConnector::Deinitialize() {
    if (mNamedPipeServer) {
        return mNamedPipeServer->Stop();
    } else {
        return false;
    }
}

bool ScannerConnector::SendScannerToEnova(const std::string& instruction, int microphoneIndex) {
    std::string dateTime = Utils::getISO8601();
    std::string tmp = "{\"time\": \"" + dateTime + "\", \"instruction\" : \"" + instruction + "\", \"microphoneIndex\" : " + std::to_string(microphoneIndex) + "}";
    std::cout << "Sending request to Client:" << tmp << std::endl;
    try {
        mNamedPipeClient->sendRequest(tmp.c_str(), false);
        return true;
    } catch (std::runtime_error& e) {
        std::cout << "ScannerConnector. ERROR DURING SENDING REQUEST FROM ScannerConnector: " << e.what() << std::endl;
        return false;
    }
}

std::string ScannerConnector::handleRequest(const char* requestMessage) {
    std::cout << "Request received from the Client: " << requestMessage << std::endl;
    return "";
}

