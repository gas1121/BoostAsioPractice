#include <BoostAsioServer/SyncServer.h>

#include <thread>
#include <catch.hpp>

using namespace BoostAsioServer;
using namespace boost::asio;
using boost::system::error_code;
using std::placeholders::_1;
using std::placeholders::_2;

namespace
{
    const unsigned short kPort = 8001;

    void ServerInstance()
    {
        io_service service;
        ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), kPort));
        SyncClientConnectionPTR client(new SyncClientConnection());
        acceptor.accept(client->Socket());
        client->UpdateLastPingTime();
        while (client->Socket().is_open())
        {
            client->AnswerToClient();
        }
    }
}

TEST_CASE("SyncServerTest", "[Asio][Sync][Server]")
{
    std::thread serverThread(ServerInstance);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    io_service clientService;
    ip::tcp::socket clientSocket(clientService);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), kPort);
    error_code ec;

    clientSocket.connect(ep, ec);
    INFO("connect:"+ec.message());
    REQUIRE(!ec);

    std::string writeBuffer = "login testClient\n";
    write(clientSocket, buffer(writeBuffer), ec);
    INFO("write:"+ec.message());
    REQUIRE(!ec);

    boost::asio::streambuf readBuffer;
    size_t readSize = std::string("login ok\n").size();
    read(clientSocket, readBuffer, boost::asio::transfer_exactly(readSize), ec);
    INFO("read:"+ec.message());
    REQUIRE(!ec);
    std::istream in(&readBuffer);
    std::string word;
    in >> word;
    REQUIRE(word == "login");
    in >> word;
    REQUIRE(word == "ok");
    readBuffer.consume(readBuffer.size());

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    writeBuffer = "ping\n";
    write(clientSocket, buffer(writeBuffer), ec);
    INFO("write:"+ec.message());
    REQUIRE(!ec);

    readSize = std::string("ping ok\n").size();
    read(clientSocket, readBuffer, boost::asio::transfer_exactly(readSize), ec);
    INFO("read:"+ec.message());
    REQUIRE(!ec);
    in >> word;
    REQUIRE(word == "ping");
    in >> word;
    readBuffer.consume(readBuffer.size());

    std::this_thread::sleep_for(std::chrono::milliseconds(5500));
    writeBuffer = "ping\n";
    write(clientSocket, buffer(writeBuffer), ec);
    INFO("write:"+ec.message());
    REQUIRE(!ec);

    readSize = std::string("ping ok\n").size();
    read(clientSocket, readBuffer, boost::asio::transfer_exactly(readSize), ec);
    INFO("read:"+ec.message());
    REQUIRE(ec);

    clientSocket.shutdown(ip::tcp::socket::shutdown_both, ec);
    clientSocket.close(ec);
    serverThread.join();
}