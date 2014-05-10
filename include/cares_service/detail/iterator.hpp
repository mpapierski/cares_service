#if !defined(CARES_SERVICE_ITERATOR_HPP_)
#define CARES_SERVICE_ITERATOR_HPP_

#include <vector>
#include <iterator>

namespace services {
namespace cares {
namespace detail {

template <typename T>
struct result_iterator
	: public std::iterator<std::forward_iterator_tag, T>
{
	result_iterator()
	{
	}
	result_iterator(const boost::shared_ptr<std::vector<T> > & addresses)
		: addresses_(addresses)
		, it_(addresses_->begin())
	{
	}
	result_iterator & operator++()
	{
		++it_;
		return *this;
	}
	bool operator==(const result_iterator & rhs)
	{
		return (!addresses_ && !rhs.addresses_)
			|| (addresses_ && !rhs.addresses_ && it_ == addresses_->end())
			|| ((addresses_ == rhs.addresses_) && (it_ == rhs.it_));
	}
	bool operator!=(const result_iterator & rhs)
	{
		return !(operator==(rhs));
	}
	T & operator*()
	{
		assert(it_ != addresses_->end());
		return *it_;
	}
	T * operator->()
	{
		assert(it_ != addresses_->end());
		return &*it_;
	}
	typedef std::vector<T> result_type;
	boost::shared_ptr<result_type> addresses_;
	typename result_type::iterator it_;
};

}

typedef detail::result_iterator<struct ares_addrttl> a_reply_iterator;

}
}

#endif