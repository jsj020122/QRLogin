#include "PipeReader.h"


PipeReader::PipeReader(LPPIPEINST pipeInst,
	LPCOMMONDATA pCommonData) {
	mCommonData = pCommonData;
	mReadPipe = pipeInst;
	mReadPipe->pUserContext = this;
}
VOID PipeReader::Create() {
	mResponseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	mReadPipe->fPendingIO = ConnectToNewClient();
	mReadPipe->dwState = mReadPipe->fPendingIO ?
		CONNECTING_STATE : // still connecting 
		READING_STATE;     // ready to read 
}

PipeReader::~PipeReader()
{
	if (mResponseEvent != NULL) {
		CloseHandle(mResponseEvent);
	}

	if (mReadPipe != NULL) {
		delete mReadPipe;
		mReadPipe = NULL;
	}

}

VOID PipeReader::OnNewConnect() {
	AddResponseEventInfo(std::make_shared<EVENTINFO>(nullptr,
		0,
		enNewConnect,
		mCommonData->id));
	SetEvent(mResponseEvent);
	ReadData();
}

BOOL PipeReader::ReadData() {
	BOOL fRead = ReadFileEx(
		mCommonData->hPipeInst,
		mReadPipe->Data,
		BUFSIZE * sizeof(TCHAR),
		(LPOVERLAPPED)mReadPipe,
		(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedReadRoutine);

	if (!fRead) {
		DisconnectAndReconnect();
	}
	else {
		mReadPipe->dwState = READING_STATE;
	}
	return TRUE;
}

DWORD PipeReader::OnRead(BYTE* pBuf, DWORD &Size) {
	return 0;
}

VOID WINAPI PipeReader::CompletedReadRoutine(DWORD dwErr,
	DWORD cbRead,
	LPOVERLAPPED lpOverLap) {

	LPPIPEINST pInst = (LPPIPEINST)lpOverLap;
	PipeReader *pThis = (PipeReader*)pInst->pUserContext;
	pInst->size = cbRead;

	if ((dwErr == 0)
		&& (cbRead != 0)) {
		pThis->OnReadComplete();
	}
	else if (dwErr != 0) {
		pThis->OnDisconnected();
	}
}

VOID PipeReader::OnDisconnected() {
	AddResponseEventInfo(std::make_shared<EVENTINFO>(nullptr,
		0,
		enDisconnected,
		mCommonData->id));
	SetEvent(mResponseEvent);
	DisconnectAndReconnect();
}
DWORD PipeReader::OnReadComplete() {
	AddResponseEventInfo(std::make_shared<EVENTINFO>(mReadPipe->Data,
		mReadPipe->size,
		enReadComplete,
		mCommonData->id));
	SetEvent(mResponseEvent);
	ReadData();
	return 0;
}
HANDLE PipeReader::GetEventHandle() {
	return mResponseEvent;
}

VOID PipeReader::UpdateReadInfo() {

	BOOL fSuccess = FALSE;
	DWORD cbRet = 0;
	DWORD dwError = 0;

	if (mReadPipe->fPendingIO) {
		fSuccess = GetOverlappedResult(
			mCommonData->hPipeInst, // handle to pipe 
			&mReadPipe->oOverlap, // OVERLAPPED structure 
			&cbRet,            // bytes transferred 
			FALSE);            // do not wait 

		if (fSuccess && mReadPipe->dwState == CONNECTING_STATE) {
			OnNewConnect();
		}
	}
}

BOOL PipeReader::ConnectToNewClient() {

	BOOL fConnected = ConnectNamedPipe(mCommonData->hPipeInst, 
		&mReadPipe->oOverlap);

	if (fConnected) {
		//printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return FALSE;
	}

	BOOL fPendingIO = FALSE;

	switch (GetLastError())
	{
	case ERROR_IO_PENDING:
		fPendingIO = TRUE;
		break;
	case ERROR_PIPE_CONNECTED:
		SetEvent(mReadPipe->oOverlap.hEvent);
		fPendingIO = FALSE;
		break;
	default:
		//printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}

	return fPendingIO;
}

VOID PipeReader::DisconnectAndReconnect()
{
	// Disconnect the pipe instance. 
	mCommonData->pipeStatus = enPipeDisconnect;
	if (!DisconnectNamedPipe(mCommonData->hPipeInst))
	{
		printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
	}

	// Call a subroutine to connect to the new client. 
	mReadPipe->fPendingIO = ConnectToNewClient();
	mReadPipe->dwState = mReadPipe->fPendingIO ?
		CONNECTING_STATE : // still connecting 
		READING_STATE;     // ready to read 
	mCommonData->pipeStatus = enPipeConnected;
}

BOOL PipeReader::GetResponseEventInfo(std::shared_ptr<EVENTINFO> &result){
	CAutoLockCS lock(mResponseSection);
	if (mResponseList.empty()) {
		return FALSE;
	}
	result = mResponseList.back();
	mResponseList.pop_back();
	return TRUE;
}
VOID PipeReader::AddResponseEventInfo(std::shared_ptr<EVENTINFO> &eventInfo) {
	CAutoLockCS lock(mResponseSection);
	mResponseList.push_back(eventInfo);
}
