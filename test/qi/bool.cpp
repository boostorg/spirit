/*=============================================================================
    Copyright (c) 2001-2010 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_directive.hpp>
#include <boost/cstdint.hpp>
#include "test.hpp"

///////////////////////////////////////////////////////////////////////////////
struct backwards_bool_policies : boost::spirit::qi::bool_policies<>
{
    // we want to interpret a 'true' spelled backwards as 'false'
    template <typename Iterator, typename Attribute>
    static bool
    parse_false(Iterator& first, Iterator const& last, Attribute& attr)
    {
        namespace spirit = boost::spirit;
        namespace qi = boost::spirit::qi;
        if (qi::detail::string_parse("eurt", first, last, qi::unused))
        {
            spirit::traits::assign_to(false, attr);    // result is false
            return true;
        }
        return false;
    }
};

///////////////////////////////////////////////////////////////////////////////
struct test_bool_type
{
    test_bool_type(bool b) : b(b) {}    // provide conversion
    bool b;
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    using spirit_test::test_attr;
    using spirit_test::test;
    using boost::spirit::qi::bool_;

    {
        BOOST_TEST(test("true", bool_));
        BOOST_TEST(test("false", bool_));
        BOOST_TEST(!test("fasle", bool_));
    }

    {
        using boost::spirit::qi::true_;
        using boost::spirit::qi::false_;

        BOOST_TEST(test("true", true_));
        BOOST_TEST(!test("true", false_));
        BOOST_TEST(test("false", false_));
        BOOST_TEST(!test("false", true_));
    }

    {
        using boost::spirit::qi::true_;
        using boost::spirit::qi::false_;
        using boost::spirit::qi::no_case;

        BOOST_TEST(test("True", no_case[bool_]));
        BOOST_TEST(test("False", no_case[bool_]));
        BOOST_TEST(test("True", no_case[true_]));
        BOOST_TEST(test("False", no_case[false_]));
    }

    {
        bool b = false;
        BOOST_TEST(test_attr("true", bool_, b) && b);
        BOOST_TEST(test_attr("false", bool_, b) && !b);
        BOOST_TEST(!test_attr("fasle", bool_, b));
    }

    {
        typedef boost::spirit::qi::bool_parser<bool, backwards_bool_policies> 
            backwards_bool_type;
        backwards_bool_type const backwards_bool = backwards_bool_type();

        BOOST_TEST(test("true", backwards_bool));
        BOOST_TEST(test("eurt", backwards_bool));
        BOOST_TEST(!test("false", backwards_bool));
        BOOST_TEST(!test("fasle", backwards_bool));

        bool b = false;
        BOOST_TEST(test_attr("true", backwards_bool, b) && b);
        BOOST_TEST(test_attr("eurt", backwards_bool, b) && !b);
        BOOST_TEST(!test_attr("false", backwards_bool, b));
        BOOST_TEST(!test_attr("fasle", backwards_bool, b));
    }

    {
        typedef boost::spirit::qi::bool_parser<test_bool_type> 
            bool_test_type;
        bool_test_type const test_bool = bool_test_type();

        BOOST_TEST(test("true", test_bool));
        BOOST_TEST(test("false", test_bool));
        BOOST_TEST(!test("fasle", test_bool));

        test_bool_type b = false;
        BOOST_TEST(test_attr("true", test_bool, b) && b.b);
        BOOST_TEST(test_attr("false", test_bool, b) && !b.b);
        BOOST_TEST(!test_attr("fasle", test_bool, b));
    }

    return boost::report_errors();
}
