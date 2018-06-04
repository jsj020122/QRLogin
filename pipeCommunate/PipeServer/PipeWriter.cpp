#include "PipeWriter.h"


PipeWriter::PipeWriter(LPPIPEINST pipeInst,
	LPCOMMONDATA pCommonData) {
	mCommonData = pCommonData;
	mWritePipe = pipeInst;
	mWritePipe->pUserContext = this;
}

VOID PipeWriter::Create() {
	mRequestEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PipeWriter::~PipeWriter()
{
	if (mRequestEvent != NULL) {
		CloseHandle(mRequestEvent);
	}

	if (mWritePipe != NULL) {
		delete mWritePipe;
		mWritePipe = NULL;
	}
}


VOID WINAPI PipeWriter::CompletedWriteRoutine(DWORD dwErr,
	DWORD cbWritten,
	LPOVERLAPPED lpOverLap) {

	LPPIPEINST pInst = (LPPIPEINST)lpOverLap;
	if ((dwErr == 0)
		&& (cbWritten == pInst->size)) {
		PipeWriter *pThis = (PipeWriter*)pInst->pUserContext;
		pThis->OnWriteComplete(pInst);
	}
}

VOID PipeWriter::OnWriteComplete(LPPIPEINST pInst) {
	AddRequestEventInfo(std::make_shared<EVENTINFO>(mWritePipe->Data,
		mWritePipe->size,
		enWriteComplete,
		mCommonData->id));
	SetEvent(mRequestEvent);
}


BOOL PipeWriter::GetRequestEventInfo(std::shared_ptr<EVENTINFO> &result) {
	CAutoLockCS lock(mRequestSection);
	if (mRequestList.empty()) {
		return FALSE;
	}
	result = mRequestList.back();
	mRequestList.pop_back();
	return TRUE;
}

VOID PipeWriter::AddRequestEventInfo(std::shared_ptr<EVENTINFO> &eventInfo) {
	CAutoLockCS lock(mRequestSection);
	mRequestList.push_front(eventInfo);
}

VOID PipeWriter::UpdateWriteInfo() {

}
//pipeWriterInterface
DWORD PipeWriter::OnWriteComplete() {
	return 0;
}
BOOL PipeWriter::OnWriteError(DWORD dwError) {
	return TRUE;
}
HANDLE PipeWriter::GetEventHandle() {
	return mRequestEvent;
}
BOOL PipeWriter::WriteData(PVOID data, DWORD dwSize) {
	memcpy(mWritePipe->Data, data, dwSize);
	mWritePipe->size = dwSize;

	BOOL fWrite = WriteFileEx(
		mCommonData->hPipeInst,
		mWritePipe->Data,
		mWritePipe->size,
		(LPOVERLAPPED)(mWritePipe),
		(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedWriteRoutine);
	return fWrite;
}