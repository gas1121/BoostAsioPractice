#include <thread>
#include "BoostAsioClient/SyncClient.h"
#include "BoostAsioClient/AsyncClient.h"
#include "BoostAsioClient/StacklessCoroutineClient.h"
#include "BoostAsioClient/StackfulCoroutineClient.h"
#include <iostream>

using namespace BoostAsioClient;

#define STACKFUL_COROUTINE_TEST 1

int main(int argc, char* argv[])
{
#ifdef SYNC_TEST
	std::thread clientThread1(RunSyncClient, "John");
	std::thread clientThread2(RunSyncClient, "Shawn");
	clientThread1.join();
	clientThread2.join();
	system("pause");
#elif ASYNC_TEST
    TestAsyncClient();
	system("pause");
#elif STACKLESS_COROUTINE_TEST
	TestStacklessCoroutineClient();
	system("pause");
#elif STACKFUL_COROUTINE_TEST
	TestStackfulCoroutineClient();
	system("pause");
#endif
	return 0;
}