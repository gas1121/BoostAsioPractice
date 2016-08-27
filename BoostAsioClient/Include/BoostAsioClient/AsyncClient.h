#include <boost/asio.hpp>
#include <boost/asio/system_timer.hpp>

namespace BoostAsioClient {

	void TestAsyncClient();
	
	class AsyncClient :public std::enable_shared_from_this<AsyncClient>, boost::noncopyable
    {
    public:
        typedef boost::system::error_code error_code;
        typedef boost::asio::ip::tcp tcp;
        typedef std::shared_ptr<AsyncClient> AsyncClientPTR;

        static AsyncClientPTR Start(tcp::endpoint ep, const std::string& userName, boost::asio::io_service *service);

		void Stop();

        bool Started();

    private:
        AsyncClient(const std::string& userName, boost::asio::io_service *service);
        void Start(tcp::endpoint ep);
        void OnConnect(const error_code& err);
		void DoWrite(const std::string& message);
		void OnWrite(const error_code& err, size_t bytes);
		void DoRead();
        size_t IsReadComplete(const error_code& err, size_t bytes);
        void OnRead(const error_code& err, size_t bytes);
		void OnLogin();
		void OnPing(const std::string& message);
		void OnClients(const std::string& message);
		void DoAskClients();
		void PostponePing();
		void DoPing();

        tcp::socket socket_;
        bool started_;
        std::string userName_;
		boost::asio::system_timer timer_;

        enum { kBufferSize = 1024 };
        char readBuffer_[kBufferSize];
        char writeBuffer_[kBufferSize];
    };
}