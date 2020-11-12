#include "pch.h"
#include "NamedPipeServer.h"
#include "Common.h"

#include <tchar.h>
#include <sstream>
#include <locale>
#include <codecvt>


NamedPipeServer::NamedPipeServer(const char *pipeName, RequestHandlerInterface* requestHandler) : mRequestHandler(requestHandler) {
    mPipeName = Utils::stringToWstring(pipeName);
}

NamedPipeServer::~NamedPipeServer() {
    Stop();
}

bool NamedPipeServer::Start() {
    if (!mIsRun) {
        mPipeThread = std::thread(&NamedPipeServer::Listen, this);
        return true;
    } else {
        return false;
    }
}

bool NamedPipeServer::Stop() {
    if (mIsRun) {
        mIsRun = false;
        LPCTSTR lpszPipename = mPipeName.c_str();
        DeleteFile(lpszPipename);
        if (mPipeThread.joinable()) {
            mPipeThread.join();
        }
        return true;
    } else {
        return false;
    }
}

int NamedPipeServer::Listen() {
    mIsRun = true;
    BOOL   fConnected = FALSE;
    DWORD  dwThreadId = 0;
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
    LPCTSTR lpszPipename = mPipeName.c_str();

    // The main loop creates an instance of the named pipe and
    // then waits for a client to connect to it. When the client
    // connects, a thread is created to handle communications
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop.
    while (mIsRun) {
        OutputDebugString(L"Pipe Server: Main thread awaiting client connection\n");
        std::cout << "Pipe Server: Main thread awaiting client connection" << std::endl;
        mHPipe = CreateNamedPipe(
            lpszPipename,             // pipe name
            PIPE_ACCESS_DUPLEX,       // read/write access
            PIPE_TYPE_MESSAGE |       // message type pipe
            PIPE_READMODE_MESSAGE |   // message-read mode
            PIPE_WAIT,                // blocking mode
            PIPE_UNLIMITED_INSTANCES, // max. instances
            BUFSIZE,                  // output buffer size
            BUFSIZE,                  // input buffer size
            0,                        // client time-out
            NULL);                    // default security attribute

        if (mHPipe == INVALID_HANDLE_VALUE) {
            std::cout << "NamedPipeServer::Read, CreateNamedPipe failed, GLE=" << GetLastError() << std::endl;
            return -1;
        }

        // Wait for the client to connect; if it succeeds,
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED.
        fConnected = ConnectNamedPipe(mHPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (fConnected) {
            OutputDebugString(L"Client connected, creating a processing thread.\n");
            std::cout << "Client connected" << std::endl;
            Read();
        } else {
            // The client could not connect, so close the pipe.
            CloseHandle(mHPipe);
        }
    }

    return 0;
}

DWORD WINAPI NamedPipeServer::Read() {
    // This routine is a thread processing function to read from and reply to a client
    // via the open pipe connection passed from the main loop. Note this allows
    // the main loop to continue executing, potentially creating more threads of
    // of this procedure to run concurrently, depending on the number of incoming
    // client connections.
    HANDLE hHeap = GetProcessHeap();
    TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
    TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;

    // Do some extra error checking since the app will keep running even if this
    // thread fails.
    if (mHPipe == NULL) {
        OutputDebugString(L"\nERROR - Pipe Server Failure:\n");
        OutputDebugString(L"   InstanceThread got an unexpected NULL value in mHPipe.\n");
        OutputDebugString(L"   InstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    if (pchRequest == NULL) {
        OutputDebugString(L"\nERROR - Pipe Server Failure:\n");
        OutputDebugString(L"   InstanceThread got an unexpected NULL heap allocation.\n");
        OutputDebugString(L"   InstanceThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }

    if (pchReply == NULL) {
        OutputDebugString(L"\nERROR - Pipe Server Failure:\n");
        OutputDebugString(L"   InstanceThread got an unexpected NULL heap allocation.\n");
        OutputDebugString(L"   InstanceThread exitting.\n");
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    // Loop until done reading
    while (mIsRun) {
        //Read client requests from the pipe. This simplistic code only allows messages
        //up to BUFSIZE characters in length.
        std::cout << "NamedPipeServer::Read, about to read from pipe." << std::endl;

        fSuccess = ReadFile(
            mHPipe,        // handle to pipe
            pchRequest,    // buffer to receive data
            BUFSIZE * sizeof(TCHAR), // size of buffer
            &cbBytesRead, // number of bytes read
            NULL);        // not overlapped I/O

        if (!fSuccess || cbBytesRead == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                std::cout << "NamedPipeServer::Read, client disconnected." << std::endl;
            } else {
                std::cout << "NamedPipeServer::Read, ReadFile failed, GLE=" << GetLastError() << std::endl;
            }
            break;
        }

        std::cout << "NamedPipeServer::Read, read from pipe complete." << std::endl;

        //Process the incoming message.
        std::string request = Utils::wstringToString(pchRequest);

        if (mRequestHandler) {
            auto response = mRequestHandler->handleRequest(request.c_str());
            if (!response.empty()) {
                try {
                    sendResponse(response.c_str());
                } catch (std::runtime_error& e) {
                    std::cout << "ERROR DURING SENDING RESPONSE FROM NamedPipeServer: " << e.what() << "\n";
                    return -1;
                }
            }
        }
    }

    // Flush the pipe to allow the client to read the pipe's contents
    // before disconnecting. Then disconnect the pipe, and close the
    // handle to this pipe instance.
    FlushFileBuffers(mHPipe);
    DisconnectNamedPipe(mHPipe);
    CloseHandle(mHPipe);

    HeapFree(hHeap, 0, pchRequest);
    HeapFree(hHeap, 0, pchReply);

    return 1;
}

void NamedPipeServer::sendResponse(const char *responseMessage) {
    BOOL fSuccess = FALSE;
    DWORD cbWritten = 0, cbReplyBytes = (DWORD) strlen(responseMessage) + 1;

    std::wstring wide = Utils::stringToWstring(responseMessage);
    cbReplyBytes = (DWORD) (wide.size() + 1) * sizeof(wchar_t);

    // Write the reply to the pipe.
    fSuccess = WriteFile(
        mHPipe,       // handle to pipe
        wide.c_str(), // buffer to write from
        cbReplyBytes, // number of bytes to write
        &cbWritten,   // number of bytes written
        NULL);        // not overlapped I/O

    if (!fSuccess || cbReplyBytes != cbWritten) {
        std::cout << "NamedPipeServer::Read, WriteFile failed, GLE=" << GetLastError() << std::endl;
        std::string errorTxt = "NamedPipeServer::sendResponse() failed, GLE=" + std::to_string(GetLastError());
        throw std::runtime_error(errorTxt);
    }
}