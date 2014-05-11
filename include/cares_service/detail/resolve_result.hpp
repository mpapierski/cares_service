#if !defined(CARES_SERVICE_DETAIL_RESOLVE_RESULT_HPP_)
#define CARES_SERVICE_DETAIL_RESOLVE_RESULT_HPP_

namespace services {
namespace cares {
namespace detail {

template <typename Callback>
struct resolve_result
{
	Callback callback_;
	typedef void result_type;
	boost::system::error_code ec_;
	resolve_result(const Callback & callback, const boost::system::error_code & ec)
		: callback_(callback)
		, ec_(ec)
	{
	}
	template <typename T>
	void operator()(T && t)
	{
		callback_(ec_, std::forward<T>(t));
	}
};

}
}
}

#endif