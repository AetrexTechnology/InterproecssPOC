#pragma once

#include "NamedPipeServer.h"

#include <thread>
#include <string>


class NamedPipeClient;

class ScannerConnector final: public NamedPipeServer::RequestHandlerInterface {
public:
    ScannerConnector();
    ScannerConnector(ScannerConnector& other) = delete;
    void operator=(const ScannerConnector&) = delete;
    virtual ~ScannerConnector() override;

    bool SendScannerToEnova(const std::string& instruction, int microphoneIndex);

private:
    std::string handleRequest(const char *requestMessage) override;
    bool Initialize();
    bool Deinitialize();

private:
    std::unique_ptr<NamedPipeServer> mNamedPipeServer;
    std::unique_ptr<NamedPipeClient> mNamedPipeClient;
};
