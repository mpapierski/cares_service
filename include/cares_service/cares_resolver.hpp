#if !defined(CARES_SERVICE_CARES_RESOLVER_HPP_)
#define CARES_SERVICE_CARES_RESOLVER_HPP_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "cares_service/cares_service.hpp"
#include "cares_service/detail/iterator.hpp"
#include "cares_service/detail/resolve_result.hpp"

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
	void parse_reply(const unsigned char * abuf, int alen, struct ares_addrttl * addresses, int * matches, boost::system::error_code & ec)
	{
		ec.clear();
		ec.assign(::ares_parse_a_reply(abuf, alen, nullptr, addresses, matches), get_error_category());
	}
	void parse_reply(const unsigned char * abuf, int alen, struct ares_addr6ttl * addresses, int * matches, boost::system::error_code & ec)
	{
		ec.clear();
		ec.assign(::ares_parse_aaaa_reply(abuf, alen, nullptr, addresses, matches), get_error_category());
	}
	template <typename T>
	void parse_reply_into(const unsigned char * abuf, int alen, std::vector<T> & output, boost::system::error_code & ec)
	{
		ec.clear();
		for (int size = 8; true; size += 4)
		{
			// allocate memory
			output.resize(size);
			// copy size
			int matches = size;
			// parse the answer
			parse_reply(abuf, alen, output.data(), &matches, ec);
			
			// on failure we leap out, otherwise we continue to allocate more memory
			if (ec)
			{
				return;
			}
			
			// check if we had enough space
			if (matches >= size)
			{
				continue;
			}
			
			output.resize(matches);
			return;
		}
		assert(0);
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
		Callback * cb = new Callback(std::move(callback));
		chan->query(input, ns_c_in, ns_t_a,
			boost::bind(&resolver::a_callback<Callback>, this, _1, _2, _3, _4, cb));
		chan->getsock();
	};
	template <typename Callback>
	void resolve_aaaa(const std::string & input, const Callback & callback)
	{
		boost::shared_ptr<detail::channel> chan = boost::asio::use_service<cares>(io_service_).get_channel();
		boost::system::error_code ec;
		chan->init(ec);
		if (ec)
		{
			get_io_service().post(boost::bind(detail::resolve_result<Callback>(callback, ec), aaaa_reply_iterator()));
			return;
		}
		Callback * cb = new Callback(std::move(callback));
		chan->query(input, ns_c_in, ns_t_aaaa,
			boost::bind(&resolver::aaaa_callback<Callback>, this, _1, _2, _3, _4, cb));
		chan->getsock();
	};
	/**
	 * Generic callback for both A and AAAA replies
	 */
	template <typename Callback>
	void aaaa_callback(int status, int timeouts, unsigned char * abuf, int alen, Callback * callback)
	{
		std::unique_ptr<Callback> cb(callback);
		boost::system::error_code ec(status, get_error_category());
		if (ec)
		{
			get_io_service().post(boost::bind(detail::resolve_result<Callback>(*callback, ec), aaaa_reply_iterator()));
			return;
		}
		std::vector<struct ares_addr6ttl> vec;
		parse_reply_into(abuf, alen, vec, ec);
		if (ec)
		{
			get_io_service().post(boost::bind(detail::resolve_result<Callback>(*callback, ec), aaaa_reply_iterator()));
			return;
		}
		aaaa_reply_iterator result_iterator(boost::make_shared<std::vector<struct ares_addr6ttl> >(vec));
		get_io_service().post(boost::bind(detail::resolve_result<Callback>(*callback, ec), result_iterator));
	}
	/**
	 * Generic callback for both A and AAAA replies
	 */
	template <typename Callback>
	void a_callback(int status, int timeouts, unsigned char * abuf, int alen, Callback * callback)
	{
		std::unique_ptr<Callback> cb(callback);
		boost::system::error_code ec(status, get_error_category());
		if (ec)
		{
			get_io_service().post(boost::bind(detail::resolve_result<Callback>(*callback, ec), a_reply_iterator()));
			return;
		}
		std::vector<struct ares_addrttl> vec;
		parse_reply_into(abuf, alen, vec, ec);
		if (ec)
		{
			get_io_service().post(boost::bind(detail::resolve_result<Callback>(*callback, ec), a_reply_iterator()));
			return;
		}
		a_reply_iterator result_iterator(boost::make_shared<std::vector<struct ares_addrttl> >(vec));
		get_io_service().post(boost::bind(detail::resolve_result<Callback>(*callback, ec), result_iterator));
	}
};

}
}

#endif
