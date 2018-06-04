#include "PipeClient.h"
#include <memory>
#include <iostream>
#include <sstream>

std::shared_ptr<PipeClient>  client;
DWORD WINAPI threadFunc(
	LPVOID lpThreadParameter
	) {
	do {
		char a[100];
		std::cin >> a;
		client->WriteData((PVOID)a, strlen(a));
	} while (1);
}
int main() {
	CreateThread(nullptr, 0, threadFunc, nullptr, 0, nullptr);
	client = std::make_shared<PipeClient>(1);
	client->Run();
}