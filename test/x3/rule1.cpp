/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// this file deliberately contains non-ascii characters
// boostinspect:noascii

#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>

#include <string>
#include <cstring>
#include <iostream>
#include "test.hpp"
namespace x3 = boost::spirit::x3;
namespace lits
{
    using x3::lit;
    auto a = lit('a');
    auto b = lit('b');
    auto c = lit('c');
}
namespace rules
{
    using x3::lit;
    using x3::rule;
    auto a = rule<class a>()
        = lit('a');

    auto b = rule<class b>()
        = lit('b');

    auto c = rule<class c>()
        = lit('c');
}
#if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
namespace recur_lit
{
        using x3::rule;
        using namespace lits;
        rule<class r> r;

        auto r_def =
                r = (a | b) >> (r | b);

        BOOST_SPIRIT_DEFINE(r)
}//recur_lit
namespace recur_rule
{
        using x3::rule;
        using namespace rules;
        rule<class r> r;

        auto r_def =
                r = (a | b) >> (r | c);

        BOOST_SPIRIT_DEFINE(r)
}//recur_lit
#endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT

int
main()
{
    using spirit_test::test;

    using namespace x3::ascii;
    using x3::rule;
    using x3::phrase_parse;
    using x3::skip_flag;

    { // basic tests

        using namespace lits;
        rule<class r> r;

        {
            auto r_def =
                r = *(a | b | c);

            BOOST_TEST(test("abcabcacb", r_def));
        }
        auto recur_tests=[](auto start)
        {
            BOOST_TEST(test("aaaabababaaabbb", start));
            BOOST_TEST(test("aaaabababaaabba", start, false));

            // ignore the skipper!
            BOOST_TEST(test("aaaabababaaabba", start, space, false));
        };
      #if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
        {
            recur_tests(recur_lit::r_def);
        }
      #else
        {
            auto r_def =
                r = (a | b) >> (r | b);

            recur_tests(r_def);
        }
      #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
    }

    { // basic tests w/ skipper

        using namespace lits;
        rule<class r> r;

        {
            auto r_def =
                r = *(a | b | c);

            BOOST_TEST(test(" a b c a b c a c b ", r_def, space));
        }
        auto recur_tests=[](auto start)
        {
            BOOST_TEST(test(" a a a a b a b a b a a a b b b ", start, space));
            BOOST_TEST(test(" a a a a b a b a b a a a b b a ", start, space, false));
        };
      #if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
        {
            recur_tests(recur_lit::r_def);
          #if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP
            struct derived:x3::gram_base<derived>
            {
              struct r_id{};
              const rule_b<r_id> r;
              derived():r{"r_rule"}{}
              
              BOOST_SPIRIT_DER_DEFINE(r = (a | b) >> (r | b))            
            };
            derived d;
            recur_tests(d.r);
          #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_CRTP
        }
      #else
        {
            auto r_def =
                r = (a | b) >> (r | b);
            recur_tests(r_def);

        }
      #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
    }

    { // basic tests w/ skipper but no final post-skip

        using namespace rules;
        {
            auto r_def = rule<class r>() = *(a | b) >> c;

            char const *s1 = " a b a a b b a c ... "
              , *const e1 = s1 + std::strlen(s1);
            BOOST_TEST(phrase_parse(s1, e1, r_def, space, skip_flag::dont_post_skip)
              && s1 == e1 - 5);

        }

        auto recur_tests=[](auto start)
        {
            {
                char const *s1 = " a a a a b a b a b a a a b b b c "
                  , *const e1 = s1 + std::strlen(s1);
                BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::post_skip)
                  && s1 == e1);
            }
            {
                char const *s1 = " a a a a b a b a b a a a b b b c "
                  , *const e1 = s1 + std::strlen(s1);
                BOOST_TEST(phrase_parse(s1, e1, start, space, skip_flag::dont_post_skip)
                  && s1 == e1 - 1);
            }
        };
      #if BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
        {
            recur_tests(recur_rule::r_def);
        }
      #else
        {
            rule<class r> r;

            auto r_def =
                r = (a | b) >> (r | c);
            recur_tests(r_def);
        }
      #endif//BOOST_SPIRIT_X3_EXPERIMENTAL_GET_RHS_NO_CONTEXT
    }

    return boost::report_errors();
}
