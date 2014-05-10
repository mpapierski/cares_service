#include <boost/asio.hpp>
#include "cares_service/cares_service.hpp"

void handle_resolve(const boost::system::error_code & ec,
	std::string input)
{
	std::cout << input << ": " << ec.message() << std::endl;
}

int
main(int argc, char * argv[])
{
	boost::asio::io_service io_service;
	services::cares::cares & cares_service = boost::asio::use_service<services::cares::cares>(io_service);
	for (int i = 1; i < argc; ++i)
	{
		std::string url(argv[i]);
		std::cout << "Resolve... " << url << std::endl;
		cares_service.resolve(argv[i], boost::bind(&handle_resolve,
			boost::asio::placeholders::error,
			url));
	}
	io_service.run();
}