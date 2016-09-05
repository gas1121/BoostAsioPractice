#include <BoostAsioClient/ClientTestUtility.h>

#include <boost/asio.hpp>
#include <catch.hpp>

using namespace boost::asio;
using boost::system::error_code;

namespace
{
    const std::string clientName = "testClient";
}

void MockServer(unsigned short port)
{
    io_service service;
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port));
    ip::tcp::socket serverSocket(service);
    error_code ec;
    acceptor.accept(serverSocket, ec);
    INFO("accept:" + ec.message());
    REQUIRE(!ec);

    boost::asio::streambuf readBuffer;
    size_t readSize = std::string("login " + clientName + "\n").size();
    read(serverSocket, readBuffer, boost::asio::transfer_exactly(readSize), ec);
    INFO("read:" + ec.message());
    REQUIRE(!ec);
    std::istream in(&readBuffer);
    std::string word;
    in >> word;
    REQUIRE(word == "login");
    in >> word;
    REQUIRE(word == "testClient");
    readBuffer.consume(readBuffer.size());

    std::string writeBuffer = "login ok\n";
    write(serverSocket, buffer(writeBuffer), ec);
    REQUIRE(!ec);

    readSize = std::string("ask_clients\n").size();
    read(serverSocket, readBuffer, boost::asio::transfer_exactly(readSize), ec);
    INFO("read:" + ec.message());
    REQUIRE(!ec);
    in >> word;
    REQUIRE(word == "ask_clients");

    writeBuffer = "clients " + clientName + "\n";
    write(serverSocket, buffer(writeBuffer), ec);
    REQUIRE(!ec);

    readSize = std::string("ping\n").size();
    read(serverSocket, readBuffer, boost::asio::transfer_exactly(readSize), ec);
    INFO("read:" + ec.message());
    REQUIRE(!ec);
    in >> word;
    REQUIRE(word == "ping");

    writeBuffer = "ping ok\n";
    write(serverSocket, buffer(writeBuffer), ec);
    REQUIRE(!ec);

    readSize = std::string("ping\n").size();
    read(serverSocket, readBuffer, boost::asio::transfer_exactly(readSize), ec);
    INFO("read:" + ec.message());
    REQUIRE(!ec);
    in >> word;
    REQUIRE(word == "ping");

    serverSocket.shutdown(ip::tcp::socket::shutdown_both, ec);
    serverSocket.close(ec);
    REQUIRE(!ec);
}