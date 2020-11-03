#include "NamedPipeClient.h"
#include "Common.h"

#include <tchar.h>
#include <sstream>
#include <stdexcept>
#include <locale>
#include <codecvt>


NamedPipeClient::NamedPipeClient(const char* pipeName) {
    mPipeName = Utils::stringToWstring(pipeName);
}

std::string NamedPipeClient::sendRequest(const char *requestMessage, bool waitForResponse) {
    HANDLE mHPipe;
    TCHAR  chBuf[BUFSIZE];
    BOOL   fSuccess = FALSE;
    DWORD  cbRead, cbToWrite, cbWritten, dwMode;
    LPCTSTR lpszPipename = mPipeName.c_str();
    // Try to open a named pipe; wait for it, if necessary.
    while (true) {
        mHPipe = CreateFile(
            lpszPipename,   // pipe name 
            GENERIC_READ |  // read and write access 
            GENERIC_WRITE,
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file 

        // Break if the pipe handle is valid. 
        if (mHPipe != INVALID_HANDLE_VALUE)
            break;

        // Exit if an error other than ERROR_PIPE_BUSY occurs. 
        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::string errorTxt = "NamedPipeClient::sendRequest, Can't open pipe, GLE=" + std::to_string(GetLastError());
            throw std::runtime_error(errorTxt);
        }

        // All pipe instances are busy, so wait for 20 seconds. 
        if (!WaitNamedPipe(lpszPipename, 2000)) {
            throw std::runtime_error("Could not open pipe: 2 second wait timed out.");
        }
    }

    // The pipe connected; change to message-read mode. 
    dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState(
        mHPipe,    // pipe handle 
        &dwMode,  // new pipe mode 
        NULL,     // don't set maximum bytes 
        NULL);    // don't set maximum time 

    if (!fSuccess) {
        std::string errorTxt = "NamedPipeClient::sendRequest, SetNamedPipeHandleState failed, GLE=" + std::to_string(GetLastError());
        throw std::runtime_error(errorTxt);
    }

    // Send a message to the pipe server. 
    std::wstring wide = Utils::stringToWstring(requestMessage);
    cbToWrite = (DWORD) (wide.size() + 1) * sizeof(wchar_t);

    fSuccess = WriteFile(
        mHPipe,                  // pipe handle 
        wide.c_str(),
        cbToWrite,              // message length 
        &cbWritten,             // bytes written 
        NULL);                  // not overlapped 

    if (!fSuccess) {
        std::string errorTxt = "NamedPipeClient::sendRequest, WriteFile to pipe failed, GLE=" + std::to_string(GetLastError());
        throw std::runtime_error(errorTxt);
    }

    std::string response = "";
    if (waitForResponse) {
        do {
            // Read from the pipe. 
            fSuccess = ReadFile(
                mHPipe,    // pipe handle 
                chBuf,    // buffer to receive reply 
                BUFSIZE * sizeof(TCHAR),  // size of buffer 
                &cbRead,  // number of bytes read 
                NULL);    // not overlapped 

            if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
                break;

        } while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

        if (!fSuccess) {
            std::string errorTxt = "NamedPipeClient::sendRequest, ReadFile from pipe failed, GLE=" + std::to_string(GetLastError());
            throw std::runtime_error(errorTxt);
        }
        response = Utils::wstringToString(chBuf);
    }

    CloseHandle(mHPipe);

    return response;
}