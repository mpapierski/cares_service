#if !defined(CARES_SERVICE_CARES_RESOLVER_HPP_)
#define CARES_SERVICE_CARES_RESOLVER_HPP_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "cares_service/cares_service.hpp"
#include "cares_service/detail/iterator.hpp"

namespace services {
namespace cares {

struct resolver
{
	template <typename Callback>
	struct callback_context
	{
		resolver * self;
		boost::optional<Callback> callback;
	};
	boost::asio::io_service & io_service_;
	resolver(boost::asio::io_service & io_service)
		: io_service_(io_service)
	{
	}
	inline
	boost::asio::io_service & get_io_service()
	{
		return io_service_;
	}
	template <typename Callback>
	void resolve(const std::string & input, const Callback & callback)
	{
		boost::shared_ptr<detail::channel> chan = boost::asio::use_service<cares>(io_service_).get_channel();
		boost::system::error_code ec;
		chan->init(ec);
		if (ec)
		{
			get_io_service().post(boost::bind<void>(callback, ec, detail::iterator()));
			return;
		}
		callback_context<Callback> * context = new callback_context<Callback>();
		context->self = this;
		context->callback = callback;
		::ares_query(chan->get(), input.c_str(), ns_c_in, ns_t_a, &ares_callback_function<Callback>, context);
		chan->getsock();
	};
	template <typename Callback>
	static void ares_callback_function(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
	{
		assert(arg);
		boost::shared_ptr<callback_context<Callback> > ctx(static_cast<callback_context<Callback> *>(arg));
		boost::system::error_code ec(status, get_error_category());
		Callback callback = *ctx->callback;
		if (ec)
		{
			assert(ctx->self);
			ctx->self->get_io_service().post(boost::bind<void>(callback, ec, detail::iterator()));
			return;
		}
		struct ares_addrttl *addrttls = nullptr;
		struct hostent * host = nullptr;
		int num;
		ec.assign(::ares_parse_a_reply(abuf, alen, &host, addrttls, &num), get_error_category());
		if (ec)
		{
			ctx->self->get_io_service().post(boost::bind<void>(callback, ec, detail::iterator()));
			return;
		}
		// try parsing answer
		for (int size = 8; true; size += 4)
		{
			// allocate memory
			boost::shared_ptr<std::vector<struct ares_addrttl> > addresses =
				boost::make_shared<std::vector<struct ares_addrttl> >(size);
			// copy size
			int matches = size;
			
			// parse the answer
			ec.assign(::ares_parse_a_reply(abuf, alen, nullptr, addresses->data(), &matches), get_error_category());

			// on failure we leap out, otherwise we continue to allocate more memory
			if (ec)
			{
				ctx->self->get_io_service().post(boost::bind<void>(callback, ec, detail::iterator()));
				return;
			}
			
			// check if we had enough space
			if (matches >= size) continue;
			addresses->resize(matches);
			detail::iterator result_iterator(addresses);
			ctx->self->get_io_service().post(boost::bind<void>(callback, ec, result_iterator));
#if 0			
			// we have enough space, copy the results to the object
			for(int i = 0 ; i < matches ; i++)
			{
				char *name = inet_ntoa(addresses[i].ipaddr);
				std::cout << "IP is " << name << ", TTL is " << addresses[i].ttl << std::endl;
			}
			for(int i = 0; host->h_aliases[i]!=NULL;i++)
			{
				std::cout << "Alias " << (i+1) << ". " << host->h_aliases[i] << std::endl;
			}
			for(int i = 0 ;  host->h_addr_list[i] != NULL; i++)
			{
				printf("%d.%d.%d.%d\n",(unsigned char)host->h_addr_list[i][0],(unsigned char)host->h_addr_list[i][1],(unsigned char)host->h_addr_list[i][2],(unsigned char)host->h_addr_list[i][3]);
			}
#endif
			// done
			return;
		}
	}
};

}
}

#endif
