#pragma once
#include <windows.h>
#include <memory>

#include <vector>
#include <map>
#include <list>
#include "ComInteface.h"

#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2 
#define PENDING_STATE 3

#define INSTANCES 4 
#define PIPE_TIMEOUT 5000
#define BUFSIZE (1024*100)

typedef enum {
	enPipeReady,
	enPipeDisconnect,
	enPipeConnected,
	enPipeUnknown
}PIPESTATUS;

typedef struct {
	HANDLE		hPipeInst = INVALID_HANDLE_VALUE;
	DWORD		id = -1;
	char		mPipeName[MAX_PATH];
	PIPESTATUS  pipeStatus = enPipeUnknown;
}COMMONDATA, *LPCOMMONDATA;

typedef struct
{
	OVERLAPPED	oOverlap;
	char		Data[BUFSIZE] = { 0 };
	DWORD		size = 0;
	DWORD		dwState = 0;
	BOOL		fPendingIO = FALSE;
	PVOID		pUserContext = NULL;
} PIPEINST, *LPPIPEINST;


template <int v>
class Int2Type{
	enum { value = v };
};

class CCriticalSection {
public:
	CCriticalSection() {
		m_bInited = FALSE;
		m_bInited = ::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
	}
	virtual ~CCriticalSection() {
		if (m_bInited)
			::DeleteCriticalSection(&m_cs);
	}

	void Lock() {
		if (m_bInited)
			EnterCriticalSection(&m_cs);
	}
	void Unlock() {
		if (m_bInited)
			LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
	BOOL m_bInited;
};

class CAutoLockCS
{
private:
	CCriticalSection&	m_CS;

public:
	explicit CAutoLockCS(CCriticalSection& cs) : m_CS(cs)
	{
		m_CS.Lock();
	}

	~CAutoLockCS()
	{
		m_CS.Unlock();
	}
};
