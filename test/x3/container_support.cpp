/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <iostream>
#include <map>
#include <unordered_map>
#include <boost/unordered_map.hpp>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <unordered_set>
#include <boost/unordered_set.hpp>
#include <string>
#include "test.hpp"

namespace x3 = boost::spirit::x3;

x3::rule<class std_map_rule, std::map<std::string,std::string>> const std_map_rule("std_map");
x3::rule<class std_unordered_map_rule, std::unordered_map<std::string,std::string>> const std_unordered_map_rule("std_unordered_map");
x3::rule<class boost_unordered_map_rule, boost::unordered_map<std::string,std::string>> const boost_unordered_map_rule("boost_unordered_map");

x3::rule<class std_multimap_rule, std::multimap<std::string,std::string>> const std_multimap_rule("std_multimap");
x3::rule<class std_unordered_multimap_rule, std::unordered_multimap<std::string,std::string>> const std_unordered_multimap_rule("std_unordered_multimap");
x3::rule<class boost_unordered_multimap_rule, boost::unordered_multimap<std::string,std::string>> const boost_unordered_multimap_rule("boost_unordered_multimap");

x3::rule<class std_vector_rule, std::vector<std::string>> const std_vector_rule("std_vector");
x3::rule<class std_list_rule, std::list<std::string>> const std_list_rule("std_list");
x3::rule<class std_deque_rule, std::deque<std::string>> const std_deque_rule("std_deque");

x3::rule<class std_set_rule, std::set<std::string>> const std_set_rule("std_set");
x3::rule<class std_unordered_set_rule, std::unordered_set<std::string>> const std_unordered_set_rule("std_unordered_set");
x3::rule<class boost_unordered_set_rule, boost::unordered_set<std::string>> const boost_unordered_set_rule("boost_unordered_set");

x3::rule<class std_multiset_rule, std::multiset<std::string>> const std_multiset_rule("std_multiset");
x3::rule<class std_unordered_multiset_rule, std::unordered_multiset<std::string>> const std_unordered_multiset_rule("std_unordered_multiset");
x3::rule<class boost_unordered_multiset_rule, boost::unordered_multiset<std::string>> const boost_unordered_multiset_rule("boost_unordered_multiset");

x3::rule<class std_string_rule, std::string> const std_string_rule("std_string");

x3::rule<class pair_rule, std::pair<std::string,std::string>> const pair_rule("pair");
x3::rule<class string_rule, std::string> const string_rule("string");

auto const std_map_rule_def = pair_rule % x3::lit(',');
auto const std_unordered_map_rule_def = pair_rule % x3::lit(',');
auto const boost_unordered_map_rule_def = pair_rule % x3::lit(',');

auto const std_multimap_rule_def = pair_rule % x3::lit(',');
auto const std_unordered_multimap_rule_def = pair_rule % x3::lit(',');
auto const boost_unordered_multimap_rule_def = pair_rule % x3::lit(',');

auto const std_vector_rule_def = string_rule % x3::lit(',');
auto const std_list_rule_def = string_rule % x3::lit(',');
auto const std_deque_rule_def = string_rule % x3::lit(',');

auto const std_set_rule_def = string_rule % x3::lit(',');
auto const std_unordered_set_rule_def = string_rule % x3::lit(',');
auto const boost_unordered_set_rule_def = string_rule % x3::lit(',');

auto const std_multiset_rule_def = string_rule % x3::lit(',');
auto const std_unordered_multiset_rule_def = string_rule % x3::lit(',');
auto const boost_unordered_multiset_rule_def = string_rule % x3::lit(',');

auto const std_string_rule_def = string_rule % x3::lit(',');

auto const pair_rule_def = string_rule > x3::lit('=') > string_rule;
auto const string_rule_def = x3::lexeme[*x3::alnum];

BOOST_SPIRIT_DEFINE
(
	std_map_rule, std_unordered_map_rule, boost_unordered_map_rule,
	std_multimap_rule, std_unordered_multimap_rule, boost_unordered_multimap_rule,
	std_vector_rule, std_list_rule, std_deque_rule,
	std_set_rule, std_unordered_set_rule, boost_unordered_set_rule,
	std_multiset_rule, std_unordered_multiset_rule, boost_unordered_multiset_rule,
	std_string_rule,
	pair_rule, string_rule
);

template <typename Container>
void test_map_support(const auto& rule)
{
    using spirit_test::test_attr;

    Container container;
    BOOST_TEST(test_attr("k1=v1,k2=v2,k2=v3", rule, container));

    BOOST_TEST(container.size() == 2);

    Container compare {{"k1", "v1"}, {"k2", "v2"}};
    BOOST_TEST(container == compare);
}

template <typename Container>
void test_multimap_support(const auto& rule)
{
    using spirit_test::test_attr;

    Container container;
    BOOST_TEST(test_attr("k1=v1,k2=v2,k2=v3", rule, container));

    BOOST_TEST(container.size() == 3);

    Container compare {{"k1", "v1"}, {"k2", "v2"}, {"k2", "v3"}};
    BOOST_TEST(container == compare);
}

template <typename Container>
void test_sequence_support(const auto& rule)
{
    using spirit_test::test_attr;

    Container container;
    BOOST_TEST(test_attr("e1,e2,e2", rule, container));

    BOOST_TEST(container.size() == 3);

    Container compare {"e1", "e2", "e2"};
    BOOST_TEST(container == compare);
}

template <typename Container>
void test_set_support(const auto& rule)
{
    using spirit_test::test_attr;

    Container container;
    BOOST_TEST(test_attr("e1,e2,e2", rule, container));

    BOOST_TEST(container.size() == 2);

    Container compare {"e1", "e2"};
    BOOST_TEST(container == compare);
}

template <typename Container>
void test_multiset_support(const auto& rule)
{
    using spirit_test::test_attr;

    Container container;
    BOOST_TEST(test_attr("e1,e2,e2", rule, container));

    BOOST_TEST(container.size() == 3);

    Container compare {"e1", "e2", "e2"};
    BOOST_TEST(container == compare);
}

template <typename Container>
void test_string_support(const auto& rule)
{
    using spirit_test::test_attr;

    Container container;
    BOOST_TEST(test_attr("e1,e2,e2", rule, container));

    BOOST_TEST(container.size() == 6);

    Container compare {"e1e2e2"};
    BOOST_TEST(container == compare);
}

static_assert(typename x3::traits::detail::has_reserve_method<std::vector<int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<std::string>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<std::unordered_set<int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<boost::unordered_set<int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<std::unordered_multiset<int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<boost::unordered_multiset<int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<std::unordered_map<int,int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<boost::unordered_map<int,int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<std::unordered_multimap<int,int>>::type{}, "reserve problem");
static_assert(typename x3::traits::detail::has_reserve_method<boost::unordered_multimap<int,int>>::type{}, "reserve problem");

static_assert(!typename x3::traits::detail::has_reserve_method<std::deque<int>>::type{}, "reserve problem");
static_assert(!typename x3::traits::detail::has_reserve_method<std::list<int>>::type{}, "reserve problem");
static_assert(!typename x3::traits::detail::has_reserve_method<std::set<int>>::type{}, "reserve problem");
static_assert(!typename x3::traits::detail::has_reserve_method<std::multiset<int>>::type{}, "reserve problem");
static_assert(!typename x3::traits::detail::has_reserve_method<std::map<int,int>>::type{}, "reserve problem");
static_assert(!typename x3::traits::detail::has_reserve_method<std::multimap<int,int>>::type{}, "reserve problem");

int
main()
{
	test_map_support<std::map<std::string,std::string>>(std_map_rule);
	test_map_support<std::unordered_map<std::string,std::string>>(std_unordered_map_rule);
	test_map_support<boost::unordered_map<std::string,std::string>>(boost_unordered_map_rule);

	test_multimap_support<std::multimap<std::string,std::string>>(std_multimap_rule);
	test_multimap_support<std::unordered_multimap<std::string,std::string>>(std_unordered_multimap_rule);
	test_multimap_support<boost::unordered_multimap<std::string,std::string>>(boost_unordered_multimap_rule);

	test_sequence_support<std::vector<std::string>>(std_vector_rule);
	test_sequence_support<std::list<std::string>>(std_list_rule);
	test_sequence_support<std::deque<std::string>>(std_deque_rule);

	test_set_support<std::set<std::string>>(std_set_rule);
	test_set_support<std::unordered_set<std::string>>(std_unordered_set_rule);
	test_set_support<boost::unordered_set<std::string>>(boost_unordered_set_rule);

	test_multiset_support<std::multiset<std::string>>(std_multiset_rule);
	test_multiset_support<std::unordered_multiset<std::string>>(std_unordered_multiset_rule);
	test_multiset_support<boost::unordered_multiset<std::string>>(boost_unordered_multiset_rule);

	test_string_support<std::string>(std_string_rule);

    return boost::report_errors();
}
