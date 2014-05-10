#if !defined(CARES_SERVICE_DETAIL_OPERATORS_HPP_)
#define CARES_SERVICE_DETAIL_OPERATORS_HPP_

#include <ostream>
#include <ares.h>

std::ostream & operator<<(std::ostream & lhs, const struct ares_addrttl & rhs)
{
	char buf[1024] = {0};
	const char * result = ares_inet_ntop(AF_INET, &(rhs.ipaddr), buf, 1023);
	assert(result == buf);
	return lhs << result;
}

std::ostream & operator<<(std::ostream & lhs, const struct ares_addr6ttl & rhs)
{
	char buf[1024] = {0};
	const char * result = ares_inet_ntop(AF_INET6, &(rhs.ip6addr), buf, 1023);
	assert(result == buf);
	return lhs << result;
}

#endif