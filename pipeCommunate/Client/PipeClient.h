#pragma once
#include <windows.h>
#define BUFSIZE (1024*100)

typedef struct  {
	OVERLAPPED oOverlap = {0};
	PVOID	pData[BUFSIZE];
	DWORD   dwSize = BUFSIZE;
}PIPEINST;

class PipeClient
{
public:
	explicit PipeClient(DWORD dwID);
	~PipeClient();
	
	BOOL Init();
	BOOL SetPipeAttrib();
	VOID Run();

	BOOL WriteData(PVOID pData, DWORD dwSize);
	BOOL GetWriteStatus();
	VOID OnWrite();


	BOOL ReadData();
	BOOL GetReadStatus();
	VOID OnRead(PVOID pData, DWORD dwBytesRead);

private:
	HANDLE mhPipe = INVALID_HANDLE_VALUE;
	char mPipeName[MAX_PATH];
	HANDLE mhEvent[2] = {nullptr};
	PIPEINST mPipeInst[2];
	DWORD mId;
};

