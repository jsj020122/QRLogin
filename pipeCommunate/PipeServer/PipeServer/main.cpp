#include <windows.h>
#include "PipeServerManager.h"
int main(VOID) {

	std::shared_ptr<PipeServerManager> pManger =
		std::make_shared<PipeServerManager>();

	pManger->Init();
	pManger->Run();

	return 1;
}