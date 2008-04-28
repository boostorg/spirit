/*=============================================================================
    Copyright (c) 2001-2007 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <string>
#include <iostream>
#include "test.hpp"

using namespace spirit_test;
using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::ascii;
using namespace boost::spirit::arg_names;

struct num_list : grammar_<char const*, space_type>
{
    num_list() : base_type(start)
    {
        using boost::spirit::int_;
        num = int_;
        start = num >> *(',' >> num);
    }

    rule<char const*, space_type> start, num;
};

struct inh_g : grammar_def<char const*, int(int), space_type>
{
    inh_g()
    {
        start = lit("inherited")[_val = _r1];
    }

    rule<char const*, int(int), space_type> start, num;
};

struct my_skipper : grammar_def<char const*>
{
    my_skipper()
    {
        start = space;
    }

    rule<char const*> start, num;
};

struct num_list2 : grammar_def<char const*, grammar<my_skipper> >
{
    num_list2()
    {
        using boost::spirit::int_;
        num = int_;
        start = num >> *(',' >> num);
    }

    rule<char const*, grammar<my_skipper> > start, num;
};

template <typename Iterator, typename Skipper>
struct num_list3 : grammar_def<Iterator, Skipper>
{
    template <typename Class>
    num_list3(Class& self)
    {
        using boost::spirit::int_;
        num = int_;
        start = num >> *(',' >> num);
    }

    rule<Iterator, Skipper> start, num;
};

int
main()
{
    { // simple grammar test

        num_list nlist;
        BOOST_TEST(test("123, 456, 789", nlist, space));
    }

    { // simple grammar test with user-skipper

        num_list2 def;
        grammar<num_list2> nlist(def);
        my_skipper skipdef;
        grammar<my_skipper> skip(skipdef);
        BOOST_TEST(test("123, 456, 789", nlist, skip));
    }

    { // direct access to the rules

        num_list def;
        BOOST_TEST(test("123", def.num));
        BOOST_TEST(test("123, 456, 789", def.start, space));
    }

    { // grammar with inherited attributes

        inh_g def;
        grammar<inh_g> g(def);
        int n = -1;
        BOOST_TEST(test_attr("inherited", def.start(123), n, space)); // direct to the rule
        BOOST_TEST(n == 123);
        BOOST_TEST(test_attr("inherited", g(123), n, space)); // using the grammar
        BOOST_TEST(n == 123);
    }

    { // grammar_class test (no skipper)

        grammar_class<num_list3> nlist;

        char const* first = "123,456,789";
        char const* last = first;
        while (*last)
            last++;
        BOOST_TEST(parse(first, last, nlist) && (first == last));
    }

    { // grammar_class test (w/skipper)

        grammar_class<num_list3> nlist;

        char const* first = "123, 456, 789";
        char const* last = first;
        while (*last)
            last++;
        BOOST_TEST(phrase_parse(first, last, nlist, space)
            && (first == last));
    }
    return boost::report_errors();
}

