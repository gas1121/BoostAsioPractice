#include <boost/asio.hpp>
#include <chrono>

namespace BoostAsioServer {
    void TestSyncServer();

    void AcceptThread();
    void HandleClientsThread();

    class SyncClientConnection:public std::enable_shared_from_this<SyncClientConnection>,boost::noncopyable
    {
    public:
        typedef boost::asio::ip::tcp tcp;

		enum class Status
		{
			kSuccess = 0,
			kError
		};

        SyncClientConnection();

        std::string UserName() const;
        tcp::socket& Socket();
        void UpdateLastPingTime();
        bool TimeOut() const;
		Status Stop();

		Status ReadRequest();
        void ProcessRequest();
        void AnswerToClient();
    private:
        typedef std::chrono::system_clock::time_point time_point;
		typedef boost::system::error_code error_code;

        void OnLogin(const std::string& message);
        void OnPing();
        void OnClients();

		Status Write(const std::string& message);

        void UpdateClientsChanged();
        void SetClientsChanged();

        tcp::socket socket_;
        enum { kBuffSize = 1024 };
        char buff_[kBuffSize];
        int readedSize_;
        bool started_;
        std::string userName_;
        bool isClientsChanged_;
        time_point lastPing_;
    };

    typedef std::shared_ptr<SyncClientConnection> SyncClientConnectionPTR;
}