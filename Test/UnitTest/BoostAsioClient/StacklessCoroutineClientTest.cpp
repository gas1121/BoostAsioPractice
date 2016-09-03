#include <BoostAsioClient/StacklessCoroutineClient.h>

#include <thread>
#include <catch.hpp>
#include <BoostAsioClient/ClientTestUtility.h>

using namespace BoostAsioClient;
using namespace boost::asio;
using boost::system::error_code;
using std::placeholders::_1;
using std::placeholders::_2;

namespace
{
    const unsigned short kPort = 8001;
    const std::string clientName = "testClient";

    void ClientInstance()
    {
        io_service service;
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), kPort);
        StacklessCoroutineClient::Start(ep, clientName, &service);
        service.run();
    }
}

TEST_CASE("StacklessCoroutineClientTest", "[Asio][Async][Client]")
{
    std::thread serverThread(MockServer);
    std::thread clientThread(ClientInstance);

    serverThread.join();
    clientThread.join();
}