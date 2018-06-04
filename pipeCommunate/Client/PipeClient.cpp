#include "PipeClient.h"
#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>

PipeClient::PipeClient(DWORD dwID):
	mId(dwID){
	StringCbPrintfA(mPipeName,
		MAX_PATH,
		"\\\\.\\pipe\\2DB7D709-2E5F-47A5-80D3-ACB225A17864-%d",
		mId
	);
}


PipeClient::~PipeClient(){
	if (mhPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(mhPipe);
	}
}

BOOL PipeClient::Init() {
	while (true){
		mhPipe = CreateFileA(
			mPipeName,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			FILE_FLAG_OVERLAPPED,              // default attributes 
			NULL);          // no template file 

		if (mhPipe != INVALID_HANDLE_VALUE) {
			return TRUE;
		}

		if (GetLastError() != ERROR_PIPE_BUSY){
			return FALSE;
		}
		if (!WaitNamedPipeA(mPipeName, 10000))
		{
			return FALSE;
		}
	}

	return FALSE;
}
BOOL PipeClient::SetPipeAttrib() {
	// The pipe connected; change to message-read mode. 

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	BOOL fSuccess = SetNamedPipeHandleState(
		mhPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		return FALSE;
	}
	for (int i = 0; i < 2; i++) {
		mhEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		mPipeInst[i].oOverlap.hEvent = mhEvent[i];
	}

	return TRUE;
}
BOOL PipeClient::GetReadStatus() {
	// The read operation completed successfully. 
	do {
		DWORD dwBytesRead = 0;
		BOOL bResult = GetOverlappedResult(mhPipe,
			&(mPipeInst[0].oOverlap),
			&dwBytesRead,
			FALSE);

		if (bResult && dwBytesRead != 0) {
			OnRead(mPipeInst[0].pData, dwBytesRead);
			break;
		}
		else {
			DWORD dwErr = GetLastError();

			//no signaled
			if (ERROR_IO_INCOMPLETE == dwErr) {
				break;
			}
			//pending ...
			else if (ERROR_IO_PENDING == dwErr) {
				continue;
			}
			else {
				return FALSE;
			}


		}
	} while (1);

	return TRUE;
}
BOOL PipeClient::ReadData() {
	DWORD dwSize = 0;
	return ReadFile(mhPipe,
		mPipeInst[0].pData,
		mPipeInst[0].dwSize,
		&dwSize,
		&(mPipeInst[0].oOverlap));
}

VOID PipeClient::OnRead(PVOID pData, DWORD dwBytesRead) {
	BYTE *pStr = (BYTE *)pData;
	pStr[dwBytesRead] = 0;
	printf("%s\n", (char*)pStr);
	ReadData();
}

VOID PipeClient::OnWrite() {
	printf("Write completely\n");
}

BOOL PipeClient::GetWriteStatus() {
	DWORD dwWrote;
	do {
		DWORD dwBytesRead = 0;
		BOOL bResult = GetOverlappedResult(mhPipe,
			&(mPipeInst[1].oOverlap),
			&dwWrote,
			FALSE);

		if (bResult && dwWrote != 0) {
			OnWrite();
			break;
		}
		else {
			DWORD dwErr = GetLastError();

			//no signaled
			if (ERROR_IO_INCOMPLETE == dwErr) {
				break;
			}

			//pending ...
			else if (ERROR_IO_PENDING == dwErr) {
				continue;
			}
			else {
				return FALSE;
			}
		}
	} while (1);

	//ResetEvent(mhEvent[1]);

	return TRUE;
}
BOOL PipeClient::WriteData(PVOID pData, DWORD dwSize) {

	DWORD dwWrote;
	do {
		BOOL bResult = GetOverlappedResult(mhPipe,
			&(mPipeInst[1].oOverlap),
			&dwWrote,
			FALSE);

		if (bResult) {
			break;
		}
		else {
			DWORD dwError = GetLastError();
			if (dwError == ERROR_IO_PENDING) {
				continue;
			}
			else {
				break;
			}
		}
	} while (true);


	
	BOOL fResult =  WriteFile(mhPipe,
		pData,
		dwSize,
		&dwWrote,
		&(mPipeInst[1].oOverlap));

	return fResult;
}
VOID PipeClient::Run() {

	Init();
	SetPipeAttrib();
	ReadData();

	BOOL result = TRUE;
	do {
		DWORD dwWait = WaitForMultipleObjects(2, mhEvent, FALSE, INFINITE);
		DWORD dwIndex = dwWait - WAIT_OBJECT_0;
		switch (dwIndex) {
		case 0:
			result = GetReadStatus();
			break;
		case 1:
			result = GetWriteStatus();
			break;
		case WAIT_TIMEOUT:
			result = TRUE;
			break;
		case WAIT_FAILED:
			result = FALSE;
			break;
		default:
			result = TRUE;
			break;
		}
	} while (result);
}