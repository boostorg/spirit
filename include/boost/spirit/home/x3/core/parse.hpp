/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_PARSE_APRIL_16_2006_0442PM)
#define BOOST_SPIRIT_X3_PARSE_APRIL_16_2006_0442PM

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/parse_result.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>

#include <optional>
#include <iterator>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, typename Attribute>
    [[nodiscard]] constexpr bool
    parse_main(
        It& first,
        Se last,
        Parser&& p,
        Attribute& attr
    )
    {
        static_assert(Parsable<as_parser_plain_t<Parser>, It, Se, unused_type, unused_type, Attribute>);
        return as_parser(std::forward<Parser>(p)).parse(first, last, unused, unused, attr);
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, typename Attribute>
    [[nodiscard]] constexpr bool
    parse(
        It& first,
        Se last,
        Parser&& p,
        Attribute& attr
    )
    {
        return x3::parse_main(first, last, std::forward<Parser>(p), attr);
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, typename Attribute>
    [[nodiscard]] constexpr bool
    parse(
        It const& first_,
        Se last,
        Parser&& p,
        Attribute& attr
    )
    {
        It first = first_;
        return x3::parse_main(first, last, std::forward<Parser>(p), attr);
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser>
    [[nodiscard]] constexpr bool
    parse(
        It& first,
        Se last,
        Parser&& p
    )
    {
        return x3::parse_main(first, last, std::forward<Parser>(p), unused);
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser>
    [[nodiscard]] constexpr bool
    parse(
        It const& first_,
        Se last,
        Parser&& p
    )
    {
        It first = first_;
        return x3::parse_main(first, last, std::forward<Parser>(p), unused);
    }

    ///////////////////////////////////////////////////////////////////////////

    template <std::ranges::forward_range R, X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser, typename Attribute>
    [[nodiscard]] constexpr parse_result_for<R>
    parse_main(
        R& range,
        Parser&& p,
        Attribute& attr
    )
    {
        using It = std::ranges::iterator_t<R>;
        using Se = std::ranges::sentinel_t<R>;

        std::optional<x3::expectation_failure<It>> expect_failure;
        auto failure_ctx = x3::make_context<expectation_failure_tag>(expect_failure);
        using Context = decltype(failure_ctx);

        static_assert(Parsable<as_parser_plain_t<Parser>, It, Se, Context, unused_type, Attribute>);

        It first = std::ranges::begin(range);
        Se last = std::ranges::end(range);
        bool const ok = as_parser(std::forward<Parser>(p)).parse(first, last, failure_ctx, unused, attr);
        return parse_result_for<R>{
            .ok = ok,
            .expect_failure = std::move(expect_failure),
            .remainder = {std::move(first), std::move(last)}
        };
    }

    template <std::ranges::forward_range R, X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser, typename Attribute>
    constexpr void
    parse_main(
        parse_result_for<R>& res
      , R& range
      , Parser&& p
      , Attribute& attr)
    {
        using It = std::ranges::iterator_t<R>;
        using Se = std::ranges::sentinel_t<R>;

        res.expect_failure.reset();
        auto failure_ctx = x3::make_context<expectation_failure_tag>(res.expect_failure);
        using Context = decltype(failure_ctx);

        static_assert(Parsable<as_parser_plain_t<Parser>, It, Se, Context, unused_type, Attribute>);

        It first = std::ranges::begin(range);
        Se last = std::ranges::end(range);
        res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, failure_ctx, unused, attr);
        res.remainder = {std::move(first), std::move(last)};
    }

    template <std::ranges::forward_range R, X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser, typename Attribute>
    [[nodiscard]] constexpr parse_result_for<R>
    parse(
        R& range
      , Parser&& p
      , Attribute& attr)
    {
        return x3::parse_main(range, std::forward<Parser>(p), attr);
    }

    template <std::ranges::forward_range R, X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser>
    [[nodiscard]] constexpr parse_result_for<R>
    parse(
        R& range
      , Parser&& p)
    {
        return x3::parse_main(range, std::forward<Parser>(p), unused);
    }

    template <std::ranges::forward_range R, X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser, typename Attribute>
    constexpr void
    parse(
        parse_result_for<R>& res,
        R& range,
        Parser&& p,
        Attribute& attr
    )
    {
        return x3::parse_main(res, range, std::forward<Parser>(p), attr);
    }

    template <std::ranges::forward_range R, X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser>
    constexpr void
    parse(
        parse_result_for<R>& res,
        R& range,
        Parser&& p
    )
    {
        return x3::parse_main(res, range, std::forward<Parser>(p), unused);
    }

    ///////////////////////////////////////////////////////////////////////////
    enum class skip_flag
    {
        post_skip,      // force post-skipping in phrase_parse()
        dont_post_skip  // inhibit post-skipping in phrase_parse()
    };

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, X3Parser<It, Se> Skipper, typename Attribute>
    [[nodiscard]] constexpr bool
    phrase_parse_main(
        It& first,
        Se last,
        Parser&& p,
        Skipper&& s,
        Attribute& attr,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        auto skipper_ctx = x3::make_context<skipper_tag>(as_parser(std::forward<Skipper>(s)));
        using Context = decltype(skipper_ctx);
        static_assert(Parsable<as_parser_plain_t<Parser>, It, Se, Context, unused_type, Attribute>);

        bool const ok = as_parser(std::forward<Parser>(p)).parse(first, last, skipper_ctx, unused, attr);
        if (post_skip == skip_flag::post_skip)
        {
            x3::skip_over(first, last, skipper_ctx);
        }
        return ok;
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, X3Parser<It, Se> Skipper, typename Attribute>
    [[nodiscard]] constexpr bool
    phrase_parse(
        It& first,
        Se last,
        Parser&& p,
        Skipper&& s,
        Attribute& attr,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        return x3::phrase_parse_main(first, last, std::forward<Parser>(p), std::forward<Skipper>(s), attr, post_skip);
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, X3Parser<It, Se> Skipper, typename Attribute>
    [[nodiscard]] constexpr bool
    phrase_parse(
        It const& first_,
        Se last,
        Parser&& p,
        Skipper&& s,
        Attribute& attr,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        It first = first_;
        return x3::phrase_parse_main(first, last, std::forward<Parser>(p), std::forward<Skipper>(s), attr, post_skip);
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, X3Parser<It, Se> Skipper>
    [[nodiscard]] constexpr bool
    phrase_parse(
        It& first,
        Se last,
        Parser&& p,
        Skipper&& s,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        return x3::phrase_parse_main(first, last, std::forward<Parser>(p), std::forward<Skipper>(s), unused, post_skip);
    }

    template <std::forward_iterator It, std::sentinel_for<It> Se, X3Parser<It, Se> Parser, X3Parser<It, Se> Skipper>
    [[nodiscard]] constexpr bool
    phrase_parse(
        It const& first_,
        Se last,
        Parser&& p,
        Skipper&& s,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        It first = first_;
        return x3::phrase_parse_main(first, last, std::forward<Parser>(p), std::forward<Skipper>(s), unused, post_skip);
    }

    ///////////////////////////////////////////////////////////////////////////

    template <
        std::ranges::forward_range R,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Skipper,
        typename Attribute
    >
    [[nodiscard]] constexpr parse_result_for<R>
    phrase_parse_main(
        R& range,
        Parser&& p,
        Skipper&& s,
        Attribute& attr,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        using It = std::ranges::iterator_t<R>;
        using Se = std::ranges::sentinel_t<R>;

        auto skipper_ctx = x3::make_context<skipper_tag>(as_parser(std::forward<Skipper>(s)));

        std::optional<x3::expectation_failure<It>> expect_failure;
        auto ctx = x3::make_context<expectation_failure_tag>(expect_failure, skipper_ctx);

        using Context = decltype(ctx);
        static_assert(Parsable<as_parser_plain_t<Parser>, It, Se, Context, unused_type, Attribute>);

        It first = std::ranges::begin(range);
        Se last = std::ranges::end(range);
        bool ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, unused, attr);
        if (post_skip == skip_flag::post_skip)
        {
            x3::skip_over(first, last, ctx);
            if (expect_failure) ok = false;
        }
        return parse_result_for<R>{
            .ok = ok,
            .expect_failure = std::move(expect_failure),
            .remainder = {std::move(first), std::move(last)}
        };
    }

    template <
        std::ranges::forward_range R,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Skipper,
        typename Attribute
    >
    constexpr void
    phrase_parse_main(
        parse_result_for<R>& res,
        R& range,
        Parser&& p,
        Skipper&& s,
        Attribute& attr,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        using It = std::ranges::iterator_t<R>;
        using Se = std::ranges::sentinel_t<R>;

        auto skipper_ctx = x3::make_context<skipper_tag>(as_parser(std::forward<Skipper>(s)));

        res.expect_failure.reset();
        auto ctx = x3::make_context<expectation_failure_tag>(res.expect_failure, skipper_ctx);

        using Context = decltype(ctx);
        static_assert(Parsable<as_parser_plain_t<Parser>, It, Se, Context, unused_type, Attribute>);

        It first = std::ranges::begin(range);
        Se last = std::ranges::end(range);
        res.ok = as_parser(std::forward<Parser>(p)).parse(first, last, ctx, unused, attr);
        if (post_skip == skip_flag::post_skip)
        {
            x3::skip_over(first, last, ctx);
            if (res.expect_failure) res.ok = false;
        }
        res.remainder = {std::move(first), std::move(last)};
    }

    template <
        std::ranges::forward_range R,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Skipper,
        typename Attribute
    >
    [[nodiscard]] constexpr parse_result_for<R>
    phrase_parse(
        R& range,
        Parser&& p,
        Skipper&& s,
        Attribute& attr,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        return x3::phrase_parse_main(range, std::forward<Parser>(p), std::forward<Skipper>(s), attr, post_skip);
    }

    template <
        std::ranges::forward_range R,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Skipper,
        typename Attribute
    >
    [[nodiscard]] constexpr parse_result_for<R>
    phrase_parse(
        R& range,
        Parser&& p,
        Skipper&& s,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        return x3::phrase_parse_main(range, std::forward<Parser>(p), std::forward<Skipper>(s), unused, post_skip);
    }

    template <
        std::ranges::forward_range R,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Skipper,
        typename Attribute
    >
    constexpr void
    phrase_parse(
        parse_result_for<R>& res,
        R& range,
        Parser&& p,
        Skipper&& s,
        Attribute& attr,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        return x3::phrase_parse_main(res, range, std::forward<Parser>(p), std::forward<Skipper>(s), attr, post_skip);
    }

    template <
        std::ranges::forward_range R,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Parser,
        X3Parser<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>> Skipper,
        typename Attribute
    >
    constexpr void
    phrase_parse(
        parse_result_for<R>& res,
        R& range,
        Parser&& p,
        Skipper&& s,
        skip_flag post_skip = skip_flag::post_skip
    )
    {
        return x3::phrase_parse_main(res, range, std::forward<Parser>(p), std::forward<Skipper>(s), unused, post_skip);
    }


    template <typename Skipper, typename ItOrRange, typename Se = void>
    struct phrase_parse_context;

    template <typename Skipper, typename ItOrRange, typename Se = void>
    using phrase_parse_context_t = typename phrase_parse_context<Skipper, ItOrRange, Se>::type;

    template <typename Skipper, std::forward_iterator It, class Se>
    struct phrase_parse_context<Skipper, It, Se>
    {
        using iterator_t = It;
        using sentinel_t = std::conditional_t<std::is_void_v<Se>, It, Se>;
        static_assert(X3Parser<Skipper, It, sentinel_t>);

        using skipper_ctx_type = decltype(x3::make_context<skipper_tag>(as_parser(std::declval<Skipper const&>())));

#if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
        using type = skipper_ctx_type;
#else
        using type = decltype(x3::make_context<expectation_failure_tag>(
            std::declval<std::optional<expectation_failure<It>>&>(),
            std::declval<skipper_ctx_type const&>()
        ));
#endif
    };

    template <typename Skipper, std::ranges::forward_range R>
    struct phrase_parse_context<Skipper, R>
    {
        using iterator_t = std::ranges::iterator_t<R>;
        using sentinel_t = std::ranges::sentinel_t<R>;
        static_assert(X3Parser<Skipper, iterator_t, sentinel_t>);

        using skipper_ctx_type = decltype(x3::make_context<skipper_tag>(as_parser(std::declval<Skipper const&>())));

        using type = decltype(x3::make_context<expectation_failure_tag>(
            std::declval<std::optional<expectation_failure<iterator_t>>&>(),
            std::declval<skipper_ctx_type const&>()
        ));
    };


    template <typename ItOrRange, typename Se = void>
    struct parse_context;

    template <typename ItOrRange, typename Se = void>
    using parse_context_t = typename parse_context<ItOrRange, Se>::type;

    template <std::forward_iterator It>
    struct parse_context<It>
    {
#if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
        using type = unused_type;
#else
        using type = decltype(x3::make_context<expectation_failure_tag>(
            std::declval<std::optional<expectation_failure<It>>&>()
        ));
#endif
    };

    template <std::ranges::forward_range R>
    struct parse_context<R>
    {
        using type = decltype(x3::make_context<expectation_failure_tag>(
            std::declval<std::optional<expectation_failure<std::ranges::iterator_t<R>>>&>()
        ));
    };

} // boost::spirit::x3

#endif
