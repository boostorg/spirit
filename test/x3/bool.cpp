/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Bryce Lelbach
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include "test.hpp"

#include <boost/spirit/home/x3.hpp>

#include <string_view>
#include <iterator>

struct backwards_bool_policies : boost::spirit::x3::bool_policies<>
{
    // we want to interpret a 'true' spelled backwards as 'false'
    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename CaseCompare>
    [[nodiscard]] static constexpr bool
    parse_false(
        It& first, Se const& last, Attribute& attr, CaseCompare const& case_compare
    )
    {
        using namespace std::string_view_literals;
        namespace x3 = boost::spirit::x3;
        if (x3::detail::string_parse("eurt"sv, first, last, x3::unused, case_compare))
        {
            x3::traits::move_to(false, attr);   // result is false
            return true;
        }
        return false;
    }
};

int main()
{
    using spirit_test::test_attr;
    using spirit_test::test;
    using boost::spirit::x3::bool_;

    {
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(bool_);

        BOOST_TEST(test("true", bool_));
        BOOST_TEST(test("false", bool_));
        BOOST_TEST(!test("fasle", bool_));
    }

    {
        using boost::spirit::x3::true_;
        using boost::spirit::x3::false_;

        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(true_);
        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(false_);

        BOOST_TEST(test("true", true_));
        BOOST_TEST(!test("true", false_));
        BOOST_TEST(test("false", false_));
        BOOST_TEST(!test("false", true_));
    }

    {
        using boost::spirit::x3::true_;
        using boost::spirit::x3::false_;
        using boost::spirit::x3::no_case;

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
        using backwards_bool_type = boost::spirit::x3::bool_parser<bool, boost::spirit::x3::char_encoding::standard, backwards_bool_policies>;
        constexpr backwards_bool_type backwards_bool{};

        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(backwards_bool);

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
        struct test_bool_type
        {
            test_bool_type(bool b = false) : b(b) {}    // provide conversion
            bool b;
        };

        using bool_test_type = boost::spirit::x3::bool_parser<test_bool_type, boost::spirit::x3::char_encoding::standard>;
        constexpr bool_test_type test_bool{};

        BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(test_bool);

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
