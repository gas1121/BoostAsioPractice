#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/system_timer.hpp>

namespace BoostAsioClient {

	void TestStacklessCoroutineClient();

	class StacklessCoroutineClient :public std::enable_shared_from_this<StacklessCoroutineClient>, boost::noncopyable
	{
	public:
		typedef boost::system::error_code error_code;
		typedef boost::asio::ip::tcp tcp;
		typedef std::shared_ptr<StacklessCoroutineClient> StacklessCoroutineClientPTR;

		static StacklessCoroutineClientPTR Start(tcp::endpoint ep, const std::string& userName, boost::asio::io_service *service);

		void Stop();

		bool Started();

	private:
		StacklessCoroutineClient(const std::string& userName, boost::asio::io_service *service);
		void Start(tcp::endpoint ep);
		void Step(const error_code& ec, int bytes);
		void OnAnswerFromServer();
		void OnLogin();
		void OnPing();
		void OnClients();
		void DoAskClients();
		void PostponePing();
		void DoPing();

		tcp::socket socket_;
		bool started_;
		std::string userName_;
		boost::asio::system_timer timer_;
		boost::asio::io_service* service_;

		boost::asio::streambuf readBuffer_;
		boost::asio::streambuf writeBuffer_;

		boost::asio::coroutine coroutine_;
	};
}