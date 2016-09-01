#include "BoostAsioClient/SyncClient.h"
#include <thread>
#include <iostream>

#include <Core/Core.hpp>

using namespace boost::asio;
using namespace BoostAsioPractice;

namespace {
    const unsigned short kPort = 8001;

    BoostAsioClient::SyncClient::Status StatusFromBoostErrorCode(boost::system::error_code ec)
	{
		if (ec)
		{
			return BoostAsioClient::SyncClient::Status::kError;
		}
		else
		{
			return BoostAsioClient::SyncClient::Status::kSuccess;
		}
	}
}

namespace BoostAsioClient {

    void RunSyncClient(const std::string& clientName)
    {
        io_service service;
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), kPort);
        SyncClient client(clientName,&service);
        if (SyncClient::Status::kSuccess==client.Connect(ep))
        {
            client.Loop();
        }
        else
        {
            std::cout << "client terminated " << std::endl;
        }
    }

    SyncClient::SyncClient(const std::string& userName, boost::asio::io_service* service)
        :socket_(*service),started_(true),userName_(userName),readedSize_(0)
    {

    }

    SyncClient::Status SyncClient::Connect(tcp::endpoint ep)
    {
        error_code ec;
        socket_.connect(ep, ec);
        return StatusFromBoostErrorCode(ec);
    }

    void SyncClient::Loop()
    {
		if (Status::kSuccess!= Write("login " + userName_ + "\n"))
		{
			return;
		}
		if (Status::kSuccess!= ReadAnswer())
		{
			return;
		}
        while (started_)
        {
			if (Status::kSuccess==WriteRequest()
				&&Status::kSuccess==ReadAnswer())
			{
				int sleepTime = RandomNumber<int>(0, 7000);
				std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
			}
			else
			{
				break;
			}
        }
    }

    std::string SyncClient::UserName() const
    {
        return userName_;
    }

	SyncClient::Status SyncClient::WriteRequest()
    {
        return Write("ping\n");
    }

    SyncClient::Status SyncClient::ReadAnswer()
    {
		error_code ec;
        readedSize_ = read(socket_, buffer(buff_), 
            std::bind(&SyncClient::IsReadComplete, this, std::placeholders::_1, std::placeholders::_2), ec);
		if (!ec)
		{
			ProcessMessage();
		}
		return StatusFromBoostErrorCode(ec);
    }

    size_t SyncClient::IsReadComplete(const error_code& err, size_t bytes)
    {
        if (err)
        {
            return 0;
        }
        bool found = std::find(buff_, buff_ + bytes, '\n') < buff_ + bytes;
		return found ? 0 : 1;
    }

    void SyncClient::ProcessMessage()
    {
        std::string message(buff_, readedSize_);
        if (message.find("login ")==0)
        {
            OnLogin();
        }
        else if (message.find("ping")==0)
        {
            OnPing(message);
        }
        else if (message.find("clients ")==0)
        {
            OnClients(message);
        }
        else
        {
            std::cerr << "invalid message " << message << std::endl;
        }
    }

    void SyncClient::OnLogin()
    {
        DoAskClients();
    }

    void SyncClient::OnPing(const std::string& message)
    {
        std::istringstream in(message);
        std::string answer;
        in >> answer >> answer;
        if (answer=="client_list_changed")
        {
            DoAskClients();
        }
    }

    void SyncClient::OnClients(const std::string& message)
    {
        std::string clients = message.substr(8);
        std::cout << userName_ << ", new client list:" << clients;
    }

    void SyncClient::DoAskClients()
    {
        Write("ask_clients\n");
        ReadAnswer();
    }

	SyncClient::Status SyncClient::Write(const std::string& message)
    {
		error_code ec;
        socket_.write_some(buffer(message),ec);
		return StatusFromBoostErrorCode(ec);
    }

}