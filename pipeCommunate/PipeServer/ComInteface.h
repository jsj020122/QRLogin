#pragma once
#include <windows.h>
typedef enum {
	enReadData,
	enReadComplete,
	enReadError,
	enWriteData,
	enWriteComplete,
	enWriteError,
	enNewConnect,
	enDisconnected,
	enUnKnown
}EVENTTEYPE;

struct EVENTINFO {
	EVENTINFO(PVOID data,
		DWORD dwSize,
		EVENTTEYPE type,
		DWORD id) {
		Type = type;
		Id = id;
		if (data != nullptr && dwSize > 0) {
			Size = dwSize+1;
			Data = new BYTE[Size];
			memcpy(Data, data, Size);
			((BYTE*)Data)[Size-1] = 0;
		}
	}

	~EVENTINFO() {
		if (Data != nullptr) {
			delete Data;
			Data = nullptr;
			Size = 0;
		}
		Type = enUnKnown;
	}
	EVENTTEYPE Type = enUnKnown;
	PVOID   Data = nullptr;
	DWORD	Size = 0;
	DWORD	Id = -1;
};



class CommCallBack {
public:
	virtual VOID onRequest(std::shared_ptr<EVENTINFO> &eventInfor) = 0;
	virtual VOID onResponse(std::shared_ptr<EVENTINFO> &eventInfor) = 0;
};

class CommInterface {
public:
	virtual void RunLoop() = 0;
	virtual BOOL SendData(PVOID Data, DWORD dwSize, DWORD id) = 0;
	virtual BOOL SubscribeEvent(std::shared_ptr<CommCallBack> &eventCallBack,
		DWORD id) = 0;
};

std::shared_ptr<CommInterface> getPipeInstance();

