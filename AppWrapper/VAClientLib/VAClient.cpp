#include "pch.h"

#include "VAClient.h"
#include "VAClientLib.h"

#include <iostream>

static VAClient* pVAClient = nullptr;

bool start() {
	bool res = false;
	if (pVAClient == nullptr) {
		pVAClient = new VAClient();
		res = pVAClient->Start();
	}

	return res;
}


bool stop() {
	bool res = false;
	if (pVAClient) {
		res = pVAClient->Stop();
		delete pVAClient;
		pVAClient = nullptr;
	}

	return res;
}

VAClient::VAClient() {
	std::cout << "VAClient is created in DLL." << std::endl;
}

VAClient::~VAClient() {
	std::cout << "VAClient is destroyed in DLL." << std::endl;
}

bool VAClient::Start() {
	std::cout << "VAClient is started in DLL." << std::endl;
	return true;
}

bool VAClient::Stop() {
	std::cout << "VAClient is stopped in DLL." << std::endl;
	return true;
}

