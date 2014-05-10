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
	void resolve_a(const std::string & input, const Callback & callback)
	{
		boost::shared_ptr<detail::channel> chan = boost::asio::use_service<cares>(io_service_).get_channel();
		boost::system::error_code ec;
		chan->init(ec);
		if (ec)
		{
			get_io_service().post(boost::bind<void>(callback, ec, a_reply_iterator()));
			return;
		}
		callback_context<Callback> * context = new callback_context<Callback>();
		context->self = this;
		context->callback = callback;
		::ares_query(chan->get(), input.c_str(), ns_c_in, ns_t_a, &ares_callback_function<struct ares_addrttl, Callback>, context);
		chan->getsock();
	};
	template <typename Result, typename Callback>
	static void ares_callback_function(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
	{
		assert(arg);
		boost::shared_ptr<callback_context<Callback> > ctx(static_cast<callback_context<Callback> *>(arg));
		boost::system::error_code ec(status, get_error_category());
		Callback callback = *ctx->callback;
		if (ec)
		{
			assert(ctx->self);
			ctx->self->get_io_service().post(boost::bind<void>(callback, ec, detail::result_iterator<Result>()));
			return;
		}
		struct ares_addrttl *addrttls = nullptr;
		struct hostent * host = nullptr;
		int num;
		ec.assign(::ares_parse_a_reply(abuf, alen, &host, addrttls, &num), get_error_category());
		if (ec)
		{
			ctx->self->get_io_service().post(boost::bind<void>(callback, ec, detail::result_iterator<Result>()));
			return;
		}
		// try parsing answer
		for (int size = 8; true; size += 4)
		{
			// allocate memory
			boost::shared_ptr<std::vector<Result> > addresses =
				boost::make_shared<std::vector<Result> >(size);
			// copy size
			int matches = size;
			
			// parse the answer
			ec.assign(::ares_parse_a_reply(abuf, alen, nullptr, addresses->data(), &matches), get_error_category());

			// on failure we leap out, otherwise we continue to allocate more memory
			if (ec)
			{
				ctx->self->get_io_service().post(boost::bind<void>(callback, ec, detail::result_iterator<Result>()));
				return;
			}
			
			// check if we had enough space
			if (matches >= size) continue;
			addresses->resize(matches);
			detail::result_iterator<Result> result_iterator(addresses);
			ctx->self->get_io_service().post(boost::bind<void>(callback, ec, result_iterator));
			// done
			return;
		}
	}
};

}
}

#endif
