#include "PipeServer.h"
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>
#include <functional>

void PipeSever::RunLoop() {
	do {
		Run();
	} while (true);
}

PipeSever::~PipeSever() {

	if (hEventList) {
		for (DWORD i = 0; i < mPipeSize * 2; i++) {
			CloseHandle(hEventList[i]);
		}
		delete hEventList;
	}

}
BOOL PipeSever::CreateServer() {
	hEventList = new HANDLE[mPipeSize * 2];

	for (DWORD i = 0; i < mPipeSize; i++) {
		if (!CreatePipe(i)) {
			Uninit();
			break;
		}
	}


	mRequestThreadHandle = CreateThread(NULL, 0, RequestThread, this, 0, NULL);
	mResponseThreadHandle = CreateThread(NULL, 0, ResponseThread, this, 0, NULL);
	return TRUE;
}
VOID PipeSever::Uninit() {

}
BOOL PipeSever::CreatePipe(UINT32 index) {

	LPCOMMONDATA pCommonData = new COMMONDATA();
	StringCbPrintfA(pCommonData->mPipeName,
		MAX_PATH,
		"\\\\.\\pipe\\mynamedpipe%d",
		index
		);
	pCommonData->id = index;
	pCommonData->hPipeInst = CreateNamedPipeA(
		pCommonData->mPipeName,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		BUFSIZE * sizeof(char),
		BUFSIZE * sizeof(char),
		PIPE_TIMEOUT,
		NULL);

	if (pCommonData->hPipeInst == INVALID_HANDLE_VALUE) {
		//printf("CreateNamedPipe failed with %d.\n", GetLastError());
		delete pCommonData;
		return FALSE;
	}

	//read
	LPPIPEINST instRead = new PIPEINST();
	hEventList[index * 2] = CreateEvent(NULL,
		TRUE,
		TRUE,
		NULL);
	instRead->oOverlap.hEvent = hEventList[index * 2];


	//write
	LPPIPEINST instWrite = new PIPEINST();
	hEventList[index * 2 + 1] = CreateEvent(NULL,
		TRUE,
		FALSE,
		NULL);
	instWrite->oOverlap.hEvent = hEventList[index * 2 + 1];

	PipeChannel *pipeChannel = new PipeChannel(instWrite, instRead, pCommonData);
	pipeChannel->Init();
	mChannelList.push_back(pipeChannel);

	return TRUE;
}

BOOL PipeSever::Run() {

	BOOL fSuccess = FALSE;
	DWORD dwWait = WaitForMultipleObjectsEx(
		mPipeSize * 2,    // number of event objects 
		hEventList,      // array of event objects 
		FALSE,        // does not wait for all 
		INFINITE,	 // waits indefinitely
		TRUE);			// Alertable readfileex,writefileex

	DWORD i = dwWait - WAIT_OBJECT_0;  // determines which pipe 

	DWORD dwError = GetLastError();

	if (dwWait == WAIT_IO_COMPLETION) {
		return FALSE;
	}

	if (i < 0 || i >(mPipeSize * 2 - 1)) {
		//printf("Index out of range.\n");
		return FALSE;
	}



	DWORD index = i / 2;
	BOOL bReader = (i % 2 == 0);

	PipeChannel *channel = GetChannel(index);

	if (channel == nullptr) {
		return FALSE;
	}


	if (bReader) {
		response.UpateInfo(channel);
	}
	else {
		request.UpateInfo(channel);
	}

	ResetEvent(hEventList[i]);

	return TRUE;
}

BOOL PipeSever::GetStatus(LPPIPEINST pipeInst) {
	return TRUE;
}


PipeChannel* PipeSever::GetChannel(DWORD id) {
	for (auto &it : mChannelList) {
		if (it->id() == id) {
			return it;
		}
	}
	return nullptr;
}


DWORD WINAPI PipeSever::RequestThread(LPVOID lpParam) {
	PipeSever *pThis = (PipeSever*)lpParam;
	DoAction(pThis->request, pThis->mChannelList);
	return 0;
}


DWORD WINAPI PipeSever::ResponseThread(LPVOID lpParam) {
	PipeSever *pThis = (PipeSever*)lpParam;
	DoAction(pThis->response, pThis->mChannelList);
	return 0;
}
