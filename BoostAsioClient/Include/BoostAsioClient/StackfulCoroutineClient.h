#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/system_timer.hpp>

namespace BoostAsioClient {

	void TestStackfulCoroutineClient();

	class StackfulCoroutineClient :public std::enable_shared_from_this<StackfulCoroutineClient>, boost::noncopyable
	{
	public:
		typedef boost::system::error_code error_code;
		typedef boost::asio::ip::tcp tcp;
		typedef std::shared_ptr<StackfulCoroutineClient> StackfulCoroutineClientPTR;

		static StackfulCoroutineClientPTR Start(tcp::endpoint ep, const std::string& userName, boost::asio::io_service *service);

		void Stop();

		bool Started();

	private:
		StackfulCoroutineClient(const std::string& userName, boost::asio::io_service *service);
		void Start(tcp::endpoint ep);
		void Step(const error_code& ec, int bytes);
        //size_t IsReadComplete(const error_code& err, size_t bytes);
        void OnAnswerFromServer(boost::asio::yield_context yield);
		void OnLogin(boost::asio::yield_context yield);
		void OnPing(boost::asio::yield_context yield);
		void OnClients(boost::asio::yield_context yield);
		void DoAskClients(boost::asio::yield_context yield);
		void PostponePing(boost::asio::yield_context yield);
		void DoPing(boost::asio::yield_context yield);

		tcp::socket socket_;
		bool started_;
		std::string userName_;
		boost::asio::system_timer timer_;
		boost::asio::io_service* service_;

		boost::asio::streambuf readBuffer_;
		boost::asio::streambuf writeBuffer_;
	};
}