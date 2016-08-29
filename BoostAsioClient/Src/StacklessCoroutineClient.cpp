#include "BoostAsioClient/StacklessCoroutineClient.h"
#include <thread>
#include <iostream>
#include <boost/asio/yield.hpp>

#include <Core/Core.hpp>

using namespace boost::asio;
using namespace BoostAsioPractice;

namespace BoostAsioClient {

	void TestStacklessCoroutineClient()
	{
		io_service service;
		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
		std::vector<std::string> clientList;
		clientList.push_back("John");
		clientList.push_back("James");
		clientList.push_back("Lucy");
		for (auto& clientName : clientList)
		{
			StacklessCoroutineClient::Start(ep, clientName, &service);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		service.run();
	}

	StacklessCoroutineClient::StacklessCoroutineClientPTR StacklessCoroutineClient::Start(tcp::endpoint ep, const std::string& userName, io_service *service)
	{
		StacklessCoroutineClientPTR newConnection(new StacklessCoroutineClient(userName, service));
		newConnection->Start(ep);
		return newConnection;
	}

	void StacklessCoroutineClient::Stop()
	{
		if (!started_)
		{
			return;
		}
		error_code ec;
		socket_.close(ec);
		started_ = false;
	}

	bool StacklessCoroutineClient::Started()
	{
		return started_;
	}

	StacklessCoroutineClient::StacklessCoroutineClient(const std::string& userName, io_service *service)
		:socket_(*service), started_(false), userName_(userName), timer_(*service), service_(service)
	{

	}

	void StacklessCoroutineClient::Start(tcp::endpoint ep)
	{
		socket_.async_connect(ep, std::bind(&StacklessCoroutineClient::Step,
			shared_from_this(), std::placeholders::_1, 0));
	}

	void StacklessCoroutineClient::Step(const error_code& ec,int bytes)
	{
		reenter(coroutine_)
		{
			for (;;)
			{
				if (!Started())
				{
					started_ = true;
					std::ostream out(&writeBuffer_);
					out << "login " << userName_ << "\n";
				}
				
				yield {
					if (ec)
					{
						Stop();
						break;
					}
					async_write(socket_, writeBuffer_, std::bind(&StacklessCoroutineClient::Step,
						shared_from_this(), std::placeholders::_1, std::placeholders::_2));
				}

				yield {
					if (ec)
					{
						Stop();
						break;
					}
					async_read_until(socket_, readBuffer_, "\n", std::bind(&StacklessCoroutineClient::Step,
						shared_from_this(), std::placeholders::_1, std::placeholders::_2));
				}
				yield {
					if (ec)
					{
						Stop();
						break;
					}
					service_->post(std::bind(&StacklessCoroutineClient::OnAnswerFromServer, shared_from_this()));
				}
			}
		}
	}

	void StacklessCoroutineClient::OnAnswerFromServer()
	{
		std::istream in(&readBuffer_);
		std::string word;
		in >> word;
		if (word=="login")
		{
			OnLogin();
		}
		else if (word=="ping")
		{
			OnPing();
		}
		else if (word=="clients")
		{
			OnClients();
		}
		// drop all data after reading complete
		readBuffer_.consume(readBuffer_.size());
	}

	void StacklessCoroutineClient::OnLogin()
	{
		DoAskClients();
	}

	void StacklessCoroutineClient::OnPing()
	{
		std::istream in(&readBuffer_);
		std::string answer;
		in >> answer;
		if (answer == "client_list_changed")
		{
			DoAskClients();
		}
		else
		{
			PostponePing();
		}
	}

	void StacklessCoroutineClient::OnClients()
	{
		std::ostringstream clients;
		clients << &readBuffer_;
		std::cout << userName_ << ", new client list:" << clients.str();
		PostponePing();
	}

	void StacklessCoroutineClient::DoAskClients()
	{
		std::ostream out(&writeBuffer_);
		out << "ask_clients\n";
		service_->post(std::bind(&StacklessCoroutineClient::Step, shared_from_this(), error_code(), 0));
	}

	void StacklessCoroutineClient::PostponePing()
	{
		int sleepTime = RandomNumber<int>(0, 7000);
		error_code ec;
		timer_.expires_from_now(std::chrono::milliseconds(sleepTime), ec);
		timer_.async_wait(std::bind(&StacklessCoroutineClient::DoPing, shared_from_this()));
	}

	void StacklessCoroutineClient::DoPing()
	{
		std::ostream out(&writeBuffer_);
		out << "ping\n";
		service_->post(std::bind(&StacklessCoroutineClient::Step, shared_from_this(), error_code(), 0));
	}
}