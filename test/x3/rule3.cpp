/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// this file deliberately contains non-ascii characters
// boostinspect:noascii

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/actor.hpp>
#include <boost/ref.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/object.hpp>
//~ #include <boost/phoenix/bind.hpp>
//~ #include <boost/fusion/include/std_pair.hpp>

#include <string>
#include <cstring>
#include <iostream>
#include "test.hpp"

using boost::spirit::x3::get;
using boost::spirit::x3::rule_context_tag;
namespace x3 = boost::spirit::x3;

struct f
{
    template <typename Context>
    void operator()(Context const& ctx, char c) const
    {
        get<rule_context_tag>(ctx).val() += c;
    }
};

struct f1
{
    template <typename Context>
    void operator()(Context const& ctx, char c) const
    {
        x3::_val(ctx) = c + std::get<0>(x3::_params(ctx));
    }
};

struct f2
{
    char& ch;
    
    template <typename Context>
    void operator()(Context const& ctx, char c) const
    {
        ch = c;
    }
};

int
main()
{
    using spirit_test::test_attr;
    using spirit_test::test;

    using namespace boost::spirit::x3::ascii;
    using boost::spirit::x3::locals;
    using boost::spirit::x3::rule;
    using boost::spirit::x3::int_;
    //~ using boost::spirit::x3::uint_;
    //~ using boost::spirit::x3::fail;
    //~ using boost::spirit::x3::on_error;
    //~ using boost::spirit::x3::debug;
    using boost::spirit::x3::lit;
    using boost::spirit::x3::_val;

    namespace phx = boost::phoenix;

    { // synth attribute value-init

        std::string s;
        typedef rule<class r, std::string> rule_type;

        auto rdef = rule_type()
            = alpha                 [f()]
            ;

        BOOST_TEST(test_attr("abcdef", +rdef, s));
        BOOST_TEST(s == "abcdef");
    }

    { // synth attribute value-init

        std::string s;
        typedef rule<class r, std::string> rule_type;
       
        auto rdef = rule_type() =
            alpha /
               [](auto& r, char c)
               {
                  _val(r) += c;
               }
            ;

        BOOST_TEST(test_attr("abcdef", +rdef, s));
        BOOST_TEST(s == "abcdef");
    }

    { // context (w/arg) tests
        BOOST_SPIRIT_USE_ACTORS(_val, _1, _r1, _r2)
        
        char ch;
        rule<class A, char(int)> a; // 1 arg
        auto adef(a = alpha[_val = _1 + _r1]);

        BOOST_TEST(test("x", adef(1)[phx::ref(ch) = _1]));
        BOOST_TEST(ch == 'x' + 1);

        BOOST_TEST(test_attr("a", adef(1), ch)); // allow scalars as rule args too.
        BOOST_TEST(ch == 'a' + 1);

        rule<class B, char(int, int)> b; // 2 args
        auto bdef(b = alpha[_val = _1 + _r1 + _r2]);
        BOOST_TEST(test_attr("a", bdef(1, 2), ch));
        BOOST_TEST(ch == 'a' + 1 + 2);
    }

    { // context (w/ reference arg) tests
        BOOST_SPIRIT_USE_ACTORS(_1, _r1)
        
        char ch;
        rule<class A, void(char&)> a; // 1 arg (reference)
        auto adef(a = alpha[_r1 = _1]);

        BOOST_TEST(test("x", adef(boost::ref(ch))));
        BOOST_TEST(ch == 'x');
    }
#if 0 // <- wait until I submmit the patch for char
    { // context (w/locals) tests
        BOOST_SPIRIT_USE_ACTORS(_1, _a)
        
        rule<class A> a; // 1 local
        auto adef(a = locals<char>()[alpha[_a = _1] >> char_(_a)]);
        BOOST_TEST(test("aa", adef));
        BOOST_TEST(!test("ax", adef));
    }


    { // context (w/args and locals) tests
        BOOST_SPIRIT_USE_ACTORS(_1, _a, _r1)

        rule<class A, void(int)> a; // 1 arg + 1 local
        auto adef(a = locals<char>()[alpha[_a = _1 + _r1] >> char_(_a)]);
        BOOST_TEST(test("ab", adef(1)));
        BOOST_TEST(test("xy", adef(1)));
        BOOST_TEST(!test("ax", adef(1)));
    }
#endif
    // $$$ No longer relevant $$$
    //~ { // void() has unused type (void == unused_type)

        //~ std::pair<int, char> attr;
        //~ rule<char const*, void()> r;
        //~ r = char_;
        //~ BOOST_TEST(test_attr("123ax", int_ >> char_ >> r, attr));
        //~ BOOST_TEST(attr.first == 123);
        //~ BOOST_TEST(attr.second == 'a');
    //~ }

    // $$$ Not yet implemented $$$
    //~ { // bug: test that injected attributes are ok

        //~ rule<char const*, char(int) > r;

        //~ // problem code:
        //~ r = char_(_r1)[_val = _1];
    //~ }

    return boost::report_errors();
}

