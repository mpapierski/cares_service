#include <iostream>
#include <cassert>
#include <ares.h>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include "cares_service/detail/iterator.hpp"

using namespace services::cares;

#define TESTCASE \
do \
{ \
	std::cout << __PRETTY_FUNCTION__ << std::endl; \
} while (0)

void test1()
{
	TESTCASE;
	a_reply_iterator it1, it2;
	assert(it1 == it2);
	assert(!(it1 != it2));
}

void test2()
{
	TESTCASE;
	auto vec = boost::make_shared<std::vector<struct ares_addrttl> >(10);
	a_reply_iterator it1(vec), end;
	assert(it1 != end);
}

void test3()
{
	TESTCASE;
	auto vec = boost::make_shared<std::vector<struct ares_addrttl> >(3);
	a_reply_iterator it1(vec), end;
	a_reply_iterator it2 = it1;
	assert(it1.addresses_ == it2.addresses_);
	assert(it1.it_ == it2.it_);
	assert(it1 == it2);
	++it1;
	assert(it1 != it2);
	++it1;
	assert(it1 != it2);
	++it1;
	assert(it1 == end);
}

int
main()
{
	test1();
	test2();
	test3();
}