/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/binary.hpp>
#include <boost/spirit/home/x3/directive/with.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/preprocessor/facilities/overload.hpp>
#include <boost/preprocessor/facilities/expand.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <iostream>
#include <utility>
#include <type_traits>

#include "test.hpp"

namespace x3 = boost::spirit::x3;


#if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
# define TEST_SUCCESS_IMPL(tester, ...) \
    BOOST_TEST_NO_THROW({ \
        BOOST_TEST(tester(__VA_ARGS__)); \
    })

# define TEST_FAILURE_IMPL_4(tester, input, parser, catch_stmt) \
    BOOST_TEST_THROWS( \
        { \
            try \
            { \
                BOOST_TEST(tester(input, parser)); \
            } \
            catch (expectation_failure<char const*> const& x) \
            { \
                catch_stmt \
                throw; \
            } \
        }, \
        expectation_failure<char const*> \
    )

# define TEST_FAILURE_IMPL_5(tester, input, parser, arg0, catch_stmt) \
    BOOST_TEST_THROWS( \
        { \
            try \
            { \
                BOOST_TEST(tester(input, parser, arg0)); \
            } \
            catch (expectation_failure<char const*> const& x) \
            { \
                catch_stmt \
                throw; \
            } \
        }, \
        expectation_failure<char const*> \
    )

# define TEST_FAILURE_IMPL_6(tester, input, parser, arg0, arg1, catch_stmt) \
    BOOST_TEST_THROWS( \
        { \
            try \
            { \
                BOOST_TEST(tester(input, parser, arg0, arg1)); \
            } \
            catch (expectation_failure<char const*> const& x) \
            { \
                catch_stmt \
                throw; \
            } \
        }, \
        expectation_failure<char const*> \
    )

#else

using expectation_failure_optional_t = x3::expectation_failure_optional<char const*>;

namespace detail
{
    template <typename T, typename Enabled = std::enable_if_t<!std::is_const_v<T>>>
    decltype(auto) wrap_with_expectation_failure(expectation_failure_optional_t&, T& attr)
    {
        return attr;
    }

    template <typename T>
    auto wrap_with_expectation_failure(expectation_failure_optional_t& xopt, T const& skipper)
    {
        return x3::with<x3::expectation_failure_tag>(xopt)[skipper];
    }
} // detail

# define TEST_SUCCESS_IMPL_3(tester, input, parser) \
    BOOST_TEST_NO_THROW({ \
        expectation_failure_optional_t xopt; \
        BOOST_TEST(tester(input, with<expectation_failure_tag>(xopt)[parser])); \
        BOOST_TEST(!xopt.has_value()); \
    })

# define TEST_SUCCESS_IMPL_4(tester, input, parser, arg0) \
    BOOST_TEST_NO_THROW({ \
        expectation_failure_optional_t xopt; \
        BOOST_TEST(tester( \
            input, \
            with<expectation_failure_tag>(xopt)[parser], \
            detail::wrap_with_expectation_failure(xopt, arg0) \
        )); \
        BOOST_TEST(!xopt.has_value()); \
    })

# define TEST_SUCCESS_IMPL_5(tester, input, parser, arg0, arg1) \
    BOOST_TEST_NO_THROW({ \
        expectation_failure_optional_t xopt; \
        BOOST_TEST(tester( \
            input, \
            with<expectation_failure_tag>(xopt)[parser], \
            detail::wrap_with_expectation_failure(xopt, arg0), \
            detail::wrap_with_expectation_failure(xopt, arg1) \
        )); \
        BOOST_TEST(!xopt.has_value()); \
    })

# define TEST_SUCCESS_IMPL(...) BOOST_PP_EXPAND(BOOST_PP_OVERLOAD(TEST_SUCCESS_IMPL_, __VA_ARGS__) (__VA_ARGS__))


# define TEST_FAILURE_IMPL_4(tester, input, parser, catch_stmt) \
    BOOST_TEST_NO_THROW( \
        { \
            expectation_failure_optional_t xopt; \
            do \
            { \
                if (!BOOST_TEST(tester(input, with<expectation_failure_tag>(xopt)[parser]))) break; \
                if (!BOOST_TEST(xopt.has_value())) break; \
                auto const& x = *xopt; \
                catch_stmt \
            } while (false); \
        } \
    )

# define TEST_FAILURE_IMPL_5(tester, input, parser, arg0, catch_stmt) \
    BOOST_TEST_NO_THROW( \
        { \
            expectation_failure_optional_t xopt; \
            do \
            { \
                if (!BOOST_TEST(tester( \
                    input, \
                    with<expectation_failure_tag>(xopt)[parser], \
                    detail::wrap_with_expectation_failure(xopt, arg0))) \
                ) break; \
                \
                if (!BOOST_TEST(xopt.has_value())) break; \
                auto const& x = *xopt; \
                catch_stmt \
            } while (false); \
        } \
    )

# define TEST_FAILURE_IMPL_6(tester, input, parser, arg0, arg1, catch_stmt) \
    BOOST_TEST_NO_THROW( \
        { \
            expectation_failure_optional_t xopt; \
            do \
            { \
                if (!BOOST_TEST(tester( \
                    input, \
                    with<expectation_failure_tag>(xopt)[parser], \
                    detail::wrap_with_expectation_failure(xopt, arg0), \
                    detail::wrap_with_expectation_failure(xopt, arg1))) \
                ) break; \
                \
                if (!BOOST_TEST(xopt.has_value())) break; \
                auto const& x = *xopt; \
                catch_stmt \
            } while (false); \
        } \
    )
#endif

#define TEST_FAILURE_IMPL(...) BOOST_PP_EXPAND(BOOST_PP_OVERLOAD(TEST_FAILURE_IMPL_, __VA_ARGS__) (__VA_ARGS__))

// Comments below are intentionally written verbosely
// to provide human-friendly intellisense tooltip for testers

// expect_throw:   parser = ok, exception = none
// expect_nothrow: parser = ok, exception = none, expectation_failure = none
#define TEST_SUCCESS_PASS(...)      TEST_SUCCESS_IMPL(test, __VA_ARGS__)

// expect_throw:   parser = ok, exception = none
// expect_nothrow: parser = ok, exception = none, expectation_failure = none
#define TEST_ATTR_SUCCESS_PASS(...) TEST_SUCCESS_IMPL(test_attr, __VA_ARGS__)

// expect_throw:   parser = fail, exception = none
// expect_nothrow: parser = fail, exception = none, expectation_failure = none
#define TEST_SUCCESS_FAIL(...)      TEST_SUCCESS_IMPL(!test, __VA_ARGS__)

// expect_throw:   parser = fail, exception = none
// expect_nothrow: parser = fail, exception = none, expectation_failure = none
#define TEST_ATTR_SUCCESS_FAIL(...) TEST_SUCCESS_IMPL(!test_attr, __VA_ARGS__)

// expect_throw:   parser = fail, exception = thrown
// expect_nothrow: parser = fail, exception = none, expectation_failure = yes
#define TEST_FAILURE(...)           TEST_FAILURE_IMPL(!test, __VA_ARGS__)

// expect_throw:   parser = fail, exception = thrown
// expect_nothrow: parser = fail, exception = none, expectation_failure = yes
#define TEST_ATTR_FAILURE(...)      TEST_FAILURE_IMPL(!test_attr, __VA_ARGS__)


// For testers; development QOL purpose only.

#define DEBUG_PRINT(x) \
    do { \
        std::cout << "----------------------------------\n"; \
        std::cout << "which: " << x.which() << "\n"; \
        std::cout << "where: " << x.where() << "\n"; \
        std::cout << "----------------------------------\n"; \
        BOOST_TEST(!"remove DEBUG_PRINT before commit!"); \
    } while (false)


#if defined(TEST_MAIN_FUNC)
# define TEST_MAIN_FUNC_IS_MAIN 0
#else
# define TEST_MAIN_FUNC_IS_MAIN 1
# define TEST_MAIN_FUNC main
#endif

int TEST_MAIN_FUNC()
{
    using namespace std::string_view_literals;

    using x3::standard::alpha;
    using x3::standard::digit;
    using x3::standard::space;
    using x3::standard::blank;
    using x3::standard::char_;
    using x3::standard::string;
    using x3::standard::lit;

    // using x3::lit;
    using x3::expect;
    using x3::lexeme;
    using x3::no_case;
    using x3::no_skip;
    using x3::omit;
    using x3::raw;
    using x3::skip;
    using x3::seek;
    using x3::repeat;
    using x3::matches;
    using x3::eps;
    using x3::eoi;
    using x3::eol;
    using x3::attr;
    using x3::dword;
    using x3::int_;
    using x3::symbols;
    using x3::confix;
    using x3::with;
    using x3::expectation_failure;
    using x3::expectation_failure_tag;

    using boost::fusion::vector;
    using boost::fusion::at_c;

    using spirit_test::test;
    using spirit_test::test_attr;
    BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(expect['x']);
    BOOST_SPIRIT_ASSERT_CONSTEXPR_CTORS(char_ > char_);

    {
        TEST_SUCCESS_PASS("aa", char_ >> expect[char_]);
        TEST_SUCCESS_PASS("aaa", char_ >> expect[char_ >> char_('a')]);
        TEST_SUCCESS_PASS("xi", char_('x') >> expect[char_('i')]);
        TEST_SUCCESS_FAIL("xi", char_('y') >> expect[char_('o')]); // should not throw!
        TEST_SUCCESS_PASS("xin", char_('x') >> expect[char_('i') >> char_('n')]);

        TEST_FAILURE("xi", char_('x') >> expect[char_('o')], {
            BOOST_TEST(x.which() == "'o'"sv);
            BOOST_TEST(x.where() == "i"sv);
        });
    }

    {
        TEST_SUCCESS_PASS("aa", char_ > char_);
        TEST_SUCCESS_PASS("aaa", char_ > char_ > char_('a'));
        TEST_SUCCESS_PASS("xi", char_('x') > char_('i'));
        TEST_SUCCESS_FAIL("xi", char_('y') > char_('o')); // should not throw!
        TEST_SUCCESS_PASS("xin", char_('x') > char_('i') > char_('n'));

        TEST_FAILURE("xi", char_('x') > char_('o'),
        {
            BOOST_TEST(x.which() == "'o'"sv);
            BOOST_TEST(x.where() == "i"sv);
        });
    }

    {
    #ifndef BOOST_SPIRIT_X3_NO_RTTI
        TEST_FAILURE("ay:a", char_ > char_('x') >> ':' > 'a',
        {
            BOOST_TEST(x.which().find("sequence") != std::string::npos);
            BOOST_TEST(x.where() == "y:a"sv);
        });
    #else
        TEST_FAILURE("ay:a", char_ > char_('x') >> ':' > 'a',
        {
            BOOST_TEST(x.which() == "undefined"sv);
            BOOST_TEST(x.where() == "y:a"sv);
        });
    #endif
    }

#if defined(BOOST_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
#endif

    // Test that attributes with > (sequences) work just like >> (sequences)
    {
        vector<char, char, char> attr;
        TEST_ATTR_SUCCESS_PASS(" a\n  b\n  c", char_ > char_ > char_, attr, space);
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'b'));
        BOOST_TEST((at_c<2>(attr) == 'c'));
    }

    {
        vector<char, char, char> attr;
        TEST_ATTR_SUCCESS_PASS(" a\n  b\n  c", char_ > char_ >> char_, attr, space);
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'b'));
        BOOST_TEST((at_c<2>(attr) == 'c'));
    }

    {
        vector<char, char, char> attr;
        TEST_ATTR_SUCCESS_PASS(" a, b, c", char_ >> ',' > char_ >> ',' > char_, attr, space);
        BOOST_TEST((at_c<0>(attr) == 'a'));
        BOOST_TEST((at_c<1>(attr) == 'b'));
        BOOST_TEST((at_c<2>(attr) == 'c'));
    }

    {
        std::string attr;
        TEST_ATTR_SUCCESS_PASS("'azaaz'", "'" > *(char_("a") | char_("z")) > "'", attr, space);
        BOOST_TEST(attr == "azaaz");
    }

#if defined(BOOST_CLANG)
#pragma clang diagnostic pop
#endif

    {
        TEST_SUCCESS_PASS(" a a", char_ > char_, space);
        TEST_SUCCESS_PASS(" x i", char_('x') > char_('i'), space);

        TEST_FAILURE(" x i", char_('x') > char_('o'), space, {
            BOOST_TEST(x.which() == "'o'"sv);
            BOOST_TEST(x.where() == "i"sv);
        });
    }

    {
        TEST_FAILURE("bar", expect[lit("foo")],
        {
            BOOST_TEST(x.which() == "\"foo\""sv);
            BOOST_TEST(x.where() == "bar"sv);
        });
    }


    // skipper
    {
        TEST_FAILURE("accb", repeat(7)[alpha], (lit('a') > 'b') | (lit('c') > 'd'), {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "ccb"sv);
        });
    }

    //
    // ********* Developers note **********
    //
    // As of now (see `git blame`), get_info<T> is still not
    // specialized for many of the X3 parsers so that the
    // value of `expectation_failure<...>::which()` will be
    // implementation-defined demangled string.
    // Therefore it's essentially impossible to test them
    // right now; further work must be done.
    //
    // Some specific situations are already been reported
    // (e.g. https://github.com/boostorg/spirit/issues/777)
    // but we really need to implement all specializations for
    // X3's predefined parsers, not just the one reported above.
    //


    // sanity check: test expectation_failure propagation
    // on custom skippers
    {
        TEST_SUCCESS_PASS("a..b", lit('a') >> 'b', lit('.') >> '.');
        TEST_SUCCESS_FAIL("a..b", lit('a') >> 'b', lit('.') >> 'z');

        TEST_SUCCESS_PASS("a  b", lit('a') >> 'b', blank);
        TEST_SUCCESS_PASS("a..b", lit('a') >> 'b', +lit('.'));
        TEST_SUCCESS_PASS("a..b", lit('a') >> 'b', lit('.') >> '.');

        // if this test fails, there might be a problem in x3::skip_over
        TEST_FAILURE     ("a..b", lit('a') >> 'b', lit('.') >> expect[lit('z')], {
            BOOST_TEST(x.which() == "'z'"sv);
            BOOST_TEST(x.where() == ".b"sv);
        });

        // ---------------------------------------------------------
        // skip(...) version of the code above
        // we must test against semantically identical cases!

        TEST_SUCCESS_PASS("a..b", skip(lit('.') >> '.')[lit('a') >> 'b']);
        TEST_SUCCESS_FAIL("a..b", skip(lit('.') >> 'z')[lit('a') >> 'b']);

        TEST_SUCCESS_PASS("a  b", skip(blank)[lit('a') >> 'b']);
        TEST_SUCCESS_PASS("a..b", skip(+lit('.'))[lit('a') >> 'b']);
        TEST_SUCCESS_PASS("a..b", skip(lit('.') >> '.')[lit('a') >> 'b']);

        // if this test fails, there might be a problem in x3::skip_over and/or x3::skip_directive
        TEST_FAILURE     ("a..b", skip(lit('.') >> expect[lit('z')])[lit('a') >> 'b'], {
            BOOST_TEST(x.which() == "'z'"sv);
            BOOST_TEST(x.where() == ".b"sv);
        });
    }

    // sanity check; test `post_skip` in `x3::phrase_parse(...)`
    {
        TEST_SUCCESS_PASS("a..b..", lit('a') >> 'b', lit('.') >> '.');
        TEST_SUCCESS_FAIL("a..b..", lit('a') >> 'z', lit('.') >> '.');
        TEST_SUCCESS_FAIL("a..b..", lit('a') >> 'b', lit('.') >> 'z');

        // should fail in `post_skip`
        TEST_SUCCESS_FAIL("a..b.z", lit('a') >> 'b', lit('.') >> '.');

        // if this test fails, x3::skip_over is BUGGED when `post_skip` is run
        TEST_FAILURE("a..b.z", lit('a') >> 'b', lit('.') > '.', {
            BOOST_TEST(x.which() == "'.'"sv);
            BOOST_TEST(x.where() == "z"sv);
        });
    }

    // sequence
    {
        TEST_SUCCESS_PASS("ab", lit('a') >> 'b');
        TEST_SUCCESS_FAIL("ac", lit('a') >> 'b');

        TEST_FAILURE("ac", lit('a') >> expect[lit('b')], {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
        TEST_FAILURE("ac", lit('a') > lit('b'), {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

    // auxilary parsers
    {
        TEST_SUCCESS_PASS("a12", lit('a') > eps > +digit);
        TEST_SUCCESS_PASS("a12", lit('a') > +digit > eoi);
        TEST_SUCCESS_PASS("a12\n", lit('a') > +digit > eol);

        TEST_FAILURE("a12", lit('a') > eps(false) > +digit, {
            BOOST_TEST(x.where() == "12"sv);
        });
        TEST_FAILURE("a12", lit('a') > eoi > +digit, {
            BOOST_TEST(x.where() == "12"sv);
        });
        TEST_FAILURE("a12\n", lit('a') > eol > +digit, {
            BOOST_TEST(x.where() == "12\n"sv);
        });

        int n = 0;
        TEST_ATTR_SUCCESS_PASS("abc", lit("abc") > attr(12) > eoi, n);
        BOOST_TEST(n == 12);
    }

    // binary, numeric, char, string parsers
    {
        TEST_SUCCESS_PASS("12abcd", +digit > dword);
        TEST_SUCCESS_PASS("abc12", +alpha > int_);
        TEST_SUCCESS_PASS("12a", +digit > lit('a'));

        TEST_FAILURE("12abc", +digit > dword, {
            BOOST_TEST(x.where() == "abc"sv);
        });
        TEST_FAILURE("abc", +alpha > int_, {
            BOOST_TEST(x.where() == ""sv);
        });
        TEST_FAILURE("12a", +digit > lit('b'), {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "a"sv);
        });

        symbols<> s;
        s.add("cat");
        TEST_SUCCESS_PASS("12cat", +digit > s);
        TEST_FAILURE("12dog", +digit > s, {
            BOOST_TEST(x.where() == "dog"sv);
        });
    }

    // confix
    {
        TEST_SUCCESS_PASS("[12cat]", confix('[', ']')[+digit > lit("cat")]);
        TEST_FAILURE("[12dog]", confix('[', ']')[+digit > lit("cat")], {
            BOOST_TEST(x.which() == "\"cat\""sv);
            BOOST_TEST(x.where() == "dog]"sv);
        });
    }

    // expect
    {
        TEST_SUCCESS_PASS("abc", lit('a') >> expect[lit('b') >> 'c']);
        TEST_FAILURE("abc", lit('a') >> expect[lit('b') >> 'd'], {
            BOOST_TEST(x.where() == "bc"sv);
        });
        TEST_FAILURE("abc", lit('a') >> expect[lit('b') > 'd'], {
            BOOST_TEST(x.which() == "'d'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

    // lexeme
    {
        TEST_SUCCESS_PASS("12 ab", int_ >> lexeme[lit('a') > 'b'], space);
        TEST_FAILURE("12 a b", int_ >> lexeme[lit('a') > 'b'], space, {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == " b"sv);
        });
    }

    // matches
    {
        TEST_SUCCESS_PASS("ab", matches[lit('a') >> 'b']);
        TEST_SUCCESS_PASS("ac", matches[lit('a') >> 'b'] >> "ac");
        TEST_SUCCESS_PASS("ab", matches[lit('a') > 'b']);
        TEST_FAILURE("ac", matches[lit('a') > 'b'] >> "ac", {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });

        bool attr = false;
        TEST_ATTR_SUCCESS_PASS("ab", matches[lit('a') > 'b'], attr);
        BOOST_TEST(attr == true);
    }

    // no_case
    {
        TEST_SUCCESS_PASS("12 aB", int_ >> no_case[lit('a') > 'b'], space);
        TEST_FAILURE("12 aB", int_ >> no_case[lit('a') > 'c'], space, {
            BOOST_TEST(x.which() == "'c'"sv);
            BOOST_TEST(x.where() == "B"sv);
        });
    }

    // no_skip
    {
        TEST_SUCCESS_PASS("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'b'], space);
        TEST_FAILURE("12 3ab", int_ >> int_ >> no_skip[lit('a') > 'c'], space, {
            BOOST_TEST(x.which() == "'c'"sv);
            BOOST_TEST(x.where() == "b"sv);
        });
    }

    // skip
    {
        TEST_SUCCESS_PASS("ab[]c[]d", skip(lit('[') > ']')[+alpha]);
        TEST_FAILURE("ab[]c[5]d", skip(lit('[') > ']')[+alpha], {
            BOOST_TEST(x.which() == "']'"sv);
            BOOST_TEST(x.where() == "5]d"sv);
        });

        TEST_SUCCESS_PASS("a1[]b2c3[]d4", skip(lit('[') > ']')[+(alpha > digit)]);
        TEST_FAILURE("a1[]b2c3[]d", skip(lit('[') > ']')[+(alpha > digit)], {
            BOOST_TEST(x.where() == ""sv);
        });

        TEST_FAILURE("a b c", lit('a') > 'c', space, {
            BOOST_TEST(x.which() == "'c'"sv);
            BOOST_TEST(x.where() == "b c"sv);
        });


        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d", skip(space)[*char_ > lit('z')], s, {
                BOOST_TEST(x.which() == "'z'"sv);
                BOOST_TEST(x.where() == ""sv);
            });
        }

        {
            std::string s;
            TEST_ATTR_SUCCESS_PASS("a b\n c\n d", char_('a') > char_('b') > skip(space)[char_('c') > char_('d')], s, blank);
            BOOST_TEST(s == "abcd");
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b\n c\n d", char_('a') > char_('z') > skip(space)[char_('c') > char_('d')], s, blank, {
                BOOST_TEST(x.which() == "'z'"sv);
                BOOST_TEST(x.where() == "b\n c\n d"sv);
            });
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b\n c\n d", char_('a') > char_('b') > skip(space)[char_('z') > char_('d')], s, blank, {
                BOOST_TEST(x.where() == "\n c\n d"sv);
            });
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b\n c\n d", char_('a') > char_('b') > skip(space)[char_('c') > char_('z')], s, blank, {
                BOOST_TEST(x.which() == "'z'"sv);
                BOOST_TEST(x.where() == "d"sv);
            });
        }

        // reskip
        {
            std::string s;
            TEST_ATTR_SUCCESS_PASS("a b c d", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > skip[char_('d')]], s, blank);
            BOOST_TEST(s == "abcd");
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > skip[char_('z')]], s, blank, {
                BOOST_TEST(x.where() == "d"sv);
            });
        }

        // reskip with expectation failure context propagation
        {
            std::string s;
            TEST_ATTR_SUCCESS_PASS("a b c d e", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > skip[char_('d') > char_('e')]], s, blank);
            BOOST_TEST(s == "abcde");
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d e", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > skip[char_('z') > char_('e')]], s, blank, {
                BOOST_TEST(x.where() == " d e"sv);
            });
        }
        {
            std::string s;
            TEST_ATTR_FAILURE("a b c d e", char_('a') > char_('b') > no_skip[lit(' ') > char_('c') > skip[char_('d') > char_('z')]], s, blank, {
                BOOST_TEST(x.which() == "'z'"sv);
                BOOST_TEST(x.where() == "e"sv);
            });
        }
    }

    // omit
    {
        TEST_SUCCESS_PASS("ab", omit[lit('a') > 'b']);
        TEST_FAILURE("ab", omit[lit('a') > 'c'], {
            BOOST_TEST(x.which() == "'c'"sv);
            BOOST_TEST(x.where() == "b"sv);
        });
    }

    // raw
    {
        TEST_SUCCESS_PASS("ab", raw[lit('a') > 'b']);
        TEST_FAILURE("ab", raw[lit('a') > 'c'], {
            BOOST_TEST(x.which() == "'c'"sv);
            BOOST_TEST(x.where() == "b"sv);
        });
    }

    // repeat
    {
        TEST_SUCCESS_PASS("ababac", repeat(1, 3)[lit('a') >> 'b'] >> "ac" | +alpha);

        TEST_FAILURE("ababac", repeat(1, 3)[lit('a') > 'b'] | +alpha, {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
        TEST_FAILURE("acab", repeat(2, 3)[lit('a') > 'b'] | +alpha, {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "cab"sv);
        });

        TEST_SUCCESS_PASS("bcab", repeat(2, 3)[lit('a') > 'b'] | +alpha);
    }

    // seek
    {
        TEST_SUCCESS_PASS("a1b1c1", seek[lit('c') > '1']);
        TEST_FAILURE("a1b1c2c1", seek[lit('c') > '1'], {
            BOOST_TEST(x.which() == "'1'"sv);
            BOOST_TEST(x.where() == "2c1"sv);
        });
    }

    // alternative
    {
        TEST_SUCCESS_PASS("ac", lit('a') >> 'b' | "ac");
        TEST_SUCCESS_PASS("ac", lit('a') >> 'b' | lit('a') >> 'd' | "ac");

        TEST_FAILURE("ac", (lit('a') > 'b') | "ac", {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
        TEST_FAILURE("ac", lit('a') >> 'b' | (lit('a') > 'd') | "ac", {
            BOOST_TEST(x.which() == "'d'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

    // predicate
    {
        TEST_SUCCESS_PASS("abc", lit('a') >> &(lit('b') > 'c') >> "bc");
        TEST_FAILURE("abc", lit('a') >> &(lit('b') > 'd') >> "bc", {
            BOOST_TEST(x.which() == "'d'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

    // difference
    {
        TEST_SUCCESS_PASS("bcac", *(char_ - (lit('a') >> 'b')));
        TEST_SUCCESS_PASS("bcab", *(char_ - (lit('a') > 'b')) >> "ab");
        TEST_FAILURE("bcac", *(char_ - (lit('a') > 'b')) >> "ab", {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

    // kleene
    {
        TEST_SUCCESS_PASS("abac", *(lit('a') >> 'b') >> "ac");
        TEST_SUCCESS_PASS("abbc", *(lit('a') > 'b') >> "bc");
        TEST_FAILURE("abac", *(lit('a') > 'b') >> "ac", {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

    // list
    {
        TEST_SUCCESS_PASS("ab::ab::ac", (lit('a') >> 'b') % (lit(':') >> ':') >> "::ac");
        TEST_SUCCESS_PASS("ab::ab:ac", (lit('a') > 'b') % (lit(':') >> ':') >> ":ac");

        TEST_FAILURE("ab::ab::ac", (lit('a') > 'b') % (lit(':') >> ':') >> "::ac", {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
        TEST_FAILURE("ab::ab:ab", (lit('a') >> 'b') % (lit(':') > ':') >> ":ab", {
            BOOST_TEST(x.which() == "':'"sv);
            BOOST_TEST(x.where() == "ab"sv);
        });
    }

    // not
    {
        TEST_SUCCESS_PASS("[ac]", lit('[') >> !(lit('a') >> 'b') >> +alpha >> ']');
        TEST_SUCCESS_PASS("[bc]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']');
        TEST_FAILURE("[ac]", lit('[') >> !(lit('a') > 'b') >> +alpha >> ']', {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c]"sv);
        });
    }

    // optional
    {
        TEST_SUCCESS_PASS("ac", -(lit('a') >> 'b') >> "ac");
        TEST_SUCCESS_PASS("ab", -(lit('a') > 'b'));
        TEST_FAILURE("ac", -(lit('a') > 'b') >> "ac", {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

    // plus
    {
        TEST_SUCCESS_PASS("abac", +(lit('a') >> 'b') >> "ac");
        TEST_SUCCESS_PASS("abbc", +(lit('a') > 'b') >> "bc");
        TEST_FAILURE("abac", +(lit('a') > 'b') >> "ac", {
            BOOST_TEST(x.which() == "'b'"sv);
            BOOST_TEST(x.where() == "c"sv);
        });
    }

#if TEST_MAIN_FUNC_IS_MAIN
    return boost::report_errors();
#else
    return 0;
#endif
}
