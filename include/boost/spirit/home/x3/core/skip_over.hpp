/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SKIP_APRIL_16_2006_0625PM)
#define BOOST_SPIRIT_X3_SKIP_APRIL_16_2006_0625PM

#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_category.hpp>

#include <boost/spirit/home/x3/core/parser.hpp>

#include <iterator>
#include <type_traits>

namespace boost::spirit::x3
{
    // Tag used to find the skipper from the context
    struct skipper_tag;

    // Move the /first/ iterator to the first non-matching position
    // given a skip-parser. The function is a no-op if unused_type or
    // unused_skipper is passed as the skip-parser.
    template <X3Subject Skipper>
    struct [[nodiscard]] unused_skipper
    {
        constexpr unused_skipper(Skipper const& skipper) noexcept
            : skipper(skipper)
        {}

        Skipper const& skipper;
    };

    template <typename Context>
    using unused_skipper_t = unused_skipper<std::remove_cvref_t<decltype(x3::get<skipper_tag>(std::declval<Context>()))>>;

    namespace detail
    {
        template <typename Skipper>
        struct is_unused_skipper : std::false_type
        {
            static_assert(X3Subject<Skipper>);
        };

        template <typename Skipper>
        struct is_unused_skipper<unused_skipper<Skipper>> : std::true_type {};

        template <>
        struct is_unused_skipper<unused_type> : std::true_type {};

        template <typename Skipper>
        [[nodiscard]] constexpr Skipper const&
        get_unused_skipper(Skipper const& skipper) noexcept
        {
            static_assert(X3Subject<Skipper>);
            return skipper;
        }

        template <typename Skipper>
        [[nodiscard]] constexpr Skipper const&
        get_unused_skipper(unused_skipper<Skipper> const& unused_skipper) noexcept
        {
            static_assert(X3Subject<Skipper>);
            return unused_skipper.skipper;
        }

        template <typename Context>
        struct skip_over_context
        {
            using type = unused_type;
        };

        template <typename Context>
            requires (!std::is_same_v<expectation_failure_t<Context>, unused_type>)
        struct skip_over_context<Context>
        {
            using type = std::remove_cvref_t<decltype(x3::make_context<expectation_failure_tag>(
                x3::get<expectation_failure_tag>(std::declval<Context const&>())
            ))>;
        };

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, X3Subject Skipper>
        constexpr void skip_over(
            It& first, Se const& last, Context& context, Skipper const& skipper
        )
            noexcept(is_nothrow_parsable_v<Skipper, It, Se, typename skip_over_context<Context>::type, unused_type, unused_type>)
        {
        #if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
            (void)context;
            while (skipper.parse(first, last, unused, unused, unused))
                /* loop */;
        #else
            if constexpr (std::is_same_v<expectation_failure_t<Context>, unused_type>)
            {
                // The context given by parent was truly `unused_type`.
                // There exists only one such case in core; that is
                // `x3::phrase_parse(...)` which creates a fresh context
                // for the (post)skipper.
                //
                // In that case, it is perfectly fine to pass `unused`
                // because the skipper should have been wrapped
                // like `x3::with<x3::expectation_failure_tag>(failure)[skipper]`.
                // (Note that we have plenty of static_asserts in other
                // locations to detect the absence of the context.)
                //
                // If we encounter this branch in any other situations,
                // that should be a BUG of `expectation_failure` logic.

                while (skipper.parse(first, last, unused, unused, unused))
                    /* loop */;
            }
            else
            {
                // In order to cut the template instantiation chain,
                // we must *forget* the original context at least once
                // during the (recursive) invocation of skippers.
                //
                // Traditionally, implementation detail of `skip_over`
                // was disposing the context because we can clearly assume
                // that any 'context,' including those provided by users,
                // is semantically meaningless as long as we're just
                // *skipping* iterators. As you can see in the other branch,
                // `unused` was passed for that purpose.
                //
                // However, we need to do a quite different thing when the
                // non-throwing expectation_failure mode is enabled.
                //
                // Since the reference bound to `x3::expectation_failure_tag` is
                // provided by the user in the first place, if we do forget it
                // then it will be impossible to resurrect the value afterwards.
                // It will also be problematic for `skip_over` itself because the
                // underlying skipper may (or may not) raise an expectation failure.
                // In traditional mode, the error was thrown by a C++ exception.
                // But how can we propagate that error without throwing?
                //
                // For this reason we're going to cherry-pick the reference
                // and repack it into a brand new context.

                auto const local_ctx = x3::make_context<expectation_failure_tag>(
                    x3::get<expectation_failure_tag>(context));

                while (skipper.parse(first, last, local_ctx, unused, unused))
                    /* loop */;
            }
        #endif
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
        constexpr void skip_over(It&, Se const&, Context&, unused_type) noexcept
        {
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Skipper>
        constexpr void skip_over(It&, Se const&, Context&, unused_skipper<Skipper> const&) noexcept
        {
        }
    }

    template <typename Context>
    struct has_skipper
      : std::bool_constant<!detail::is_unused_skipper<
            std::remove_cvref_t<decltype(x3::get<skipper_tag>(std::declval<Context>()))>
        >::value>
    {};

    template <typename Context>
    constexpr bool has_skipper_v = has_skipper<Context>::value;

    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context>
    constexpr void skip_over(It& first, Se const& last, Context& context)
        noexcept(noexcept(detail::skip_over(first, last, context, x3::get<skipper_tag>(context))))
    {
        detail::skip_over(first, last, context, x3::get<skipper_tag>(context));
    }
} // boost::spirit::x3

#endif
