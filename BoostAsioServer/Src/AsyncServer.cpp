#include "BoostAsioServer/AsyncServer.h"
#include <thread>
#include <iostream>

using namespace boost::asio;

namespace {
	typedef std::vector<BoostAsioServer::AsyncClientConnectionPTR> ClientPTRArray;
    const unsigned short kPort = 8001;

	ClientPTRArray gClientsArray;
}

namespace BoostAsioServer {

	void TestAsyncServer()
	{
		io_service service;
		AsyncClientConnectionPTR client = AsyncClientConnection::New(&service);
		ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), kPort));
		std::function<void(AsyncClientConnectionPTR, const boost::system::error_code&)> HandleAccept =
			[&HandleAccept, &service, &acceptor](AsyncClientConnectionPTR client, const boost::system::error_code& err) {
			client->Start();
			AsyncClientConnectionPTR newClient = AsyncClientConnection::New(&service);
			acceptor.async_accept(newClient->Socket(), std::bind(HandleAccept, newClient, std::placeholders::_1));
		};
		acceptor.async_accept(client->Socket(), std::bind(HandleAccept, client, std::placeholders::_1));
		service.run();
	}

    AsyncClientConnection::AsyncClientConnection(boost::asio::io_service* service)
		:socket_(*service), started_(false), userName_(""), timer_(*service), 
		lastPing_(std::chrono::system_clock::now()), isClientsChanged_(false)
    {
		strcpy(readBuffer_, "");
		strcpy(writeBuffer_, "");
    }

    AsyncClientConnectionPTR AsyncClientConnection::New(boost::asio::io_service *service)
    {
        AsyncClientConnectionPTR newServer(new AsyncClientConnection(service));
        return newServer;
    }

    void AsyncClientConnection::Start()
    {
        started_ = true;
		gClientsArray.push_back(shared_from_this());
		lastPing_ = std::chrono::system_clock::now();
        DoRead();
    }

    void AsyncClientConnection::Stop()
    {
		if (!started_)
        {
            return;
        }
        socket_.close();
        started_ = false;

		AsyncClientConnectionPTR self = shared_from_this();
		ClientPTRArray::iterator iter = std::find(gClientsArray.begin(), gClientsArray.end(), self);
		gClientsArray.erase(iter);
		UpdateClientsChanged();
    }

	bool AsyncClientConnection::Started()
	{
		return started_;
	}

    ip::tcp::socket& AsyncClientConnection::Socket()
    {
        return socket_;
    }

	std::string AsyncClientConnection::UserName() const
	{
		return userName_;
	}

	void AsyncClientConnection::SetClientsChanged()
	{
		isClientsChanged_ = true;
	}

    void AsyncClientConnection::DoRead()
    {
        async_read(socket_, buffer(readBuffer_), std::bind(&AsyncClientConnection::IsReadComplete, shared_from_this(),
            std::placeholders::_1, std::placeholders::_2), std::bind(&AsyncClientConnection::OnRead, shared_from_this(),
                std::placeholders::_1, std::placeholders::_2));
		PostCheckPing();
    }

    size_t AsyncClientConnection::IsReadComplete(const error_code& err, size_t bytes)
    {
        if (err)
        {
            return 0;
        }
        bool found = std::find(readBuffer_, readBuffer_ + bytes, '\n') < readBuffer_ + bytes;
        return found ? 0 : 1;
    }

    void AsyncClientConnection::OnRead(const error_code& err, size_t bytes)
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
		if (message.find("login ") == 0)
		{
			OnLogin(message);
		}
		else if (message.find("ping") == 0)
		{
			OnPing();
		}
		else if (message.find("ask_clients") == 0)
		{
			OnClients();
		}
    }

	void AsyncClientConnection::OnLogin(const std::string& message)
	{
		std::istringstream in(message);
		in >> userName_ >> userName_;
		DoWrite("login ok\n");
		UpdateClientsChanged();
	}

	void AsyncClientConnection::OnPing()
	{
		DoWrite(isClientsChanged_ ? "ping client_list_changed\n" : "ping ok\n");
		isClientsChanged_ = false;
	}

	void AsyncClientConnection::OnClients()
	{
		std::string message;
		{
			for (const auto& client : gClientsArray)
			{
				message += client->UserName() + " ";
			}
			DoWrite("clients " + message + "\n");
		}
	}

	void AsyncClientConnection::PostCheckPing()
	{
		error_code ec;
		//should greater than timeout limit.
		timer_.expires_from_now(std::chrono::milliseconds(5050),ec);
		timer_.async_wait(std::bind(&AsyncClientConnection::OnCheckPing, shared_from_this()));
	}

	void AsyncClientConnection::OnCheckPing()
	{
		time_point now = std::chrono::system_clock::now();
		std::chrono::system_clock::duration period = now - lastPing_;
		auto periodMs = std::chrono::duration_cast<std::chrono::milliseconds>(period);
		if (periodMs.count() > 5000)
		{
			Stop();
		}
		lastPing_ = std::chrono::system_clock::now();
	}

    void AsyncClientConnection::DoWrite(const std::string& message)
    {
        if (!started_)
        {
            return;
        }
        assert(message.size() < kBufferSize);
        std::copy(message.begin(), message.end(), writeBuffer_);
        socket_.async_write_some(buffer(writeBuffer_, message.size()), std::bind(&AsyncClientConnection::OnWrite, shared_from_this(),
            std::placeholders::_1, std::placeholders::_2));
    }

    void AsyncClientConnection::OnWrite(const error_code& err, size_t bytes)
    {
        DoRead();
    }

	void AsyncClientConnection::UpdateClientsChanged()
	{
		for (auto& client : gClientsArray)
		{
			client->SetClientsChanged();
		}
	}
}