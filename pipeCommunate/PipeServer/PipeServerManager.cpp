#include "PipeServerManager.h"



PipeServerManager::PipeServerManager(){
}


PipeServerManager::~PipeServerManager(){
}

VOID PipeServerManager::Init() {
	mPipeServer = std::make_shared<PipeSever>();
	mPipeServer->CreateServer();
}
VOID PipeServerManager::Run() {
	mPipeServer->RunLoop();
}
