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
	template <typename Callback>
	struct callback_context
	{
		base_cares<T> * cares_service;
		boost::optional<boost::asio::io_service::work> keep_busy;
		boost::optional<Callback> callback;
	};
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
		callback_context<Callback> * context = new callback_context<Callback>();
		context->cares_service = this;
		context->callback = callback;
		//context->keep_busy = boost::in_place(boost::ref(get_io_service()));
		::ares_query(chan->get(), input.c_str(), ns_c_in, ns_t_a, &ares_callback_function<Callback>, context);
		chan->getsock();
	};
	template <typename Callback>
	static void ares_callback_function(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
	{
		assert(arg);
		boost::shared_ptr<callback_context<Callback> > ctx(static_cast<callback_context<Callback> *>(arg));
		boost::system::error_code ec(status, get_error_category());
		if (ec)
		{
			assert(ctx->cares_service);
			ctx->cares_service->get_io_service().post(
				[ctx, ec]()
				{
					(*ctx->callback)(ec);
				});
			return;
		}
		struct ares_addrttl *addrttls = nullptr;
		struct hostent * host = nullptr;
		int num;
		ec.assign(::ares_parse_a_reply(abuf, alen, &host, addrttls, &num), get_error_category());
		if (ec)
		{
			ctx->cares_service->get_io_service().post(
				[ctx, ec]()
				{
					(*ctx->callback)(ec);
				});	
			return;
		}
		std::cout << "ares callback function: " << ec.message() << std::endl;

		// try parsing answer
		for (int size = 8; true; size += 4)
		{
			std::cout << "size=" << size << std::endl;
			// allocate memory
			std::vector<struct ares_addrttl> addresses(size);
			struct hostent * host = nullptr;
			
			// copy size
			int matches = size;
			
			// parse the answer
			int result = ares_parse_a_reply(abuf, alen, &host, addresses.data(), &matches);

			// on failure we leap out, otherwise we continue to allocate more memory
			if (result != ARES_SUCCESS) return;
			
			// check if we had enough space
			if (matches >= size) continue;
			
			// we have enough space, copy the results to the object
			//std::cout << "matches=" << matches << " " << addresses[i]->h_name << std::endl;
				for(int i = 0 ; i < matches ; i++)
				{
					char *name = inet_ntoa(addresses[i].ipaddr);
					std::cout << "IP is " << name << ", TTL is " << addresses[i].ttl << std::endl;
				}
				for(int i = 0; host->h_aliases[i]!=NULL;i++)
					std::cout << "Alias " << (i+1) << ". " << host->h_aliases[i] << std::endl;
				for(int i = 0 ;  host->h_addr_list[i] != NULL; i++)
	{
	   printf("%d.%d.%d.%d\n",(unsigned char)host->h_addr_list[i][0],(unsigned char)host->h_addr_list[i][1],(unsigned char)host->h_addr_list[i][2],(unsigned char)host->h_addr_list[i][3]);
	}

			// done
			return;
		}

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