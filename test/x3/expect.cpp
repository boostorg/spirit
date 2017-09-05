/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/spirit/home/x3/binary.hpp>
#include <boost/spirit/home/x3/directive/with.hpp>

#include <string>
#include <iostream>
#include "test.hpp"

int
main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::x3::ascii;
    using boost::spirit::x3::lit;
    using boost::spirit::x3::expect;
    using spirit_test::test;
    using spirit_test::test_attr;
    using boost::spirit::x3::lexeme;
    using boost::spirit::x3::no_case;
    using boost::spirit::x3::no_skip;
    using boost::spirit::x3::omit;
    using boost::spirit::x3::raw;
    using boost::spirit::x3::skip;
    using boost::spirit::x3::seek;
    using boost::spirit::x3::repeat;
    using boost::spirit::x3::matches;
    using boost::spirit::x3::eps;
    using boost::spirit::x3::eoi;
    using boost::spirit::x3::eol;
    using boost::spirit::x3::attr;
    using boost::spirit::x3::dword;
    using boost::spirit::x3::int_;
    using boost::spirit::x3::symbols;
    using boost::spirit::x3::confix;
    using boost::spirit::x3::with;

    {
        BOOST_TEST((test("aa", char_ >> expect[char_])));
        BOOST_TEST((test("aaa", char_ >> expect[char_ >> char_('a')])));
        BOOST_TEST((test("xi", char_('x') >> expect[char_('i')])));
        BOOST_TEST((!test("xi", char_('y') >> expect[char_('o')]))); // should not throw!
        BOOST_TEST((test("xin", char_('x') >> expect[char_('i') >> char_('n')])));
        BOOST_TEST((!test("xi", char_('x') >> expect[char_('o')])));
    }

    {
        BOOST_TEST((test("aa", char_ > char_)));
        BOOST_TEST((test("aaa", char_ > char_ > char_('a'))));
        BOOST_TEST((test("xi", char_('x') > char_('i'))));
        BOOST_TEST((!test("xi", char_('y') > char_('o')))); // should not throw!
        BOOST_TEST((test("xin", char_('x') > char_('i') > char_('n'))));
        BOOST_TEST((!test("xi", char_('x') > char_('o'))));
    }

    {
        BOOST_TEST((!test("ay:a", char_ > char_('x') >> ':' > 'a')));
    }

    { // Test that attributes with > (sequences) work just like >> (sequences)

        using boost::fusion::vector;
        using boost::fusion::at_c;

        {
            vector<char, char, char> attr;
            BOOST_TEST((test_attr(" a\n  b\n  c",
                char_ > char_ > char_, attr, space)));
            BOOST_TEST((at_c<0>(attr) == 'a'));
            BOOST_TEST((at_c<1>(attr) == 'b'));
            BOOST_TEST((at_c<2>(attr) == 'c'));
        }

        {
            vector<char, char, char> attr;
            BOOST_TEST((test_attr(" a\n  b\n  c",
                char_ > char_ >> char_, attr, space)));
            BOOST_TEST((at_c<0>(attr) == 'a'));
            BOOST_TEST((at_c<1>(attr) == 'b'));
            BOOST_TEST((at_c<2>(attr) == 'c'));
        }

        {
            vector<char, char, char> attr;
            BOOST_TEST((test_attr(" a, b, c",
                char_ >> ',' > char_ >> ',' > char_, attr, space)));
            BOOST_TEST((at_c<0>(attr) == 'a'));
            BOOST_TEST((at_c<1>(attr) == 'b'));
            BOOST_TEST((at_c<2>(attr) == 'c'));
        }
    }

    {
        BOOST_TEST((test(" a a", char_ > char_, space)));
        BOOST_TEST((test(" x i", char_('x') > char_('i'), space)));
        BOOST_TEST((!test(" x i", char_('x') > char_('o'), space)));
    }

    {
        BOOST_TEST((!test("bar", expect[lit("foo")])));
    }

    { // Test expect in skipper
        BOOST_TEST((test("accbeabfcdg", repeat(7)[alpha], lit('a') > 'b' | lit('c') > 'd')));
        std::string attr;
        BOOST_TEST((test_attr("accbeabfcdg", repeat(7)[alpha], attr, lit('a') > 'b' | lit('c') > 'd')));
        BOOST_TEST((attr == "accbefg"));
    }

    { // Test expect in auxilary parsers
        BOOST_TEST((test("a12", lit('a') > eps > +digit)));
        BOOST_TEST((!test("a12", lit('a') > eps(false) > +digit)));
        BOOST_TEST((test("a12", lit('a') > +digit > eoi)));
        BOOST_TEST((!test("a12", lit('a') > eoi > +digit)));
        BOOST_TEST((test("a12\n", lit('a') > +digit > eol)));
        BOOST_TEST((!test("a12\n", lit('a') > eol > +digit)));
        int n = 0;
        BOOST_TEST((test_attr("abc", lit("abc") > attr(12) > eoi, n)));
        BOOST_TEST((12 == n));
    }

    { // Test expect in binary, numeric, char, string parsers
        BOOST_TEST((test("12abcd", +digit > dword)));
        BOOST_TEST((!test("12abc", +digit > dword)));
        BOOST_TEST((test("abc12", +alpha > int_)));
        BOOST_TEST((!test("abc", +alpha > int_)));
        BOOST_TEST((test("12a", +digit > lit('a'))));
        BOOST_TEST((!test("12a", +digit > lit('b'))));
        symbols<> s;
        s.add("cat");
        BOOST_TEST((test("12cat", +digit > s)));
        BOOST_TEST((!test("12dog", +digit > s)));
    }

    { // Test expect in confix
        BOOST_TEST((test("[12cat]", confix('[', ']')[+digit > lit("cat")])));
        BOOST_TEST((!test("[12dog]", confix('[', ']')[+digit > lit("cat")])));
    }

    { // Test expect in expect
        BOOST_TEST((test("abc", lit('a') >> expect[lit('b') >> 'c'])));
        BOOST_TEST((!test("abc", lit('a') >> expect[lit('b') >> 'd'])));
        BOOST_TEST((!test("abc", lit('a') >> expect[lit('b') > 'd'])));
    }

    { // Test expect in lexeme
        BOOST_TEST((test("12 ab", int_ >> lexeme[lit('a') > 'b'], space)));
        BOOST_TEST((!test("12 a b", int_ >> lexeme[lit('a') > 'b'], space)));
    }

    { // Test expect in matches
        BOOST_TEST((test("ab", matches[lit('a') >> 'b'])));
        BOOST_TEST((test("ac", matches[lit('a') >> 'b'] >> "ac")));
        BOOST_TEST((test("ab", matches[lit('a') > 'b'])));
        BOOST_TEST((!test("ac", matches[lit('a') > 'b'] >> "ac")));
        bool attr = false;
        BOOST_TEST((test_attr("ab", matches[lit('a') > 'b'], attr)));
        BOOST_TEST((true == attr));
    }

    { // Test expect in no_case
        BOOST_TEST((test("12 aB", int_ >> no_case[lit('a') > 'b'], space)));
        BOOST_TEST((!test("12 aB", int_ >> no_case[lit('a') > 'c'], space)));
    }

    { // Test expect in no_skip
        BOOST_TEST((test("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'b'], space)));
        BOOST_TEST((!test("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'c'], space)));
    }

    { // Test expect in omit
        BOOST_TEST((test("ab", omit[lit('a') > 'b'])));
        BOOST_TEST((!test("ab", omit[lit('a') > 'c'])));
    }

    { // Test expect in raw
        BOOST_TEST((test("ab", raw[lit('a') > 'b'])));
        BOOST_TEST((!test("ab", raw[lit('a') > 'c'])));
    }

    { // Test expect in repeat
        BOOST_TEST((test("ababac", repeat(1, 3)[lit('a') >> 'b'] >> "ac" | +alpha)));
        BOOST_TEST((!test("ababac", repeat(1, 3)[lit('a') > 'b'] | +alpha)));
        BOOST_TEST((!test("acab", repeat(2, 3)[lit('a') > 'b'] | +alpha)));
        BOOST_TEST((test("bcab", repeat(2, 3)[lit('a') > 'b'] | +alpha)));
    }

    { // Test expect in seek
        BOOST_TEST((test("a1b1c1", seek[lit('c') > '1'])));
        BOOST_TEST((!test("a1b1c2c1", seek[lit('c') > '1'])));
    }

    { // Test expect in skip
        BOOST_TEST((test("ab[]c[]d", skip(lit('[') > ']')[+alpha])));
        BOOST_TEST((!test("ab[]c[5]d", skip(lit('[') > ']')[+alpha])));
        BOOST_TEST((test("a1[]b2c3[]d4", skip(lit('[') > ']')[+(alpha > digit)])));
        BOOST_TEST((!test("a1[]b2c3[]d", skip(lit('[') > ']')[+(alpha > digit)])));
    }

    { // Test expect in alternative
        BOOST_TEST((test("ac", lit('a') >> 'b' | "ac")));
        BOOST_TEST((!test("ac", lit('a') > 'b' | "ac")));
        BOOST_TEST((test("ac", lit('a') >> 'b' | lit('a') >> 'd' | "ac")));
        BOOST_TEST((!test("ac", lit('a') >> 'b' | lit('a') > 'd' | "ac")));
    }

    { // Test expect in and predicate
        BOOST_TEST((test("abc", lit('a') >> &(lit('b') > 'c') >> "bc")));
        BOOST_TEST((!test("abc", lit('a') >> &(lit('b') > 'd') >> "bc")));
    }

    { // Test expect in difference
        BOOST_TEST((test("bcac", *(char_ - (lit('a') >> 'b')))));
        BOOST_TEST((test("bcab", *(char_ - (lit('a') > 'b')) >> "ab")));
        BOOST_TEST((!test("bcac", *(char_ - (lit('a') > 'b')) >> "ab")));
    }

    { // Test expect in kleene
        BOOST_TEST((test("abac", *(lit('a') >> 'b') >> "ac")));
        BOOST_TEST((!test("abac", *(lit('a') > 'b') >> "ac")));
        BOOST_TEST((test("abbc", *(lit('a') > 'b') >> "bc")));
    }

    { // Test expect in list
        BOOST_TEST((test("ab::ab::ac", (lit('a') >> 'b') % (lit(':') >> ':') >> "::ac")));
        BOOST_TEST((!test("ab::ab::ac", (lit('a') > 'b') % (lit(':') >> ':') >> "::ac")));
        BOOST_TEST((test("ab::ab:ac", (lit('a') > 'b') % (lit(':') >> ':') >> ":ac")));
        BOOST_TEST((!test("ab::ab:ab", (lit('a') >> 'b') % (lit(':') > ':') >> ":ab")));
    }

    { // Test expect in not predicate
        BOOST_TEST((test("[ac]", lit('[') >> !(lit('a') >> 'b') >> +alpha >> ']')));
        BOOST_TEST((test("[bc]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']')));
        BOOST_TEST((!test("[ac]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']')));
    }

    { // Test expect in optional
        BOOST_TEST((test("ac", -(lit('a') >> 'b') >> "ac")));
        BOOST_TEST((test("ab", -(lit('a') > 'b'))));
        BOOST_TEST((!test("ac", -(lit('a') > 'b') >> "ac")));
    }

    { // Test expect in plus
        BOOST_TEST((test("abac", +(lit('a') >> 'b') >> "ac")));
        BOOST_TEST((test("abbc", +(lit('a') > 'b') >> "bc")));
        BOOST_TEST((!test("abac", +(lit('a') > 'b') >> "ac")));
    }

    { // Test fast expect (with bool injection)
        using boost::spirit::x3::expectation_failure_tag;
        BOOST_TEST((test("ade", with<expectation_failure_tag>(false)[lit('a') >> (lit('b') >> 'c' | lit('d') >> 'e')])));
        BOOST_TEST((test("abc", with<expectation_failure_tag>(false)[lit('a') >> (lit('b') > 'c' | lit('d') > 'e')])));
        BOOST_TEST((test("ade", with<expectation_failure_tag>(false)[lit('a') >> (lit('b') > 'c' | lit('d') > 'e')])));
        BOOST_TEST((!test("abd", with<expectation_failure_tag>(false)[lit('a') >> (lit('b') > 'c' | lit('d') > 'e')])));
        BOOST_TEST((!test("adc", with<expectation_failure_tag>(false)[lit('a') >> (lit('b') > 'c' | lit('d') > 'e')])));
    }

    return boost::report_errors();
}
