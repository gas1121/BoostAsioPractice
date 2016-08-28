#include "BoostAsioClient/AsyncClient.h"
#include <thread>
#include <iostream>

#include <Core/Core.hpp>

using namespace boost::asio;
using namespace BoostAsioPractice;

namespace BoostAsioClient {

	void TestAsyncClient()
	{
		io_service service;
		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
		std::vector<std::string> clientList;
		clientList.push_back("John");
		clientList.push_back("James");
		clientList.push_back("Lucy");
		for (auto& clientName: clientList)
		{
			AsyncClient::Start(ep, clientName, &service);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		service.run();
	}

	AsyncClient::AsyncClientPTR AsyncClient::Start(tcp::endpoint ep, const std::string& userName, io_service *service)
	{
		AsyncClientPTR newConnection(new AsyncClient(userName, service));
		newConnection->Start(ep);
		return newConnection;
	}

	void AsyncClient::Stop()
	{
		if (!started_)
		{
			return;
		}
		error_code ec;
		socket_.close(ec);
		started_ = false;
	}

	bool AsyncClient::Started()
	{
		return started_;
	}

    AsyncClient::AsyncClient(const std::string& userName, io_service *service)
        :socket_(*service), started_(true), userName_(userName), timer_(*service)
    {

    }

    void AsyncClient::Start(tcp::endpoint ep)
    {
		socket_.async_connect(ep, std::bind(&AsyncClient::OnConnect, shared_from_this(), std::placeholders::_1));
    }

    void AsyncClient::OnConnect(const error_code& err)
    {
        if (!err)
        {
            DoWrite("login "+ userName_+"\n");
        }
        else
        {
            Stop();
        }
    }

	void AsyncClient::DoWrite(const std::string& message)
	{
		if (!started_)
		{
			return;
		}
		assert(message.size() < kBufferSize);
		std::copy(message.begin(), message.end(), writeBuffer_);
		socket_.async_write_some(buffer(writeBuffer_, message.size()), std::bind(&AsyncClient::OnWrite, shared_from_this(),
			std::placeholders::_1, std::placeholders::_2));
	}

	void AsyncClient::OnWrite(const error_code& err, size_t bytes)
	{
		DoRead();
	}

    void AsyncClient::DoRead()
    {
		async_read(socket_, buffer(readBuffer_), std::bind(&AsyncClient::IsReadComplete, shared_from_this(),
            std::placeholders::_1,std::placeholders::_2), std::bind(&AsyncClient::OnRead, shared_from_this(), 
                std::placeholders::_1,std::placeholders::_2));
    }

    size_t AsyncClient::IsReadComplete(const error_code& err, size_t bytes)
    {
        if (err)
        {
            return 0;
        }
        bool found = std::find(readBuffer_, readBuffer_ + bytes, '\n') < readBuffer_ + bytes;
        return found ? 0 : 1;
    }

    void AsyncClient::OnRead(const error_code& err, size_t bytes)
    {
		if (err)
		{
			Stop();
		}
		if (!Started())
		{
			return;
		}
		std::string message(readBuffer_, bytes);
		if (message.find("login ")==0)
		{
			OnLogin();
		}
		else if (message.find("ping") == 0)
		{
			OnPing(message);
		}
		else if (message.find("clients ")==0)
		{
			OnClients(message);
		}
    }

	void AsyncClient::OnLogin()
	{
		DoAskClients();
	}

	void AsyncClient::OnPing(const std::string& message)
	{
		std::istringstream in(message);
		std::string answer;
		in >> answer >> answer;
		if (answer=="client_list_changed")
		{
			DoAskClients();
		}
		else
		{
			PostponePing();
		}
	}

	void AsyncClient::OnClients(const std::string& message)
	{
		std::string clients = message.substr(8);
		std::cout << userName_ << " , new client list:" << clients;
		PostponePing();
	}

	void AsyncClient::DoAskClients()
	{
		DoWrite("ask_clients\n");
	}

	void AsyncClient::PostponePing()
	{
		int sleepTime = RandomNumber<int>(0, 7000);
		error_code ec;
		timer_.expires_from_now(std::chrono::milliseconds(sleepTime),ec);
		timer_.async_wait(std::bind(&AsyncClient::DoPing, shared_from_this()));
	}

	void AsyncClient::DoPing()
	{
		DoWrite("ping\n");
	}
}