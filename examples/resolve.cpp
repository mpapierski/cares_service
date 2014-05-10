#include <boost/asio.hpp>
#include "cares_service/cares_resolver.hpp"

void handle_resolve(const boost::system::error_code & ec,
	services::cares::detail::iterator endpoint_iterator,
	std::string input)
{
	std::cout << input << ": " << ec.message() << std::endl;
	int i = 0;
	for (services::cares::detail::iterator it = endpoint_iterator;
		it != services::cares::detail::iterator();
		++it)
	{
		std::cout << ++i << ". " << *it << " TTL=" << it->ttl << std::endl;
	}
}

int
main(int argc, char * argv[])
{
	boost::asio::io_service io_service;
	services::cares::resolver resolver(io_service);
	for (int i = 1; i < argc; ++i)
	{
		std::string url(argv[i]);
		std::cout << "Resolve... " << url << std::endl;
		resolver.resolve_a(argv[i], boost::bind(&handle_resolve,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator,
			url));
	}
	io_service.run();
}