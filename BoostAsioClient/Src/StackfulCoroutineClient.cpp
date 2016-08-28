#include "BoostAsioClient/StackfulCoroutineClient.h"
#include <thread>
#include <iostream>

#include <Core/Core.hpp>

using namespace boost::asio;
using namespace BoostAsioPractice;

namespace BoostAsioClient {

	void TestStackfulCoroutineClient()
	{
		io_service service;
		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
		std::vector<std::string> clientList;
		clientList.push_back("John");
		clientList.push_back("James");
		clientList.push_back("Lucy");
		for (auto& clientName : clientList)
		{
			StackfulCoroutineClient::Start(ep, clientName, &service);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		service.run();
	}

	StackfulCoroutineClient::StackfulCoroutineClientPTR StackfulCoroutineClient::Start(tcp::endpoint ep, const std::string& userName, io_service *service)
	{
		StackfulCoroutineClientPTR newConnection(new StackfulCoroutineClient(userName, service));
		newConnection->Start(ep);
		return newConnection;
	}

	void StackfulCoroutineClient::Stop()
	{
		if (!started_)
		{
			return;
		}
		error_code ec;
		socket_.close(ec);
		started_ = false;
	}

	bool StackfulCoroutineClient::Started()
	{
		return started_;
	}

	StackfulCoroutineClient::StackfulCoroutineClient(const std::string& userName, io_service *service)
		:socket_(*service), started_(false), userName_(userName), timer_(*service), service_(service)
	{
	}

	void StackfulCoroutineClient::Start(tcp::endpoint ep)
	{
		socket_.async_connect(ep, std::bind(&StackfulCoroutineClient::Step,
			shared_from_this(), std::placeholders::_1, 0));
	}

	void StackfulCoroutineClient::Step(const error_code& ec, int bytes)
	{
        auto self(shared_from_this());
		spawn(*service_, 
			[this, self](yield_context yield) {
			for (;;)
			{
				if (!Started())
				{
					started_ = true;
					std::ostream out(&writeBuffer_);
                    out << "login " << userName_ << "\n";
				}
				
				error_code ec;
                async_write(socket_, writeBuffer_, yield[ec]);
				if (ec)
				{
					Stop();
					return;
				}

                async_read_until(socket_, readBuffer_, "\n", yield[ec]);
				if (ec)
				{
                    Stop();
					return;
				}
				
                OnAnswerFromServer(yield);
			}
		});
	}

	void StackfulCoroutineClient::OnAnswerFromServer(yield_context yield)
	{
        std::istream in(&readBuffer_);
		std::string word;
		in >> word;
		if (word == "login")
		{
			OnLogin(yield);
		}
		else if (word == "ping")
		{
			OnPing(yield);
		}
		else if (word == "clients")
		{
			OnClients(yield);
		}
		// drop all data after reading complete
		readBuffer_.consume(readBuffer_.size());
	}

	void StackfulCoroutineClient::OnLogin(yield_context yield)
	{
        DoAskClients(yield);
	}

	void StackfulCoroutineClient::OnPing(yield_context yield)
	{
		std::istream in(&readBuffer_);
		std::string answer;
		in >> answer;
		if (answer == "client_list_changed")
		{
			DoAskClients(yield);
		}
		else
		{
			PostponePing(yield);
		}
	}

	void StackfulCoroutineClient::OnClients(yield_context yield)
	{
        std::ostringstream clients;
        clients << &readBuffer_;
        std::cout << userName_ << ", new client list:" << clients.str();
		PostponePing(yield);
	}

	void StackfulCoroutineClient::DoAskClients(yield_context yield)
	{
        std::ostream out(&writeBuffer_);
        out << "ask_clients\n";
	}

	void StackfulCoroutineClient::PostponePing(yield_context yield)
	{
        int sleepTime = RandomInt<int>(0, 7000);
		error_code ec;
		timer_.expires_from_now(std::chrono::milliseconds(sleepTime), ec);
        timer_.async_wait(yield);
        DoPing(yield);
	}

	void StackfulCoroutineClient::DoPing(yield_context yield)
	{
        writeBuffer_.consume(writeBuffer_.size());
        std::ostream out(&writeBuffer_);
        out << "ping\n";
	}
}