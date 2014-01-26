#if !defined(CARES_SERVICE_DETAIL_CHANNEL_HPP_)
#define CARES_SERVICE_DETAIL_CHANNEL_HPP_

namespace services {
namespace cares {
namespace detail {

struct channel
{
	bool initialized_;
	ares_channel channel_;
	channel()
		: initialized_(false)
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
		if (initialized_)
		{
			::ares_destroy(channel_);
		}
	}
	inline
	ares_channel get() const
	{
		return channel_;
	}
};
	
}
}
}

#endif