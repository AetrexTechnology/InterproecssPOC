#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#define BUFSIZE 512

using namespace std;

namespace {
    auto logger = spdlog::stdout_color_mt("Aetrex.IPCCPP.Server");
}

VOID CreatePostToClientThread(HANDLE);
DWORD WINAPI PostToClientThread(LPVOID);
VOID PostMessage(HANDLE, TCHAR*, STRSAFE_LPCWSTR);

VOID CreateListenToClientThread(HANDLE);
DWORD WINAPI ListenToClientThread(LPVOID);
VOID FetchMessage(HANDLE, TCHAR*);

VOID InitLogging(const char*);

int _tmain(VOID)
{
    BOOL   fConnected = FALSE;    
    HANDLE hPipe = INVALID_HANDLE_VALUE, hListenToClientThread = NULL;
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\testpipe");

    InitLogging(".\\Logs\\Aetrex.IPCCPP.Server.log");

    // The main loop creates an instance of the named pipe and 
    // then waits for a client to connect to it. When the client 
    // connects, a thread is created to handle communications 
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop.

    for (;;)
    {
        _tprintf(TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
        hPipe = CreateNamedPipe(
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

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            _tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
            return -1;
        }

        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

        fConnected = ConnectNamedPipe(hPipe, NULL) ?
            TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (fConnected)
        {
            CreatePostToClientThread(hPipe);

            CreateListenToClientThread(hPipe);
        }
        else
        {
            // The client could not connect, so close the pipe. 
            CloseHandle(hPipe);
        }
    }

    return 0;
}

VOID CreatePostToClientThread(HANDLE hPipe)
{
    printf("Client connected, creating a posting thread.\n");
    HANDLE hPostToClientThread = NULL;
    DWORD  dwThreadId = 0;

    // Create a thread to post messages to the client. 
    hPostToClientThread = CreateThread(
        NULL,              // no security attribute 
        0,                 // default stack size 
        PostToClientThread,    // thread proc
        (LPVOID)hPipe,    // thread parameter 
        0,                 // not suspended 
        &dwThreadId);      // returns thread ID 

    if (hPostToClientThread == NULL)
    {
        _tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());        
    }
    else
    {
        _tprintf(TEXT("CloseHandle(hPostToClientThread)"));
        CloseHandle(hPostToClientThread);
    }
}
/*
DWORD WINAPI PostToClientThread(LPVOID lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{
    HANDLE hHeap = GetProcessHeap();
    TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
    TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipe = NULL;

    // Do some extra error checking since the app will keep running even if this
    // thread fails.

    if (lpvParam == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   PostToClientThread got an unexpected NULL value in lpvParam.\n");
        printf("   PostToClientThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    if (pchRequest == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   PostToClientThread got an unexpected NULL heap allocation.\n");
        printf("   PostToClientThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }

    if (pchReply == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   PostToClientThread got an unexpected NULL heap allocation.\n");
        printf("   PostToClientThread exitting.\n");
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    // Print verbose messages. In production code, this should be for debugging only.
    printf("PostToClientThread created, receiving and processing messages.\n");
    logger->info("PostToClientThread created, receiving and processing messages.");

    // The thread's parameter is a handle to a pipe object instance. 

    hPipe = (HANDLE)lpvParam;

    WelcomeClient(hPipe, pchReply, &cbReplyBytes);

    // Loop until done reading
    while (1)
    {
        // Read client requests from the pipe. This simplistic code only allows messages
        // up to BUFSIZE characters in length.
        fSuccess = ReadFile(
            hPipe,        // handle to pipe 
            pchRequest,    // buffer to receive data 
            BUFSIZE * sizeof(TCHAR), // size of buffer 
            &cbBytesRead, // number of bytes read 
            NULL);        // not overlapped I/O 

        if (!fSuccess || cbBytesRead == 0)
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
            {
                _tprintf(TEXT("PostToClientThread: client disconnected.\n"));
            }
            else
            {
                _tprintf(TEXT("PostToClientThread ReadFile failed, GLE=%d.\n"), GetLastError());
            }
            break;
        }

        // Process the incoming message.
        GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);

        // Write the reply to the pipe. 
        fSuccess = WriteFile(
            hPipe,        // handle to pipe 
            pchReply,     // buffer to write from 
            cbReplyBytes, // number of bytes to write 
            &cbWritten,   // number of bytes written 
            NULL);        // not overlapped I/O 

        if (!fSuccess || cbReplyBytes != cbWritten)
        {
            _tprintf(TEXT("PostToClientThread WriteFile failed, GLE=%d.\n"), GetLastError());
            break;
        }
    }

    // Flush the pipe to allow the client to read the pipe's contents 
    // before disconnecting. Then disconnect the pipe, and close the 
    // handle to this pipe instance. 

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    HeapFree(hHeap, 0, pchRequest);
    HeapFree(hHeap, 0, pchReply);

    printf("PostToClientThread exiting.\n");
    return 1;
}
*/

DWORD WINAPI PostToClientThread(LPVOID lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{
    HANDLE hHeap = GetProcessHeap();
    TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipe = NULL;

    // Do some extra error checking since the app will keep running even if this
    // thread fails.

    if (lpvParam == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   PostToClientThread got an unexpected NULL value in lpvParam.\n");
        printf("   PostToClientThread exitting.\n");
        if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
        return (DWORD)-1;
    }

    if (pchReply == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   PostToClientThread got an unexpected NULL heap allocation.\n");
        printf("   PostToClientThread exitting.\n");
        return (DWORD)-1;
    }

    // Print verbose messages. In production code, this should be for debugging only.
    printf("PostToClientThread created, receiving and processing messages.\n");
    logger->info("PostToClientThread created, receiving and processing messages.");

    // The thread's parameter is a handle to a pipe object instance. 
    hPipe = (HANDLE)lpvParam;

    // Loop until done reading
    for (int i=0; i< 10; i++)
    {
        //Sllep for 4 seconds
        this_thread::sleep_for(chrono::milliseconds(4000));

        const DWORD commandBufferSize = 64;
        TCHAR command[commandBufferSize];
        const TCHAR format[] = _T("Command %d");
        _snwprintf_s(command, commandBufferSize, format, i + 1);

        PostMessage(hPipe, pchReply, command);
       
        const DWORD logMessageBufferSize = 64;
        TCHAR logMessage[logMessageBufferSize];        
        _snwprintf_s(logMessage, commandBufferSize, _T("Voice command posted to scanner %s\n"), command);
        _tprintf(logMessage);
        //logger->debug(logMessage);

    }  
    
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    HeapFree(hHeap, 0, pchReply);

    printf("PostToClientThread exiting.\n");
    return 1;
}

VOID PostMessage(HANDLE hPipe, TCHAR* pchReply, STRSAFE_LPCWSTR pszSrc)
{
    // Check the outgoing message to make sure it's not too long for the buffer.
    if (FAILED(StringCchCopy(pchReply, BUFSIZE, pszSrc)))
    {
        pchReply[0] = 0;
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    DWORD replyBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);

    DWORD cbWritten;
    BOOL fSuccess = FALSE;
    fSuccess = WriteFile(
        hPipe,        // handle to pipe 
        pchReply,     // buffer to write from 
        replyBytes, // number of bytes to write 
        &cbWritten,   // number of bytes written 
        NULL);        // not overlapped I/O 

    if (!fSuccess || replyBytes != cbWritten)
    {
        _tprintf(TEXT("PostToClientThread WriteFile failed, GLE=%d.\n"), GetLastError());
    }
}

VOID CreateListenToClientThread(HANDLE hPipe)
{
    printf("Client connected, creating a listening thread.\n");
    HANDLE hListenToClientThread = NULL;
    DWORD  dwThreadId = 0;

    // Create a thread to post messages to the client. 
    hListenToClientThread = CreateThread(
        NULL,              // no security attribute 
        0,                 // default stack size 
        ListenToClientThread,    // thread proc
        (LPVOID)hPipe,    // thread parameter 
        0,                 // not suspended 
        &dwThreadId);      // returns thread ID 

    if (hListenToClientThread == NULL)
    {
        _tprintf(TEXT("ListenToClientThread failed, GLE=%d.\n"), GetLastError());
    }
    else
    {
        _tprintf(TEXT("CloseHandle(hListenToClientThread)\n"));
        CloseHandle(hListenToClientThread);
    }
}

DWORD WINAPI ListenToClientThread(LPVOID lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{
    HANDLE hHeap = GetProcessHeap();
    TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

    DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
    BOOL fSuccess = FALSE;
    HANDLE hPipe = NULL;

    // Do some extra error checking since the app will keep running even if this
    // thread fails.

    if (lpvParam == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   ListenToClientThread got an unexpected NULL value in lpvParam.\n");
        printf("   ListenToClientThread exitting.\n");
        if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
        return (DWORD)-1;
    }

    if (pchRequest == NULL)
    {
        printf("\nERROR - Pipe Server Failure:\n");
        printf("   ListenToClientThread got an unexpected NULL heap allocation.\n");
        printf("   ListenToClientThread exitting.\n");
        return (DWORD)-1;
    }

    // Print verbose messages. In production code, this should be for debugging only.
    printf("ListenToClientThread created, receiving and processing messages.\n");
    logger->info("ListenToClientThread created, receiving and processing messages.");

    // The thread's parameter is a handle to a pipe object instance. 
    hPipe = (HANDLE)lpvParam;

    // Loop until done reading
    for (int i = 0; i < 10; i++)
    {
        FetchMessage(hPipe, pchRequest);

        //const DWORD messageFromClientBufferSize = 100;
        //wchar_t messageFromClient[messageFromClientBufferSize];
        //swprintf_s(messageFromClient, _T("ListenToClientThread message from client %s.\n"), pchRequest);
        //_tprintf(messageFromClient);
        //logger->info(messageFromClient);
    }

    HeapFree(hHeap, 0, pchRequest);

    printf("ListenToClientThread exiting.\n");
    return 1;
}

VOID FetchMessage(HANDLE hPipe, TCHAR* pchRequest)
{
    // Read client requests from the pipe. This simplistic code only allows messages
    // up to BUFSIZE characters in length.
    DWORD cbBytesRead;
    
    bool fSuccess = ReadFile(
        hPipe,        // handle to pipe 
        pchRequest,    // buffer to receive data 
        BUFSIZE * sizeof(TCHAR), // size of buffer 
        &cbBytesRead, // number of bytes read 
        NULL);        // not overlapped I/O 
    
    if (!fSuccess || cbBytesRead == 0)
    {
        if (GetLastError() == ERROR_BROKEN_PIPE)
        {
            logger->info("FetchMessage: client disconnected");
        }
        else
        {
            const DWORD warningMessageBufferSize = 100;
            char warningMessage[warningMessageBufferSize];
            snprintf(warningMessage, warningMessageBufferSize, "FetchMessage ReadFile failed, GLE=%d", GetLastError());
            logger->warn(warningMessage);
        }
        
    }
    
}

/*
VOID GetAnswerToRequest(LPTSTR pchRequest,
    LPTSTR pchReply,
    LPDWORD pchBytes)
    // This routine is a simple function to print the client request to the console
    // and populate the reply buffer with a default data string. This is where you
    // would put the actual client request processing code that runs in the context
    // of an instance thread. Keep in mind the main thread will continue to wait for
    // and receive other client connections while the instance thread is working.
{
    _tprintf(TEXT("Client Request String:\"%s\"\n"), pchRequest);

    // Check the outgoing message to make sure it's not too long for the buffer.
    if (FAILED(StringCchCopy(pchReply, BUFSIZE, TEXT("default answer from server"))))
    {
        *pchBytes = 0;
        pchReply[0] = 0;
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    *pchBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);
}

VOID WelcomeClient(HANDLE hPipe, TCHAR* pchReply, LPDWORD pchReplyBytes)
{
    // Check the outgoing message to make sure it's not too long for the buffer.
    if (FAILED(StringCchCopy(pchReply, BUFSIZE, TEXT("I am the one true server!"))))
    {
        *pchReplyBytes = 0;
        pchReply[0] = 0;
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    *pchReplyBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);

    DWORD cbWritten;
    BOOL fSuccess = FALSE;
    fSuccess = WriteFile(
        hPipe,        // handle to pipe 
        pchReply,     // buffer to write from 
        *pchReplyBytes, // number of bytes to write 
        &cbWritten,   // number of bytes written 
        NULL);        // not overlapped I/O 

    if (!fSuccess || *pchReplyBytes != cbWritten)
    {
        _tprintf(TEXT("PostToClientThread WriteFile failed, GLE=%d.\n"), GetLastError());        
    }
}
*/
// create logger with 2 targets with different log levels and formats.
// the console will show only warnings or errors, while the file will log all.
VOID InitLogging(const char* log_filepath) {

    logger = spdlog::rotating_logger_mt("rotating_file_logger", log_filepath, 100000, 20);
    spdlog::flush_every(std::chrono::seconds(3));
    if (logger) {
        logger->set_pattern("[%c] [%l] %v");
    }
}