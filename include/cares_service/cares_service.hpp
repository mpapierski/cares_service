#if !defined(CARES_SERVICE_CARES_SERVICE_HPP_)
#define CARES_SERVICE_CARES_SERVICE_HPP_

#include <string>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <ares.h>
#include <arpa/nameser.h>
#include "detail/error.hpp"
#include "detail/channel.hpp"

namespace services {
namespace cares {

/**
 * c-ares service implementation.
 */
template <typename T>
class base_cares
	: public boost::asio::io_service::service
{
public:
	static boost::asio::io_service::id id;
	base_cares(boost::asio::io_service & io_service)
		: boost::asio::io_service::service(io_service)
	{
		if (::ares_library_init(ARES_LIB_INIT_ALL) != 0)
		{
			throw std::runtime_error("Unable to initialize cares library.");
		}
	}
	~base_cares()
	{
	}
	void shutdown_service()
	{
		std::cout << "ares_library_cleanup" << std::endl;
		::ares_library_cleanup();
	}
	template <typename Callback>
	void resolve(const std::string & input, const Callback & callback)
	{
		boost::shared_ptr<detail::channel> chan = boost::make_shared<detail::channel>(boost::ref(get_io_service()));
		boost::system::error_code ec;
		chan->init(ec);
		if (ec)
		{
			get_io_service().post(boost::bind<void>(callback, ec));
			return;
		}
		unsigned char * ptr = NULL;
		int buflen = 0;
		::ares_query(chan->get(), input.c_str(), ns_c_in, ns_t_a, &ares_callback_function, this);
		chan->getsock();
	};
	static void ares_callback_function(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
	{
		boost::system::error_code ec(status, get_error_category());
		std::cout << "ares callback function: " << ec.message() << std::endl;
	}
	static void sock_state_callback(void * data, int s, int read, int write)
	{
		std::cout << "sock state callback s=" << s << " read=" << read << " write=" << write << std::endl;
	}
};

template <typename T>
boost::asio::io_service::id base_cares<T>::id;

typedef base_cares<void> cares;

}
}


#endif