#if !defined(CARES_SERVICE_DETAIL_ERROR_HPP_)
#define CARES_SERVICE_DETAIL_ERROR_HPP_

#include <boost/system/error_code.hpp>

namespace services {
namespace cares {
namespace detail {

struct error_category
	: boost::system::error_category

{
	const char * name() const BOOST_SYSTEM_NOEXCEPT
	{
		return "c-ares";
	}
	std::string message(int rc) const BOOST_SYSTEM_NOEXCEPT
	{
		return ::ares_strerror(rc);
	}
};

} // end namespace detail

inline
boost::system::error_category & get_error_category()
{
	static detail::error_category category;
	return category;
}

} // end namespace cares
} // end namespace services

#endif