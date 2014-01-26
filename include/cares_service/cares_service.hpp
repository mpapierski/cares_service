#if !defined(CARES_SERVICE_CARES_SERVICE_HPP_)
#define CARES_SERVICE_CARES_SERVICE_HPP_

#include <string>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <ares.h>
#include "detail/error.hpp"
#include "detail/channel.hpp"

namespace services {
namespace cares {

class cares
{
private:
	boost::asio::io_service & io_service_;
public:
	cares(boost::asio::io_service & io_service)
		: io_service_(io_service)
	{
		if (::ares_library_init(ARES_LIB_INIT_ALL) != 0)
		{
			throw std::runtime_error("Unable to initialize cares library.");
		}
	}
	~cares()
	{
		::ares_library_cleanup();
	}
	template <typename Callback>
	void resolve(const std::string & input, const Callback & callback)
	{
		boost::shared_ptr<detail::channel> chan = boost::make_shared<detail::channel>();
		boost::system::error_code ec;
		chan->init(ec);
		if (ec)
		{
			io_service_.post(boost::bind<void>(callback, ec));
			return;
		}
		ares_options opts;
		opts.sock_state_cb = NULL;
		ec.assign(::ares_init_options(chan->get(), &opts))

	};
};

}
}


#endif