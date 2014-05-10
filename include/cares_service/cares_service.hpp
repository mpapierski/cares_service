#if !defined(CARES_SERVICE_CARES_SERVICE_HPP_)
#define CARES_SERVICE_CARES_SERVICE_HPP_

#include <string>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
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
		boost::system::error_code ec(::ares_library_init(ARES_LIB_INIT_ALL), get_error_category());
		boost::asio::detail::throw_error(ec, "ares_library_init");
	}	
	~base_cares()
	{
	}
	void shutdown_service()
	{
		std::cout << "ares_library_cleanup" << std::endl;
		::ares_library_cleanup();
	}
	boost::shared_ptr<detail::channel> get_channel()
	{
		return boost::make_shared<detail::channel>(boost::ref(get_io_service()));
	}
};

template <typename T>
boost::asio::io_service::id base_cares<T>::id;

typedef base_cares<void> cares;

}
}


#endif