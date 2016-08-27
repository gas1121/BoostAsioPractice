#include <thread>
#include "SyncServer.h"
#include "AsyncServer.h"
#include <iostream>

using namespace BoostAsioServer;

#define SYNC_TEST 1

int main(int argc, char* argv[])
{
#ifdef SYNC_TEST
	TestSyncServer();
#elif ASYNC_TEST
	TestAsyncServer();
#endif
	
	return 0;
}