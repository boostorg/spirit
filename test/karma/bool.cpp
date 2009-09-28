/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_directive.hpp>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
// special bool output policy allowing to spell true and false backwards
struct special_bool_policy : boost::spirit::karma::bool_policies<>
{
    template <typename CharEncoding, typename Tag
      , typename OutputIterator>
    static bool generate (OutputIterator& sink, bool b)
    {
        //  we want to spell the names of true and false backwards
        return boost::spirit::karma::string_inserter<CharEncoding, Tag>::
            call(sink, b ? "eurt" : "aslaf");
    }
};

///////////////////////////////////////////////////////////////////////////////
// special policy allowing to use any type as a boolean
struct test_bool_data
{
    test_bool_data(bool b) : b(b) {}

    bool b;
};

bool operator!=(test_bool_data const& lhs, test_bool_data const& rhs)
{
    return lhs.b != rhs.b;
}

struct test_bool_policy : boost::spirit::karma::bool_policies<>
{
    typedef boost::spirit::karma::bool_policies<> base_type;

    template <typename CharEncoding, typename Tag
      , typename OutputIterator>
    static bool generate (OutputIterator& sink, test_bool_data b)
    {
        //  call the base class supplying the actual bool value
        return base_type::template generate<CharEncoding, Tag>(sink, b.b);
    }
};


///////////////////////////////////////////////////////////////////////////////
int main()
{
    using boost::spirit::karma::bool_;
    using boost::spirit::karma::lit;
    using boost::spirit::karma::lower;
    using boost::spirit::karma::upper;

    {
        BOOST_TEST(test("false", bool_, false));
        BOOST_TEST(test("true", bool_, true));
        BOOST_TEST(test("false", bool_(false)));
        BOOST_TEST(test("true", bool_(true)));
        BOOST_TEST(test("false", bool_(false), false));
        BOOST_TEST(test("true", bool_(true), true));
        BOOST_TEST(!test("", bool_(false), true));
        BOOST_TEST(!test("", bool_(true), false));
        BOOST_TEST(test("false", lit(false)));
        BOOST_TEST(test("true", lit(true)));
    }

    {
        BOOST_TEST(test("false", lower[bool_], false));
        BOOST_TEST(test("true", lower[bool_], true));
        BOOST_TEST(test("false", lower[bool_(false)]));
        BOOST_TEST(test("true", lower[bool_(true)]));
        BOOST_TEST(test("false", lower[bool_(false)], false));
        BOOST_TEST(test("true", lower[bool_(true)], true));
        BOOST_TEST(!test("", lower[bool_(false)], true));
        BOOST_TEST(!test("", lower[bool_(true)], false));
        BOOST_TEST(test("false", lower[lit(false)]));
        BOOST_TEST(test("true", lower[lit(true)]));
    }

    {
        BOOST_TEST(test("FALSE", upper[bool_], false));
        BOOST_TEST(test("TRUE", upper[bool_], true));
        BOOST_TEST(test("FALSE", upper[bool_(false)]));
        BOOST_TEST(test("TRUE", upper[bool_(true)]));
        BOOST_TEST(test("FALSE", upper[bool_(false)], false));
        BOOST_TEST(test("TRUE", upper[bool_(true)], true));
        BOOST_TEST(!test("", upper[bool_(false)], true));
        BOOST_TEST(!test("", upper[bool_(true)], false));
        BOOST_TEST(test("FALSE", upper[lit(false)]));
        BOOST_TEST(test("TRUE", upper[lit(true)]));
    }

    {
        typedef boost::spirit::karma::bool_generator<bool, special_bool_policy> 
            backwards_bool_type;
        backwards_bool_type const backwards_bool;

        BOOST_TEST(test("aslaf", backwards_bool, false));
        BOOST_TEST(test("eurt", backwards_bool, true));
        BOOST_TEST(test("aslaf", backwards_bool(false)));
        BOOST_TEST(test("eurt", backwards_bool(true)));
        BOOST_TEST(test("aslaf", backwards_bool(false), false));
        BOOST_TEST(test("eurt", backwards_bool(true), true));
        BOOST_TEST(!test("", backwards_bool(false), true));
        BOOST_TEST(!test("", backwards_bool(true), false));
    }

    {
        typedef boost::spirit::karma::bool_generator<
            test_bool_data, test_bool_policy> test_bool_type;
        test_bool_type const test_bool;
        test_bool_data const test_false = test_bool_data(false);
        test_bool_data const test_true = test_bool_data(true);

        BOOST_TEST(test("false", test_bool, test_false));
        BOOST_TEST(test("true", test_bool, test_true));
        BOOST_TEST(test("false", test_bool(test_false)));
        BOOST_TEST(test("true", test_bool(test_true)));
        BOOST_TEST(test("false", test_bool(test_false), test_false));
        BOOST_TEST(test("true", test_bool(test_true), test_true));
        BOOST_TEST(!test("", test_bool(test_false), test_true));
        BOOST_TEST(!test("", test_bool(test_true), test_false));
    }

    return boost::report_errors();
}

