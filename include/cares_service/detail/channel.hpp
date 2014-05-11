#if !defined(CARES_SERVICE_DETAIL_CHANNEL_HPP_)
#define CARES_SERVICE_DETAIL_CHANNEL_HPP_

#include <map>
#include <memory>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

namespace services {
namespace cares {
namespace detail {

/**
 * c-ares channel wrapper
 */
struct channel
	: boost::enable_shared_from_this<channel>
{
	boost::asio::io_service & io_service_;
	boost::asio::io_service::work keep_busy_;
	bool initialized_;
	ares_channel channel_;
	
	typedef std::map<int, boost::asio::posix::stream_descriptor> stream_descriptors_type;
	stream_descriptors_type stream_descriptors_;

	channel(boost::asio::io_service & io_service)
		: io_service_(io_service)
		, keep_busy_(io_service_)
		, initialized_(false)
		, channel_()
	{
	}
	void init(boost::system::error_code & ec)
	{
		assert(!initialized_);
		ec.clear();
		ec.assign(::ares_init(&channel_), get_error_category());
		initialized_ = !ec;
	}
	~channel()
	{
		assert(initialized_);
		::ares_destroy(channel_);
	}
	inline
	ares_channel & get()
	{
		assert(initialized_);
		return channel_;
	}
	stream_descriptors_type & get_stream_descriptors()
	{
		return stream_descriptors_;
	}
	template <typename Callback>
	void query(const std::string & name, int dnsclass, int type, const Callback & callback)
	{
		assert(initialized_);
		Callback * arg = new Callback(std::move(callback));
		::ares_query(channel_, name.c_str(), dnsclass, type, &query_callback_wrapper<Callback>, arg);
	}
	template <typename Callback>
	static void query_callback_wrapper(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
	{
		assert(arg);
		std::unique_ptr<Callback> callback(static_cast<Callback *>(arg));
		(*callback)(status, timeouts, abuf, alen);
	}
	void getsock()
	{
		assert(initialized_);
		boost::array<ares_socket_t, ARES_GETSOCK_MAXNUM> sockets_ = {{0}};
		int bitmask = ::ares_getsock(channel_, sockets_.data(), sockets_.size());
		for (int i = 0; i < ARES_GETSOCK_MAXNUM; ++i)
		{
			if (ARES_GETSOCK_READABLE(bitmask, i)
				|| ARES_GETSOCK_WRITABLE(bitmask, i))
			{
				boost::asio::posix::stream_descriptor descriptor(io_service_);
				auto result =
					stream_descriptors_.emplace(sockets_[i], std::move(descriptor));
				if (result.second)
				{
					assert(!result.first->second.is_open());
					result.first->second.assign(sockets_[i]);
				}
				assert(result.first->second.is_open());
				if (ARES_GETSOCK_READABLE(bitmask, i))
				{
					result.first->second.async_read_some(boost::asio::null_buffers(),
						boost::bind(&channel::read_handler, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred,
							sockets_[i]));
				}
				if (ARES_GETSOCK_WRITABLE(bitmask, i))
				{
					result.first->second.async_write_some(boost::asio::null_buffers(),
						boost::bind(&channel::write_handler, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred,
							sockets_[i]));
				}		
			}
		}
	}
	void read_handler(const boost::system::error_code & ec, std::size_t,
		ares_socket_t fd)
	{
		::ares_process_fd(channel_, fd, ARES_SOCKET_BAD);
		getsock();
	}
	void write_handler(const boost::system::error_code & ec, std::size_t,
		ares_socket_t fd)
	{
		::ares_process_fd(channel_, ARES_SOCKET_BAD, fd);
		getsock();
	}
};
	
}
}
}

#endif