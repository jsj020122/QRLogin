#pragma once
//#include "PipeChannel.h"
#include <vector>
#include "PipeServer.h"

class PipeServerManager
{
public:
	PipeServerManager();
	~PipeServerManager();
	VOID Init();
	VOID Run();

private:
	std::shared_ptr<PipeSever> mPipeServer;
};

