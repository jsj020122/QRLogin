#pragma once

#include "PipeInterface.h"

class PipeWriter{
public:
	PipeWriter(LPPIPEINST pipeInst,
		LPCOMMONDATA pCommonData);
	~PipeWriter();

	static VOID WINAPI CompletedWriteRoutine(DWORD dwErr,
		DWORD cbWritten,
		LPOVERLAPPED lpOverLap);

public:
	BOOL GetRequestEventInfo(std::shared_ptr<EVENTINFO> &result);
	VOID AddRequestEventInfo(std::shared_ptr<EVENTINFO> &eventInfo);

	VOID DispatchRequest(std::shared_ptr<EVENTINFO>);
	VOID UpdateWriteInfo();
	VOID OnWriteComplete(LPPIPEINST pInst);
	VOID Create();
	HANDLE GetEventHandle();
	VOID WriteData(PVOID data, DWORD dwSize);
	
public:

	//pipeWriterInterface
	virtual DWORD OnWriteComplete() final;
	virtual BOOL OnWriteError(DWORD dwError) final;

private:
	LPPIPEINST mWritePipe;
	std::list<std::shared_ptr<EVENTINFO>>  mRequestList;
	HANDLE	mRequestEvent = NULL;
	CCriticalSection mRequestSection;
	LPCOMMONDATA mCommonData = NULL;
};

