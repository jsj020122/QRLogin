#pragma once

#include "PipeInterface.h"

class PipeReader
{
public:
	PipeReader(LPPIPEINST pipeInst,
		LPCOMMONDATA pCommonData);
	~PipeReader();

	BOOL ConnectToNewClient();
	VOID DisconnectAndReconnect();

	static VOID WINAPI CompletedReadRoutine(DWORD dwErr,
		DWORD cbWritten,
		LPOVERLAPPED lpOverLap);


public:
	VOID UpdateReadInfo();
	VOID Create();
	HANDLE GetEventHandle();

	BOOL GetResponseEventInfo(std::shared_ptr<EVENTINFO> &result);
	VOID AddResponseEventInfo(std::shared_ptr<EVENTINFO> &eventInfo);
	BOOL ReadData();

public:
	DWORD OnReadComplete();
	VOID OnNewConnect();
	DWORD OnRead(BYTE* pBuf, DWORD &Size);
	VOID OnDisconnected();

private:
	LPPIPEINST mReadPipe;
	std::list<std::shared_ptr<EVENTINFO>>  mResponseList;
	HANDLE	mResponseEvent = NULL;
	CCriticalSection mResponseSection;
	LPCOMMONDATA mCommonData;
};

