/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#define BOOST_SPIRIT_X3_DEBUG

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/vector.hpp>

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include "test.hpp"

struct my_error_handler
{
    template <typename Iterator, typename Exception, typename Context>
    boost::spirit::x3::error_handler_result
    operator()(Iterator&, Iterator const& last, Exception const& x, Context const&) const
    {
        std::cout
            << "Error! Expecting: "
            << x.which()
            << ", got: \""
            << std::string(x.where(), last)
            << "\""
            << std::endl;
        return boost::spirit::x3::error_handler_result::fail;
    }
};

struct my_attribute
{
    bool alive = true;

    void access() const
    {
        BOOST_TEST(alive);
    }
    ~my_attribute()
    {
        alive = false;
    }

    friend std::ostream & operator << (std::ostream & os, my_attribute const & attr)
    {
        attr.access();
        return os << "my_attribute";
    }
};

namespace rule_defs
{
    using boost::spirit::x3::rule;
    auto a = rule<class a>() = 'a';
    auto b = rule<class b>() = 'b';
    auto c = rule<class c>() = 'c';
}
int
main()
{
    using spirit_test::test_attr;
    using spirit_test::test;

    using namespace boost::spirit::x3::ascii;
    using boost::spirit::x3::rule;
    using boost::spirit::x3::symbols;
    using boost::spirit::x3::int_;
    using boost::spirit::x3::alpha;

    //~ namespace phx = boost::phoenix;
    
    using namespace rule_defs;

    { // basic tests

        {
            auto start = *(a | b | c);
            BOOST_TEST(test("abcabcacb", start));
        }

        {
            auto recur_tests_not_space=[](auto start)
            {
                BOOST_TEST(test("aaaabababaaabbb", start));
                BOOST_TEST(test("aaaabababaaabba", start, false));
            };
            auto recur_tests_yes_space=[](auto start)
            {
                BOOST_TEST(test(" a a a a b a b a b a a a b b b ", start, space));
                BOOST_TEST(test(" a a a a b a b a b a a a b b a ", start, space, false));
            };
          #if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
            #if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP
              struct derived:boost::spirit::x3::gram_base<derived>
              {
                struct r_id{};
                const rule_b<r_id> r;
                derived():r{"r_rule"}{}
                  
                BOOST_SPIRIT_CRTP_DEFINE(r = (a | b) >> (r | b))            
              };
              derived d;
              recur_tests_not_space(d.r);
              recur_tests_yes_space(d.r);
            #else
              //Can't use context to store connection between
              //rhs and lhs of rule; and a namespace is not
              //provided to enable use of BOOST_SPIRIT_DEFINE
              //to do the connection; so, must do nothing.
            #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP
          #else
            rule<class start> start("start");
            auto start_def =
                start = (a | b) >> (start | b);
            recur_tests_not_space(start_def);                
            recur_tests_yes_space(start_def);                
          #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT

        }
    }

    { // basic tests w/ skipper

        {
            auto start = *(a | b | c);
            BOOST_TEST(test(" a b c a b c a c b ", start, space));
        }

    }

    { // std::container attributes

        typedef boost::fusion::vector<int, char> fs;
        rule<class start, std::vector<fs>> start("start");
        auto start_def =
            start = *(int_ >> alpha);

        BOOST_TEST(test("1 a 2 b 3 c", start_def, space));
    }

    { // error handling

        auto r_def = '(' > int_ > ',' > int_ > ')';
        auto r = r_def.on_error(my_error_handler());

        BOOST_TEST(test("(123,456)", r));
        BOOST_TEST(!test("(abc,def)", r));
        BOOST_TEST(!test("(123,456]", r));
        BOOST_TEST(!test("(123;456)", r));
        BOOST_TEST(!test("[123,456]", r));
    }

    {
        symbols<my_attribute> a{{{ "a", my_attribute{} }}};

        auto b = rule<struct b_id, my_attribute>("b") = a;

        my_attribute attr;

        BOOST_TEST(test_attr("a", b, attr));
    }

    return boost::report_errors();
}
