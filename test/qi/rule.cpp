/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// this file deliberately contains non-ascii characters
// boostinspect:noascii

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_auxiliary.hpp>
#include <boost/spirit/include/qi_directive.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <string>
#include <cstring>
#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test_attr;
    using spirit_test::test;

    using namespace boost::spirit::ascii;
    using namespace boost::spirit::qi::labels;
    using boost::spirit::qi::locals;
    using boost::spirit::qi::rule;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::fail;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::debug;
    using boost::spirit::qi::lit;

    namespace phx = boost::phoenix;


    { // basic tests

        rule<char const*> a, b, c, start;

        a = 'a';
        b = 'b';
        c = 'c';

        a.name("a");
        b.name("b");
        c.name("c");
        start.name("start");

        debug(a);
        debug(b);
        debug(c);
        debug(start);

        start = *(a | b | c);
        BOOST_TEST(test("abcabcacb", start));

        start = (a | b) >> (start | b);
        BOOST_TEST(test("aaaabababaaabbb", start));
        BOOST_TEST(test("aaaabababaaabba", start, false));

        // ignore the skipper!
        BOOST_TEST(test("aaaabababaaabba", start, space, false));
    }

    { // basic tests with direct initialization

        rule<char const*> a ('a');
        rule<char const*> b ('b');
        rule<char const*> c ('c');
        rule<char const*> start = (a | b) >> (start | b);

        BOOST_TEST(test("aaaabababaaabbb", start));
        BOOST_TEST(test("aaaabababaaabba", start, false));

        // ignore the skipper!
        BOOST_TEST(test("aaaabababaaabba", start, space, false));
    }

    { // basic tests w/ skipper
        rule<char const*, space_type> a, b, c, start;

        a = 'a';
        b = 'b';
        c = 'c';

        a.name("a");
        b.name("b");
        c.name("c");
        start.name("start");

        debug(a);
        debug(b);
        debug(c);
        debug(start);

        start = *(a | b | c);
        BOOST_TEST(test(" a b c a b c a c b ", start, space));

        start = (a | b) >> (start | b);
        BOOST_TEST(test(" a a a a b a b a b a a a b b b ", start, space));
        BOOST_TEST(test(" a a a a b a b a b a a a b b a ", start, space, false));
    }

    { // basic tests w/ skipper but no final post-skip

        rule<char const*, space_type> a, b, c, start;

        a = 'a';
        b = 'b';
        c = 'c';

        a.name("a");
        b.name("b");
        c.name("c");
        start.name("start");

        debug(a);
        debug(b);
        debug(c);
        debug(start);

        start = *(a | b) >> c;

        using boost::spirit::qi::phrase_parse;
        using boost::spirit::qi::skip_flag;
        {
            char const *s1 = " a b a a b b a c ... "
              , *const e1 = s1 + std::strlen(s1);
            BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::dont_postskip)
              && s1 == e1 - 5);
        }

        start = (a | b) >> (start | c);
        {
            char const *s1 = " a a a a b a b a b a a a b b b c "
              , *const e1 = s1 + std::strlen(s1);
            BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::postskip)
              && s1 == e1);
        }
        {
            char const *s1 = " a a a a b a b a b a a a b b b c "
              , *const e1 = s1 + std::strlen(s1);
            BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::dont_postskip)
              && s1 == e1 - 1);
        }
    }

    { // test unassigned rule

        rule<char const*> a;
        BOOST_TEST(!test("x", a));
    }

    { // alias tests

        rule<char const*> a, b, c, d, start;

        a = 'a';
        b = 'b';
        c = 'c';
        d = start.alias(); // d will always track start

        start = *(a | b | c);
        BOOST_TEST(test("abcabcacb", d));

        start = (a | b) >> (start | b);
        BOOST_TEST(test("aaaabababaaabbb", d));
    }

    { // copy tests

        rule<char const*> a, b, c, start;

        a = 'a';
        b = 'b';
        c = 'c';

        // The FF is the dynamic equivalent of start = *(a | b | c);
        start = a;
        start = start.copy() | b;
        start = start.copy() | c;
        start = *(start.copy());

        BOOST_TEST(test("abcabcacb", start));

        // The FF is the dynamic equivalent of start = (a | b) >> (start | b);
        start = b;
        start = a | start.copy();
        start = start.copy() >> (start | b);

        BOOST_TEST(test("aaaabababaaabbb", start));
        BOOST_TEST(test("aaaabababaaabba", start, false));
    }

    { // context tests

        char ch;
        rule<char const*, char()> a;
        a = alpha[_val = _1];

        BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        BOOST_TEST(ch == 'x');

        BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        BOOST_TEST(ch == 'z');
    }

    { // auto rules tests

        char ch = '\0';
        rule<char const*, char()> a;
        a %= alpha;

        BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        BOOST_TEST(ch == 'x');
        ch = '\0';
        BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        BOOST_TEST(ch == 'z');

        a = alpha;    // test deduced auto rule behavior
        ch = '\0';
        BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        BOOST_TEST(ch == 'x');
        ch = '\0';
        BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        BOOST_TEST(ch == 'z');
    }

    { // auto rules tests: allow stl containers as attributes to
      // sequences (in cases where attributes of the elements
      // are convertible to the value_type of the container or if
      // the element itself is an stl container with value_type
      // that is convertible to the value_type of the attribute).

        std::string s;
        rule<char const*, std::string()> r;
        r %= char_ >> *(',' >> char_);

        BOOST_TEST(test("a,b,c,d,e,f", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");

        r = char_ >> *(',' >> char_);    // test deduced auto rule behavior
        s.clear();
        BOOST_TEST(test("a,b,c,d,e,f", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");

        r %= char_ >> char_ >> char_ >> char_ >> char_ >> char_;
        s.clear();
        BOOST_TEST(test("abcdef", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");

        r = char_ >> char_ >> char_ >> char_ >> char_ >> char_;
        s.clear();
        BOOST_TEST(test("abcdef", r[phx::ref(s) = _1]));
        BOOST_TEST(s == "abcdef");
    }

    { // synth attribute value-init

        std::string s;
        rule<char const*, char()> r;
        r = alpha[_val += _1];
        BOOST_TEST(test_attr("abcdef", +r, s));
        BOOST_TEST(s == "abcdef");
    }

    { // auto rules aliasing tests

        char ch = '\0';
        rule<char const*, char()> a, b;
        a %= b;
        b %= alpha;

        BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        BOOST_TEST(ch == 'x');
        ch = '\0';
        BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        BOOST_TEST(ch == 'z');

        a = b;            // test deduced auto rule behavior
        b = alpha;

        ch = '\0';
        BOOST_TEST(test("x", a[phx::ref(ch) = _1]));
        BOOST_TEST(ch == 'x');
        ch = '\0';
        BOOST_TEST(test_attr("z", a, ch)); // attribute is given.
        BOOST_TEST(ch == 'z');
    }

    { // context (w/arg) tests

        char ch;
        rule<char const*, char(int)> a; // 1 arg
        a = alpha[_val = _1 + _r1];

        BOOST_TEST(test("x", a(phx::val(1))[phx::ref(ch) = _1]));
        BOOST_TEST(ch == 'x' + 1);

        BOOST_TEST(test_attr("a", a(1), ch)); // allow scalars as rule args too.
        BOOST_TEST(ch == 'a' + 1);

        rule<char const*, char(int, int)> b; // 2 args
        b = alpha[_val = _1 + _r1 + _r2];
        BOOST_TEST(test_attr("a", b(1, 2), ch));
        BOOST_TEST(ch == 'a' + 1 + 2);
    }

    { // context (w/ reference arg) tests

        char ch;
        rule<char const*, void(char&)> a; // 1 arg (reference)
        a = alpha[_r1 = _1];

        BOOST_TEST(test("x", a(phx::ref(ch))));
        BOOST_TEST(ch == 'x');
    }

    { // context (w/locals) tests

        rule<char const*, locals<char> > a; // 1 local
        a = alpha[_a = _1] >> char_(_a);
        BOOST_TEST(test("aa", a));
        BOOST_TEST(!test("ax", a));
    }

    { // context (w/args and locals) tests

        rule<char const*, void(int), locals<char> > a; // 1 arg + 1 local
        a = alpha[_a = _1 + _r1] >> char_(_a);
        BOOST_TEST(test("ab", a(phx::val(1))));
        BOOST_TEST(test("xy", a(phx::val(1))));
        BOOST_TEST(!test("ax", a(phx::val(1))));
    }

    { // void() has unused type (void == unused_type)

        std::pair<int, char> attr;
        rule<char const*, void()> r;
        r = char_;
        BOOST_TEST(test_attr("123ax", int_ >> char_ >> r, attr));
        BOOST_TEST(attr.first == 123);
        BOOST_TEST(attr.second == 'a');
    }

    { // bug: test that injected attributes are ok

        rule<char const*, char(int) > r;

        // problem code:
        r = char_(_r1)[_val = _1];
    }

    { // show that ra = rb and ra %= rb works as expected
        rule<char const*, int() > ra, rb;
        int attr;

        ra %= int_;
        BOOST_TEST(test_attr("123", ra, attr));
        BOOST_TEST(attr == 123);

        rb %= ra;
        BOOST_TEST(test_attr("123", rb, attr));
        BOOST_TEST(attr == 123);

        rb = ra;
        BOOST_TEST(test_attr("123", rb, attr));
        BOOST_TEST(attr == 123);
    }

    { // std::string as container attribute with auto rules

        rule<char const*, std::string()> text;
        text %= +(!char_(')') >> !char_('>') >> char_);
        std::string attr;
        BOOST_TEST(test_attr("x", text, attr));
        BOOST_TEST(attr == "x");

        // test deduced auto rule behavior
        text = +(!char_(')') >> !char_('>') >> char_);
        attr.clear();
        BOOST_TEST(test_attr("x", text, attr));
        BOOST_TEST(attr == "x");
    }

    { // error handling

        using namespace boost::spirit::ascii;
        using boost::phoenix::construct;
        using boost::phoenix::bind;

        rule<char const*> r;
        r = '(' > int_ > ',' > int_ > ')';

        on_error<fail>
        (
            r, std::cout
                << phx::val("Error! Expecting: ")
                << _4
                << phx::val(", got: \"")
                << construct<std::string>(_3, _2)
                << phx::val("\"")
                << std::endl
        );

        BOOST_TEST(test("(123,456)", r));
        BOOST_TEST(!test("(abc,def)", r));
        BOOST_TEST(!test("(123,456]", r));
        BOOST_TEST(!test("(123;456)", r));
        BOOST_TEST(!test("[123,456]", r));
    }

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1310))
#pragma setlocale("french")
#endif
    { // specifying the encoding

        typedef boost::spirit::char_encoding::iso8859_1 iso8859_1;
        rule<char const*, iso8859_1> r;

        r = no_case['·'];
        BOOST_TEST(test("¡", r));
        r = no_case[char_('·')];
        BOOST_TEST(test("¡", r));

        r = no_case[char_("Â-Ô")];
        BOOST_TEST(test("…", r));
        BOOST_TEST(!test("ˇ", r));

        r = no_case["·¡"];
        BOOST_TEST(test("¡·", r));
        r = no_case[lit("·¡")];
        BOOST_TEST(test("¡·", r));
    }

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1310))
#pragma setlocale("")
#endif

    return boost::report_errors();
}

