/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_auxiliary.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_directive.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_uint.hpp>


#include <iostream>
#include <string>
#include "test.hpp"

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    using namespace boost::spirit::ascii;
    using boost::spirit::qi::eps;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::uint_;

    {
        boost::iterator_range<char const*> range;
        std::string str;
        BOOST_TEST((test_attr("spirit_test_123", raw[alpha >> *(alnum | '_')], range)));
        BOOST_TEST((std::string(range.begin(), range.end()) == "spirit_test_123"));
        BOOST_TEST((test_attr("  spirit", raw[*alpha], range, space)));
        BOOST_TEST((std::string(range.begin(), range.end()) == "spirit"));
    }

    {
        std::string str;
        BOOST_TEST((test_attr("spirit_test_123", raw[alpha >> *(alnum | '_')], str)));
        BOOST_TEST((str == "spirit_test_123"));
    }

    {
        boost::iterator_range<char const*> range;
        BOOST_TEST((test("x", raw[alpha])));
        BOOST_TEST((test_attr("x", raw[alpha], range)));
        BOOST_TEST((std::string(range.begin(), range.end()) == "x"));
        BOOST_TEST((test_attr("x", raw[alpha] >> eps, range)));
        BOOST_TEST((std::string(range.begin(), range.end()) == "x"));
    }
    {
        std::vector<boost::iterator_range<const char*> > ranges;
        std::vector<std::string> strings;
        BOOST_TEST((test("number: 124number: 344", *("number: " >> raw[uint_]))));
        BOOST_TEST((test_attr("number: 124number: 344", *("number: " >> raw[uint_]), ranges)));
        BOOST_TEST((
            ranges.size() == 2 && 
            std::string(ranges[0].begin(), ranges[0].end()) == "124" &&
            std::string(ranges[1].begin(), ranges[1].end()) == "344"));
        BOOST_TEST((test_attr("number: 124number: 344", *("number: " >> raw[uint_]), strings)));
        BOOST_TEST((
        strings.size() == 2 && strings[0] == "124" && strings[1] == "344"));
    }

    return boost::report_errors();
}
