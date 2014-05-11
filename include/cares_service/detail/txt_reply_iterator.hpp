#if !defined(CARES_SERVICE_DETAIL_TXT_REPLY_ITERATOR_HPP_)
#define CARES_SERVICE_DETAIL_TXT_REPLY_ITERATOR_HPP_

#include <vector>
#include <iterator>

namespace services {
namespace cares {

struct txt_reply_iterator
	: public std::iterator<std::forward_iterator_tag, std::string>
{
	boost::shared_ptr<struct ares_txt_reply> reply_;
	struct ares_txt_reply * current_;
	txt_reply_iterator()
		: current_(nullptr)
	{

	}
	txt_reply_iterator(boost::shared_ptr<struct ares_txt_reply> reply)
		: reply_(reply)
		, current_(reply_.get())
	{
	}
	txt_reply_iterator & operator++()
	{
		current_ = current_->next;
		return *this;
	}
	bool operator==(const txt_reply_iterator & rhs)
	{
		return current_ == rhs.current_;
	}
	bool operator!=(const txt_reply_iterator & rhs)
	{
		return !(operator==(rhs));
	}
	std::string operator*()
	{
		assert(current_);
		return std::string(reinterpret_cast<char *>(current_->txt), current_->length);
	}
};

}
}

#endif