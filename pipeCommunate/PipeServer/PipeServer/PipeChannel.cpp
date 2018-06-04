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
		switch (eventInfo->Type)
		{
		case enWriteComplete:
			printf("id %d:write data completely\n", eventInfo->Id);
			break;
		default:
			break;
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
		switch (eventInfo->Type)
		{
		case enNewConnect:
			printf("id %d:new client connected\n", eventInfo->Id);
			break;
		case enReadComplete:
			printf("id %d:%s\n", eventInfo->Id, (char*)eventInfo->Data);
			writer->WriteData((PVOID)eventInfo->Data, eventInfo->Size);
			break;
		case enDisconnected:
			printf("id %d: pipe disconnected\n", eventInfo->Id);
			break;
		default:
			break;
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