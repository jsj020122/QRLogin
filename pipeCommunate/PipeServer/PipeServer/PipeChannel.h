#pragma once
#include "PipeInterface.h"
#include "PipeWriter.h"
#include "PipeReader.h"
class PipeChannel{
public:
	PipeChannel(LPPIPEINST writeInst,
		LPPIPEINST readInst,
		LPCOMMONDATA commdata);
	~PipeChannel();

public:
	DWORD id();
	VOID Init();
	VOID onRequest();
	VOID onResponse();

	HANDLE getReqestHandle();
	VOID   UpdateReadInfo();

	HANDLE getResponseHandle();
	VOID   UpdateWriteInfo();


private:
	PipeReader* reader;
	PipeWriter* writer;
	LPCOMMONDATA commonData;
};

