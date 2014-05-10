#if !defined(CARES_SERVICE_DETAIL_OPERATORS_HPP_)
#define CARES_SERVICE_DETAIL_OPERATORS_HPP_

#include <ostream>

std::ostream & operator<<(std::ostream & lhs, const struct ares_addrttl & rhs)
{
	return lhs << inet_ntoa(rhs.ipaddr);
}

#endif