#pragma once
#include "PipeChannel.h"

template <typename T, bool type>
class RequestOrResponse {
private:
	HANDLE getHandle(T *obj, Int2Type<true>) {
		return obj->getReqestHandle();
	}
	void onAction(T *obj, Int2Type<true>) {
		return obj->onRequest();
	}
	void UpateInfo(T *obj, Int2Type<true>) {
		return obj->UpdateWriteInfo();
	}

	HANDLE getHandle(T *obj, Int2Type<false>) {
		return obj->getResponseHandle();
	}
	void onAction(T *obj, Int2Type<false>) {
		return obj->onResponse();
	}
	void UpateInfo(T *obj, Int2Type<false>) {
		return obj->UpdateReadInfo();
	}

public:
	HANDLE getHandle(T *obj) {
		return getHandle(obj, Int2Type<type>());
	}
	void onAction(T *obj) {
		return onAction(obj, Int2Type<type>());
	}
	void UpateInfo(T *obj) {
		return UpateInfo(obj, Int2Type<type>());
	}
};

class PipeSever: public CommInterface{
public:
	virtual ~PipeSever();

public:
	BOOL CreateServer();
	
public:
	virtual void RunLoop() final;
	virtual BOOL SendData(PVOID Data, DWORD dwSize, DWORD id) final;
	virtual BOOL SubscribeEvent(std::shared_ptr<CommCallBack> &eventCallBack,
		DWORD id) final;

private:
	static DWORD WINAPI RequestThread(LPVOID lpParam);
	static DWORD WINAPI ResponseThread(LPVOID lpParam);
	BOOL CreatePipe(UINT32 index);
	BOOL Run();
	BOOL GetStatus(LPPIPEINST pipeInst);
	PipeChannel* GetChannel(DWORD id);
	VOID Uninit();
	BOOL SubscribeEvent(std::shared_ptr<CommCallBack> &eventCallBack);

private:
	RequestOrResponse<PipeChannel, true> request;
	RequestOrResponse<PipeChannel, false> response;

private:
	HANDLE	*hEventList = nullptr;
	DWORD	mPipeSize = 10;
	std::vector<PipeChannel* > mChannelList;
	HANDLE	mResponseThreadHandle = NULL;
	HANDLE	mRequestThreadHandle = NULL;
	std::map<DWORD, DWORD> mIndexIDs;
};

template <class T>
static VOID DoAction(T &obj,
	std::vector<PipeChannel* > &channelList) {
	DWORD dwSize = (DWORD)channelList.size();
	if (dwSize < 1) {
		return;
	}
	HANDLE *eventList = new HANDLE[dwSize];
	for (DWORD i = 0; i < dwSize; i++) {
		eventList[i] = obj.getHandle(channelList[i]);
	}

	do {
		DWORD result = WaitForMultipleObjects(dwSize, eventList, FALSE, INFINITE);
		if (WAIT_TIMEOUT == result) {
			continue;
		}

		DWORD index = result - WAIT_OBJECT_0;
		if (index >= 0 && index < dwSize) {
			auto channel = channelList[index];
			obj.onAction(channel);
			ResetEvent(eventList[index]);
		}
		else {
			break;
		}
	} while (true);

	delete eventList;
}