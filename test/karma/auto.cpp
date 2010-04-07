//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/karma_bool.hpp>
#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_nonterminal.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_directive.hpp>
#include <boost/spirit/include/karma_auto.hpp>

#include "test.hpp"

namespace karma = boost::spirit::karma;
namespace traits = boost::spirit::traits;

///////////////////////////////////////////////////////////////////////////////
template <typename Char, typename T>
bool test_create_generator(Char const *expected, T const& t)
{
    std::basic_string<Char> generated;
    std::back_insert_iterator<std::basic_string<Char> > sink(generated);

    BOOST_TEST((traits::meta_create_exists<karma::domain, T>::value));
    bool result = karma::generate(sink, karma::create_generator<T>(), t);

    spirit_test::print_if_failed("test_create_generator", result, generated, expected);
    return result && generated == expected;
}

template <typename Char, typename T>
bool test_create_generator_auto(Char const *expected, T const& t)
{
    std::basic_string<Char> generated;
    std::back_insert_iterator<std::basic_string<Char> > sink(generated);

    BOOST_TEST((traits::meta_create_exists<karma::domain, T>::value));
    bool result = karma::generate(sink, t);

    spirit_test::print_if_failed("test_create_generator (auto)", result, generated, expected);
    return result && generated == expected;
}

template <typename Char, typename Attribute>
bool test_rule(Char const *expected, Attribute const& attr)
{
    BOOST_TEST((traits::meta_create_exists<karma::domain, Attribute>::value));

    typedef typename spirit_test::output_iterator<Char>::type sink_type;
    karma::rule<sink_type, Attribute()> r = 
        karma::create_generator<Attribute>();
    return spirit_test::test(expected, r, attr);
}

template <typename Char, typename Attribute, typename Delimiter>
bool test_rule_delimited(Char const *expected, Attribute const& attr
  , Delimiter const& d)
{
    BOOST_TEST((traits::meta_create_exists<karma::domain, Attribute>::value));

    typedef typename spirit_test::output_iterator<Char>::type sink_type;
    karma::rule<sink_type, Attribute(), Delimiter> r = 
        karma::create_generator<Attribute>();
    return spirit_test::test_delimited(expected, r, attr, d);
}

struct my_type {};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    {
        BOOST_TEST((!traits::meta_create_exists<karma::domain, my_type>::value));
    }

    {
        // test primitive types
        BOOST_TEST(test_create_generator("true", true));
        BOOST_TEST(test_create_generator("1", 1));
        BOOST_TEST(test_create_generator("1.1", 1.1));
        BOOST_TEST(test_create_generator("test", std::string("test")));
        BOOST_TEST(test_create_generator("a", 'a'));
        BOOST_TEST(test_create_generator(L"a", L'a'));

        // test containers
        std::vector<int> v;
        v.push_back(0);
        v.push_back(1);
        v.push_back(2);
        BOOST_TEST(test_create_generator("012", v));

        std::list<int> l;
        l.push_back(0);
        l.push_back(1);
        l.push_back(2);
        BOOST_TEST(test_create_generator("012", l));

        // test optional
        boost::optional<int> o;
        BOOST_TEST(test_create_generator("", o));
        o = 1;
        BOOST_TEST(test_create_generator("1", o));

        // test alternative
        boost::variant<int, double, float, std::string> vv;
        vv = 1;
        BOOST_TEST(test_create_generator("1", vv));
        vv = 1.0;
        BOOST_TEST(test_create_generator("1.0", vv));
        vv = 1.0f;
        BOOST_TEST(test_create_generator("1.0", vv));
        vv = "some string";
        BOOST_TEST(test_create_generator("some string", vv));

        // test fusion sequence
        std::pair<int, double> p (1, 2.0);
        BOOST_TEST(test_create_generator("12.0", p));
    }

    {
        // test primitive types
//         BOOST_TEST(test_create_generator_auto("true", true));
//         BOOST_TEST(test_create_generator_auto("1", 1));
//         BOOST_TEST(test_create_generator_auto("1.1", 1.1));
//         BOOST_TEST(test_create_generator_auto("test", std::string("test")));
//         BOOST_TEST(test_create_generator_auto("a", 'a'));
//         BOOST_TEST(test_create_generator_auto(L"a", L'a'));

        // test containers
        std::vector<int> v;
        v.push_back(0);
        v.push_back(1);
        v.push_back(2);
        BOOST_TEST(test_create_generator_auto("012", v));

        std::list<int> l;
        l.push_back(0);
        l.push_back(1);
        l.push_back(2);
        BOOST_TEST(test_create_generator_auto("012", l));

        // test optional
        boost::optional<int> o;
        BOOST_TEST(test_create_generator_auto("", o));
        o = 1;
        BOOST_TEST(test_create_generator_auto("1", o));

        // test alternative
        boost::variant<int, double, float, std::string> vv;
        vv = 1;
        BOOST_TEST(test_create_generator_auto("1", vv));
        vv = 1.0;
        BOOST_TEST(test_create_generator_auto("1.0", vv));
        vv = 1.0f;
        BOOST_TEST(test_create_generator_auto("1.0", vv));
        vv = "some string";
        BOOST_TEST(test_create_generator_auto("some string", vv));

        // test fusion sequence
        std::pair<int, double> p (1, 2.0);
        BOOST_TEST(test_create_generator_auto("12.0", p));
    }

    {
        using karma::auto_;
        using karma::upper;
        using spirit_test::test;
        using spirit_test::test_delimited;

        // test primitive types
        BOOST_TEST(test("true", auto_, true));
        BOOST_TEST(test("1", auto_, 1));
        BOOST_TEST(test("1.1", auto_, 1.1));
        BOOST_TEST(test("test", auto_, "test"));
        BOOST_TEST(test(L"test", auto_, L"test"));
        BOOST_TEST(test("a", auto_, 'a'));
        BOOST_TEST(test(L"a", auto_, L'a'));

        BOOST_TEST(test("TRUE", upper[auto_], true));
        BOOST_TEST(test("TEST", upper[auto_], "test"));

        // test containers
        std::vector<int> v;
        v.push_back(0);
        v.push_back(1);
        v.push_back(2);
        BOOST_TEST(test("012", auto_, v));
        BOOST_TEST(test("0,1,2", auto_ % ',', v));
        BOOST_TEST(test_delimited("0,1,2,", auto_, v, ','));

        std::list<int> l;
        l.push_back(0);
        l.push_back(1);
        l.push_back(2);
        BOOST_TEST(test("012", auto_, l));
        BOOST_TEST(test("0,1,2", auto_ % ',', l));
        BOOST_TEST(test_delimited("0,1,2,", auto_, l, ','));

        // test optional
        boost::optional<int> o;
        BOOST_TEST(test("", auto_, o));
        o = 1;
        BOOST_TEST(test("1", auto_, o));

        // test alternative
        boost::variant<int, double, float, std::string> vv;
        vv = 1;
        BOOST_TEST(test("1", auto_, vv));
        vv = 1.0;
        BOOST_TEST(test("1.0", auto_, vv));
        vv = 1.0f;
        BOOST_TEST(test("1.0", auto_, vv));
        vv = "some string";
        BOOST_TEST(test("some string", auto_, vv));

        // test fusion sequence
        std::pair<int, double> p (1, 2.0);
        BOOST_TEST(test("12.0", auto_, p));
        BOOST_TEST(test_delimited("1,2.0,", auto_, p, ','));
    }

    {
        using karma::auto_;
        using karma::upper;
        using spirit_test::test;
        using spirit_test::test_delimited;

        // test primitive types
        BOOST_TEST(test("true", auto_(true)));
        BOOST_TEST(test("1", auto_(1)));
        BOOST_TEST(test("1.1", auto_(1.1)));
        BOOST_TEST(test("test", auto_("test")));
        BOOST_TEST(test(L"test", auto_(L"test")));
        BOOST_TEST(test("a", auto_('a')));
        BOOST_TEST(test(L"a", auto_(L'a')));

        BOOST_TEST(test("TRUE", upper[auto_(true)]));
        BOOST_TEST(test("TEST", upper[auto_("test")]));

        // test containers
        std::vector<int> v;
        v.push_back(0);
        v.push_back(1);
        v.push_back(2);
        BOOST_TEST(test("012", auto_(v)));
        BOOST_TEST(test_delimited("0,1,2,", auto_(v), ','));

        std::list<int> l;
        l.push_back(0);
        l.push_back(1);
        l.push_back(2);
        BOOST_TEST(test("012", auto_(l)));
        BOOST_TEST(test_delimited("0,1,2,", auto_(l), ','));

        // test optional
        boost::optional<int> o;
        BOOST_TEST(test("", auto_(o)));
        o = 1;
        BOOST_TEST(test("1", auto_(o)));

        // test alternative
        boost::variant<int, double, float, std::string> vv;
        vv = 1;
        BOOST_TEST(test("1", auto_(vv)));
        vv = 1.0;
        BOOST_TEST(test("1.0", auto_(vv)));
        vv = 1.0f;
        BOOST_TEST(test("1.0", auto_(vv)));
        vv = "some string";
        BOOST_TEST(test("some string", auto_(vv)));

        // test fusion sequence
        std::pair<int, double> p (1, 2.0);
        BOOST_TEST(test("12.0", auto_, p));
        BOOST_TEST(test_delimited("1,2.0,", auto_(p), ','));
    }

    {
        using spirit_test::test;
        using spirit_test::test_delimited;

        // test primitive types
        BOOST_TEST(test_rule("true", true));
        BOOST_TEST(test_rule("1", 1));
        BOOST_TEST(test_rule("1.1", 1.1));
        BOOST_TEST(test_rule("test", std::string("test")));

        // test containers
        std::vector<int> v;
        v.push_back(0);
        v.push_back(1);
        v.push_back(2);
        BOOST_TEST(test_rule("012", v));
        BOOST_TEST(test_rule_delimited("0,1,2,", v, ','));

        std::list<int> l;
        l.push_back(0);
        l.push_back(1);
        l.push_back(2);
        BOOST_TEST(test_rule("012", l));
        BOOST_TEST(test_rule_delimited("0,1,2,", l, ','));

        // test optional
        boost::optional<int> o;
        BOOST_TEST(test_rule("", o));
        o = 1;
        BOOST_TEST(test_rule("1", o));

        // test alternative
        boost::variant<int, double, float, std::string> vv;
        vv = 1;
        BOOST_TEST(test_rule("1", vv));
        vv = 1.0;
        BOOST_TEST(test_rule("1.0", vv));
        vv = 1.0f;
        BOOST_TEST(test_rule("1.0", vv));
        vv = "some string";
        BOOST_TEST(test_rule("some string", vv));

        // test fusion sequence
        std::pair<int, double> p (1, 2.0);
        BOOST_TEST(test_rule("12.0", p));
        BOOST_TEST(test_rule_delimited("1,2.0,", p, ','));
    }

    return boost::report_errors();
}
