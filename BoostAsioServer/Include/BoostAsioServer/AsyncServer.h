#include <boost/asio.hpp>
#include <boost/asio/system_timer.hpp>

namespace BoostAsioServer {

	void TestAsyncServer();

	class AsyncClientConnection;
	typedef std::shared_ptr<AsyncClientConnection> AsyncClientConnectionPTR;

	class AsyncClientConnection :public std::enable_shared_from_this<AsyncClientConnection>, boost::noncopyable
    {
    public:
        typedef boost::system::error_code error_code;
        typedef boost::asio::ip::tcp tcp;

        static AsyncClientConnectionPTR New(boost::asio::io_service *service);

        void Start();
        void Stop();
		bool Started();
        tcp::socket& Socket();
		std::string UserName() const;
        void SetClientsChanged();

    private:
		typedef std::chrono::system_clock::time_point time_point;

        AsyncClientConnection(boost::asio::io_service *service);
        void DoRead();
        size_t IsReadComplete(const error_code& err, size_t bytes);
        void OnRead(const error_code& err, size_t bytes);
		void OnLogin(const std::string& message);
		void OnPing();
		void OnClients();
		void PostCheckPing();
		void OnCheckPing();

        void DoWrite(const std::string& message);
        void OnWrite(const error_code& err, size_t bytes);

		void UpdateClientsChanged();

        tcp::socket socket_;
        bool started_;
		std::string userName_;
		boost::asio::system_timer timer_;
		time_point lastPing_;
		bool isClientsChanged_;

        enum { kBufferSize = 1024 };
        char readBuffer_[kBufferSize];
        char writeBuffer_[kBufferSize];
    };
}