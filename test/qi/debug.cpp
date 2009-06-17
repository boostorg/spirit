/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_SPIRIT_DEBUG

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_debug.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <string>
#include <iostream>
#include "test.hpp"

using namespace spirit_test;

int
main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::qi;
    using namespace boost::spirit::ascii;

    { // basic tests
        rule<char const*> a, b, c, start;

        a = 'a';
        b = 'b';
        c = 'c';
        BOOST_SPIRIT_DEBUG_NODE(a);
        BOOST_SPIRIT_DEBUG_NODE(b);
        BOOST_SPIRIT_DEBUG_NODE(c);

        start = *(a | b | c);
        BOOST_SPIRIT_DEBUG_NODE(start);
        BOOST_TEST(test("abcabcacb", start));

        start = (a | b) >> (start | b);
        BOOST_SPIRIT_DEBUG_NODE(start);
        BOOST_TEST(test("aaaabababaaabbb", start));
        BOOST_TEST(test("aaaabababaaabba", start, false));
    }

    { // basic tests w/ skipper

        rule<char const*, space_type> a, b, c, start;

        a = 'a';
        b = 'b';
        c = 'c';
        BOOST_SPIRIT_DEBUG_NODE(a);
        BOOST_SPIRIT_DEBUG_NODE(b);
        BOOST_SPIRIT_DEBUG_NODE(c);

        start = *(a | b | c);
        BOOST_TEST(test(" a b c a b c a c b", start, space));

        // allow no skipping too:
        BOOST_SPIRIT_DEBUG_NODE(start);
        BOOST_TEST(test("abcabcacb", start));

        start = (a | b) >> (start | b);
        BOOST_SPIRIT_DEBUG_NODE(start);
        BOOST_TEST(test(" a a a a b a b a b a a a b b b ", start, space));
        BOOST_TEST(test(" a a a a b a b a b a a a b b a ", start, space, false));
    }

    { // error handling

        using namespace boost::phoenix;
        using namespace boost::spirit::ascii;
        using boost::phoenix::val;
        using boost::spirit::int_;
        using boost::spirit::arg_names::_4; // what
        using boost::spirit::arg_names::_3; // error pos
        using boost::spirit::arg_names::_2; // end
        using boost::spirit::qi::fail;

        rule<char const*> r;
        r = char_('(') > int_ > ',' > int_ > ')';

        on_error<fail>
        (
            r, std::cout
                << val("Error! Expecting: ")
                << _4
                << val(" Here: \"")
                << construct<std::string>(_3, _2)
                << val("\"")
                << std::endl
        );

        BOOST_SPIRIT_DEBUG_NODE(r);
        BOOST_TEST(test("(123,456)", r));
        BOOST_TEST(!test("(abc,def)", r));
        BOOST_TEST(!test("(123,456]", r));
        BOOST_TEST(!test("(123;456)", r));
        BOOST_TEST(!test("[123,456]", r));
    }

    return boost::report_errors();
}

