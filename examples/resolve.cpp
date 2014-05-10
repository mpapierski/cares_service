#include <boost/asio.hpp>
#include "cares_service/cares_resolver.hpp"

void handle_resolve_a(const boost::system::error_code & ec,
	services::cares::a_reply_iterator iter,
	const char * input)
{
	for (; iter != services::cares::a_reply_iterator();	++iter)
	{
		std::cout << input << "\tA\t" << *iter << "\tTTL=" << iter->ttl << std::endl;
	}
}

void handle_resolve_aaaa(const boost::system::error_code & ec,
	services::cares::aaaa_reply_iterator endpoint_iterator,
	const char * input)
{
	for (services::cares::aaaa_reply_iterator it = endpoint_iterator;
		it != services::cares::aaaa_reply_iterator();
		++it)
	{
		std::cout << input << "\tAAAA\t" << *it << "\tTTL=" << it->ttl << std::endl;
	}
}

int
main(int argc, char * argv[])
{
	boost::asio::io_service io_service;
	services::cares::resolver resolver(io_service);
	for (int i = 1; i < argc; ++i)
	{
		const char * url = argv[i];
		std::cout << "Resolve... " << url << std::endl;
		resolver.resolve_a(url, boost::bind(&handle_resolve_a,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator,
			url));
		resolver.resolve_aaaa(url, boost::bind(&handle_resolve_aaaa,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator,
			url));
	}
	io_service.run();
}