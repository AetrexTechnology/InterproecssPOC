#pragma once
#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include "common.h"

class original
{
public:
	int Init();

private:
	HANDLE m_hPipe;
	static VOID WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cbWritten, LPOVERLAPPED lpOverLap);
	static VOID WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead, LPOVERLAPPED lpOverLap);
	static VOID DisconnectAndClose(LPPIPEINST lpPipeInst);
	BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap);
	BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo);
	static VOID GetAnswerToRequest(LPPIPEINST pipe);
};