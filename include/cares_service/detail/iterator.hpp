#if !defined(CARES_SERVICE_ITERATOR_HPP_)
#define CARES_SERVICE_ITERATOR_HPP_

#include <vector>
#include <iterator>

namespace services {
namespace cares {
namespace detail {

struct iterator
	: public std::iterator<std::forward_iterator_tag, struct ares_addrttl>
{
	iterator()
	{
	}
	iterator(const boost::shared_ptr<std::vector<struct ares_addrttl> > & addresses)
		: addresses_(addresses)
		, it_(addresses_->begin())
	{
	}
	iterator & operator++()
	{
		++it_;
		return *this;
	}
#if 0
	iterator operator++(int)
	{
		iterator tmp(*this);
		operator++();
		return tmp;
	}
#endif
	bool operator==(const iterator & rhs)
	{
		return (!addresses_ && !rhs.addresses_)
			|| (addresses_ && !rhs.addresses_ && it_ == addresses_->end())
			|| ((addresses_ == rhs.addresses_) && (it_ == rhs.it_));
	}
	bool operator!=(const iterator & rhs)
	{
		return !(operator==(rhs));
	}
	struct ares_addrttl & operator*()
	{
		assert(it_ != addresses_->end());
		return *it_;
	}
	struct ares_addrttl * operator->()
	{
		assert(it_ != addresses_->end());
		return &*it_;
	}
	typedef std::vector<struct ares_addrttl> addresses_type;
	boost::shared_ptr<addresses_type> addresses_;
	addresses_type::iterator it_;
};

}
}
}

#endif