#include <windows.h>
#include <memory>
#include "ComInteface.h"

class test :public CommCallBack,
	public std::enable_shared_from_this<test> {
public:
	virtual VOID onRequest(std::shared_ptr<EVENTINFO> &eventInfo) final {
		switch (eventInfo->Type)
		{
		case enWriteComplete:
			printf("id %d:write data completely\n", eventInfo->Id);
			break;
		default:
			break;
		}
	}
	virtual VOID onResponse(std::shared_ptr<EVENTINFO> &eventInfo) final {
		switch (eventInfo->Type)
		{
		case enNewConnect:
			printf("id %d:new client connected\n", eventInfo->Id);
			break;
		case enReadComplete:
			printf("id %d:%s\n", eventInfo->Id, (char*)eventInfo->Data);
			pComInterface->SendData((PVOID)eventInfo->Data, 
				eventInfo->Size,
				eventInfo->Id);
			break;
		case enDisconnected:
			printf("id %d: pipe disconnected\n", eventInfo->Id);
			break;
		default:
			break;
		}
	}

	VOID run() {
		pComInterface = getPipeInstance();
		std::shared_ptr<CommCallBack> pThis = shared_from_this();
		pComInterface->SubscribeEvent(pThis, -1);
		pComInterface->RunLoop();
	}
private:
	std::shared_ptr<CommInterface> pComInterface;
};


int main(VOID) {
	std::shared_ptr<test> pTestInst = std::make_shared<test>();
	pTestInst->run();
	return 1;
}