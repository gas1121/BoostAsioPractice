#include <boost/asio.hpp>

namespace BoostAsioClient {
    void RunSyncClient(const std::string& clientName);

    class SyncClient :public boost::noncopyable
    {
    public:
        typedef boost::asio::ip::tcp tcp;

		enum class Status
		{
			kSuccess = 0,
			kError
		};

        SyncClient(const std::string& userName,boost::asio::io_service* service);

		Status Connect(tcp::endpoint ep);
        void Loop();

        std::string UserName() const;

    private:
		typedef boost::system::error_code error_code;

		Status WriteRequest();
		Status ReadAnswer();
        size_t IsReadComplete(const error_code& err, size_t bytes);
        void ProcessMessage();
        void OnLogin();
        void OnPing(const std::string& message);
        void OnClients(const std::string& message);
        void DoAskClients();

		Status Write(const std::string& message);

        tcp::socket socket_;
        enum { kBuffSize = 1024 };
        char buff_[kBuffSize];
        int readedSize_;
        bool started_;
        std::string userName_;
    };
}