#include "BoostAsioServer/SyncServer.h"
#include <iostream>
#include <thread>
#include <mutex>

using namespace boost::asio;

namespace {
    typedef std::vector<BoostAsioServer::SyncClientConnectionPTR> ClientPTRArray;
    typedef std::lock_guard<std::mutex> ClientConnectionLock;

	BoostAsioServer::SyncClientConnection::Status StatusFromBoostErrorCode(boost::system::error_code ec)
	{
		if (ec)
		{
			return BoostAsioServer::SyncClientConnection::Status::kError;
		}
		else
		{
			return BoostAsioServer::SyncClientConnection::Status::kSuccess;
		}
	}

	io_service gService;
    ClientPTRArray gClientsArray;
	std::mutex gClientArrayMutex;
}

namespace BoostAsioServer {

    void TestSyncServer()
    {
        std::vector<std::thread> threads;
        threads.push_back(std::thread(AcceptThread));
        threads.push_back(std::thread(HandleClientsThread));
        for (auto& thread:threads)
        {
            thread.join();
        }
    }

    void AcceptThread()
    {
        ip::tcp::acceptor acceptor(gService, ip::tcp::endpoint(ip::tcp::v4(), 8001));
        while (true)
        {
            SyncClientConnectionPTR client(new SyncClientConnection());
            acceptor.accept(client->Socket());
            client->UpdateLastPingTime();
			ClientConnectionLock lock(gClientArrayMutex);
            gClientsArray.push_back(client);
        }
    }

    void HandleClientsThread()
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
			ClientConnectionLock lock(gClientArrayMutex);
            for (auto& client : gClientsArray)
            {
				client->AnswerToClient();
            }
			gClientsArray.erase(std::remove_if(gClientsArray.begin(), gClientsArray.end(), 
				[](SyncClientConnectionPTR& currClient) { 
				return !(currClient->Socket().is_open()); 
			}), gClientsArray.end());
		}
    }

    SyncClientConnection::SyncClientConnection()
        :socket_(gService), readedSize_(0),lastPing_(std::chrono::milliseconds::zero())
    {
    }

    std::string SyncClientConnection::UserName() const
    {
        return userName_;
    }

    void SyncClientConnection::AnswerToClient()
    {
		if (Status::kSuccess == ReadRequest())
		{
			ProcessRequest();
		}
        else
        {
            Stop();
        }
        if (TimeOut())
        {
            Stop();
			UpdateClientsChanged();
		}
    }

    void SyncClientConnection::SetClientsChanged()
    {
        isClientsChanged_ = true;
    }

    SyncClientConnection::tcp::socket& SyncClientConnection::Socket()
    {
        return socket_;
    }

    void SyncClientConnection::UpdateLastPingTime()
    {
        lastPing_ = std::chrono::system_clock::now();
    }

    bool SyncClientConnection::TimeOut() const
    {
		time_point now = std::chrono::system_clock::now();
        std::chrono::system_clock::duration period = now-lastPing_;
		auto periodMs = std::chrono::duration_cast<std::chrono::milliseconds>(period);
        return periodMs.count() > 5000;
    }

    SyncClientConnection::Status SyncClientConnection::Stop()
    {
		error_code ec;
        socket_.close(ec);
		return StatusFromBoostErrorCode(ec);
    }

    SyncClientConnection::Status SyncClientConnection::ReadRequest()
    {
		error_code ec;
        if (socket_.available(ec))
        {
            readedSize_ += socket_.read_some(buffer(buff_ + readedSize_, kBuffSize - readedSize_), ec);
        }
		return StatusFromBoostErrorCode(ec);
    }

	void SyncClientConnection::ProcessRequest()
    {
		bool foundEnter = std::find(buff_, buff_ + readedSize_, '\n') < buff_ + readedSize_;
        if (!foundEnter)
        {
            return;
        }
        lastPing_ = std::chrono::system_clock::now();
        size_t pos = std::find(buff_, buff_ + readedSize_, '\n')-buff_;
        std::string message(buff_, pos);
		std::copy(buff_ + readedSize_, buff_ + kBuffSize, buff_);
        readedSize_ -= pos + 1;
        if (message.find("login ")==0)
        {
            OnLogin(message);
        }
        else if (message.find("ping")==0)
        {
            OnPing();
        }
        else if (message.find("ask_clients")==0)
        {
            OnClients();
        }
        else
        {
            std::cerr << "invalid message " << message << std::endl;
        }
    }

    void SyncClientConnection::OnLogin(const std::string& message)
    {
        std::istringstream in(message);
        in >> userName_ >> userName_;
        Write("login ok\n");
        UpdateClientsChanged();
    }

    void SyncClientConnection::OnPing()
    {
        Write(isClientsChanged_ ? "ping client_list_changed\n" : "ping ok\n");
        isClientsChanged_ = false;
    }

    void SyncClientConnection::OnClients()
    {
        std::string message;
        {
            for (const auto& client:gClientsArray)
            {
                message += client->UserName() + " ";
            }
            Write("clients " + message + "\n");
        }
    }

    SyncClientConnection::Status SyncClientConnection::Write(const std::string& message)
    {
		error_code ec;
        socket_.write_some(buffer(message),ec);
		return StatusFromBoostErrorCode(ec);
    }

    void SyncClientConnection::UpdateClientsChanged()
    {
		for (auto& client : gClientsArray)
        {
			client->SetClientsChanged();
        }
    }

}