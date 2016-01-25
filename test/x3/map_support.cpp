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
#include "test.hpp"

namespace x3 = boost::spirit::x3;

x3::rule<class map_rule, std::map<std::string,std::string>> const map_rule("map");
x3::rule<class unordered_map_rule, std::unordered_map<std::string,std::string>> const unordered_map_rule("unordered_map");

x3::rule<class multimap_rule, std::multimap<std::string,std::string>> const multimap_rule("multimap");
x3::rule<class unordered_multimap_rule, std::unordered_multimap<std::string,std::string>> const unordered_multimap_rule("unordered_multimap");

x3::rule<class pair_rule, std::pair<std::string,std::string>> const pair_rule("pair");
x3::rule<class string_rule, std::string> const string_rule("string");

auto const map_rule_def = pair_rule % x3::lit(',');
auto const unordered_map_rule_def = pair_rule % x3::lit(',');

auto const multimap_rule_def = pair_rule % x3::lit(',');
auto const unordered_multimap_rule_def = pair_rule % x3::lit(',');

auto const pair_rule_def = string_rule > x3::lit('=') > string_rule;
auto const string_rule_def = x3::lexeme[*x3::alnum];

BOOST_SPIRIT_DEFINE(map_rule, unordered_map_rule, multimap_rule, unordered_multimap_rule, pair_rule, string_rule);

template <typename Map>
void test_map_support(const auto& rule)
{
    using spirit_test::test_attr;

	Map map;
    BOOST_TEST(test_attr("k1=v1,k2=v2,k2=v3", rule, map));
    BOOST_TEST(map.size() == 2);
    BOOST_TEST(map.at("k1") == "v1");
    BOOST_TEST(map.at("k2") == "v2");
}

template <typename Map>
void test_multimap_support(const auto& rule)
{
    using spirit_test::test_attr;

	Map map;
    BOOST_TEST(test_attr("k1=v1,k2=v2,k2=v3", map_rule, map));
    BOOST_TEST(map.size() == 3);
    auto const range1 = map.equal_range("k1");
    BOOST_TEST(std::find_if(range1.first, range1.second, [](const auto& pair){return pair.second == "v1";}) != range1.second);
    auto const range2 = map.equal_range("k2");
    BOOST_TEST(std::find_if(range2.first, range2.second, [](const auto& pair){return pair.second == "v2";}) != range2.second);
    BOOST_TEST(std::find_if(range2.first, range2.second, [](const auto& pair){return pair.second == "v3";}) != range2.second);
}

int
main()
{
	test_map_support<std::map<std::string,std::string>>(map_rule);
	test_map_support<std::unordered_map<std::string,std::string>>(unordered_map_rule);

	test_multimap_support<std::multimap<std::string,std::string>>(multimap_rule);
	test_multimap_support<std::unordered_multimap<std::string,std::string>>(unordered_multimap_rule);

    return boost::report_errors();
}
