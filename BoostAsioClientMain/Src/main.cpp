#include <thread>
#include "BoostAsioClient/SyncClient.h"
#include "BoostAsioClient/AsyncClient.h"
#include "BoostAsioClient/StacklessCoroutineClient.h"
#include "BoostAsioClient/StackfulCoroutineClient.h"
#include <iostream>
#include <cstdio>

using namespace BoostAsioClient;

#define STACKFUL_COROUTINE_TEST 1

int main(int argc, char* argv[])
{
#ifdef SYNC_TEST
	std::thread clientThread1(RunSyncClient, "John");
	std::thread clientThread2(RunSyncClient, "Shawn");
	clientThread1.join();
	clientThread2.join();
	std::cout<<"Press any key to Continue...\n";
	getchar();
#elif ASYNC_TEST
    TestAsyncClient();
	std::cout<<"Press any key to Continue...\n";
	getchar();
#elif STACKLESS_COROUTINE_TEST
	TestStacklessCoroutineClient();
	std::cout<<"Press any key to Continue...\n";
	getchar();
#elif STACKFUL_COROUTINE_TEST
	TestStackfulCoroutineClient();
	std::cout<<"Press any key to Continue...\n";
	getchar();
#endif
	return 0;
}