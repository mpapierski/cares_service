cares_service
=============

`cares_service` is an `boost.asio` wrapper for the excellent `c-ares` library. With very little work this service could be used as replacement for the standard `boost.asio` resolver that is limited to `A` or `AAAA` queries.

The code is encapsulated in `services::cares` namespace.

Example
=======

````c++

void handle_resolve_a(const boost::system::error_code & ec,
	services::cares::a_reply_iterator iter,
	const char * input)
{
	if (ec)
	{
		std::cerr << "Unable to resolve A query: " << ec.message() << std::endl;
		return;
	}
	for (; iter != services::cares::a_reply_iterator();	++iter)
	{
		std::cout << input << "\tA\t" << *iter << "\tTTL=" << iter->ttl << std::endl;
	}
}

int
main(int argc, char * argv[])
{
	boost::asio::io_service io_service;
	services::cares::resolver resolver(io_service);
	for (int i = 1; i < argc; ++i)
	{
		resolver.resolve_a(argv[i], boost::bind(&handle_resolve_a,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));
	}
	io_service.run();
}
````

TODO
====

* Better reply iterators
* More low level options
* Tests with large replies

License
=======

Same as `c-ares`. See http://c-ares.haxx.se/license.html.
