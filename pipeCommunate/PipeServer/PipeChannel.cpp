#include "PipeChannel.h"

PipeChannel::PipeChannel(LPPIPEINST writeInst,
	LPPIPEINST readInst,
	LPCOMMONDATA commdata) {
	reader = new PipeReader(readInst, commdata);
	writer = new PipeWriter(writeInst, commdata);
	commonData = commdata;
}


PipeChannel::~PipeChannel() {
	if (reader != nullptr) {
		delete reader;
		reader = nullptr;
	}

	if (writer != nullptr) {
		delete writer;
		writer = nullptr;
	}

	if (commonData != nullptr) {
		delete commonData;
		commonData = nullptr;
	}
}
DWORD PipeChannel::id() {
	return commonData->id;
}

VOID PipeChannel::Init() {
	reader->Create();
	writer->Create();
}

VOID PipeChannel::onRequest() {
	BOOL bMore;
	do {
		std::shared_ptr<EVENTINFO> eventInfo;
		bMore = writer->GetRequestEventInfo(eventInfo);

		if (!bMore) {
			break;
		}

		for (auto &it : eventCallBackList) {
			if (!it.expired()) {
				it.lock()->onRequest(eventInfo);
			}
		}
	} while (bMore);
}

VOID PipeChannel::onResponse() {
	BOOL bMore;
	do {
		std::shared_ptr<EVENTINFO> eventInfo;
		bMore = reader->GetResponseEventInfo(eventInfo);
		if (!bMore) {
			break;
		}

		for (auto &it : eventCallBackList) {
			if (!it.expired()) {
				it.lock()->onResponse(eventInfo);
			}
		}
	} while (bMore);
}

HANDLE PipeChannel::getReqestHandle() {
	return writer->GetEventHandle();
}

VOID PipeChannel::UpdateWriteInfo() {
	return writer->UpdateWriteInfo();
}

HANDLE PipeChannel::getResponseHandle() {
	return reader->GetEventHandle();
}
VOID  PipeChannel::UpdateReadInfo() {
	return reader->UpdateReadInfo();
}
BOOL PipeChannel::SendData(PVOID Data,
	DWORD dwSize) {
	return writer->WriteData(Data, dwSize);
}

void PipeChannel::SubscribeEvent(std::shared_ptr<CommCallBack> &eventCallBack) {
	eventCallBackList.push_back(eventCallBack);
}