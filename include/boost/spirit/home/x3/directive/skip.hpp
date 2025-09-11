/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SKIP_JANUARY_26_2008_0422PM)
#define BOOST_SPIRIT_X3_SKIP_JANUARY_26_2008_0422PM

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename Subject>
    struct reskip_directive : unary_parser<Subject, reskip_directive<Subject>>
    {
        using base_type = unary_parser<Subject, reskip_directive<Subject>>;
        static constexpr bool is_pass_through_unary = true;
        static constexpr bool handles_container = Subject::handles_container;

        template <typename SubjectT>
            requires
                (!std::is_same_v<std::remove_cvref_t<SubjectT>, reskip_directive>) &&
                std::is_constructible_v<Subject, SubjectT>
        constexpr reskip_directive(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires has_skipper_v<Context>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, RContext, Attribute>)
        {
            static_assert(Parsable<Subject, It, Se, Context, RContext, Attribute>);
            return this->subject.parse(first, last, context, rcontext, attr);
        }

    private:
        template <typename Context>
        using context_t = x3::context<
            skipper_tag,
            decltype(detail::get_unused_skipper(x3::get<skipper_tag>(std::declval<Context const&>()))),
            Context
        >;

    public:
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires (!has_skipper_v<Context>)
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            // never noexcept (requires expectation failure modification)
        {
            static_assert(Parsable<Subject, It, Se, context_t<Context>, RContext, Attribute>);

            auto const& skipper = detail::get_unused_skipper(x3::get<skipper_tag>(context));

            auto const local_ctx = x3::make_context<skipper_tag>(skipper, context);
            bool const r = this->subject.parse(first, last, local_ctx, rcontext, attr);

        #if !BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
            if (x3::has_expectation_failure(local_ctx))
            {
                x3::set_expectation_failure(x3::get_expectation_failure(local_ctx), context);
            }
        #endif

            return r;
        }
    };

    template <typename Subject, typename Skipper>
    struct skip_directive : unary_parser<Subject, skip_directive<Subject, Skipper>>
    {
        using base_type = unary_parser<Subject, skip_directive<Subject, Skipper>>;
        static constexpr bool is_pass_through_unary = true;
        static constexpr bool handles_container = Subject::handles_container;

        template <typename SubjectT, typename SkipperT>
            requires std::is_constructible_v<Subject, SubjectT> && std::is_constructible_v<Skipper, SkipperT>
        constexpr skip_directive(SubjectT&& subject, SkipperT&& skipper)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT> && std::is_nothrow_constructible_v<Skipper, SkipperT>)
            : base_type(std::forward<SubjectT>(subject))
            , skipper_(skipper)
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, unused_type const&, RContext& rcontext, Attribute& attr) const
            noexcept(is_nothrow_parsable_v<Subject, It, Se, x3::context<skipper_tag, Skipper>, RContext, Attribute>)
        {
            static_assert(Parsable<Subject, It, Se, x3::context<skipper_tag, Skipper>, RContext, Attribute>);

            // It is perfectly fine to omit the expectation_failure context
            // even in non-throwing mode if and only if the skipper itself
            // is expectation-less.
            //
            // For example:
            //   skip(a > b) [lit('foo')]
            //   skip(c >> d)[lit('foo')]
            //     `a > b`  should require non-`unused_type` context, but
            //     `c >> d` should NOT require non-`unused_type` context
            //
            // However, it's impossible right now to detect whether
            // `this->subject` actually is expectation-less, so we just
            // call the parse function to see what will happen. If the
            // subject turns out to lack the expectation context,
            // static_assert will be engaged in other locations.
            //
            // Anyways, we don't need to repack the expectation context
            // into our brand new skipper context, in contrast to the
            // repacking process done in `x3::skip_over`.
            return this->subject.parse(
                first, last, x3::make_context<skipper_tag>(skipper_), rcontext, attr
            );
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            // never noexcept (requires expectation failure modification)
        {
            static_assert(Parsable<Subject, It, Se, x3::context<skipper_tag, Context>, RContext, Attribute>);

        #if BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
            return this->subject.parse(first, last, x3::make_context<skipper_tag>(skipper_, context), rcontext, attr);

        #else
            static_assert(
                !std::is_same_v<expectation_failure_t<Context>, unused_type>,
                "Context type was not specified for x3::expectation_failure_tag. "
                "You probably forgot: `x3::with<x3::expectation_failure_tag>(failure)[p]`. "
                "Note that you must also bind the context to your skipper."
            );

            // This logic is heavily related to the instantiation chain;
            // see `x3::skip_over` for details.
            auto const local_ctx = x3::make_context<skipper_tag>(skipper_, context);
            bool const r = this->subject.parse(first, last, local_ctx, rcontext, attr);

            if (x3::has_expectation_failure(local_ctx))
            {
                x3::set_expectation_failure(x3::get_expectation_failure(local_ctx), context);
            }
            return r;
        #endif
        }

    private:
        Skipper skipper_;
    };

    namespace detail
    {
        template <X3Subject Skipper>
        struct skip_gen_impl
        {
            // Unreference rvalue reference, but hold lvalue reference as-is
            using skipper_type = std::conditional_t<
                std::is_rvalue_reference_v<Skipper>,
                std::remove_reference_t<Skipper>,
                Skipper
            >;

            template <typename SkipperT>
                requires std::is_same_v<std::remove_cvref_t<SkipperT>, std::remove_cvref_t<Skipper>>
            constexpr skip_gen_impl(SkipperT&& skipper)
                noexcept(std::is_nothrow_constructible_v<skipper_type, SkipperT>)
                : skipper_(std::forward<SkipperT>(skipper))
            {}

            template <X3Subject Subject>
            [[nodiscard]] constexpr skip_directive<as_parser_plain_t<Subject>, std::remove_cvref_t<Skipper>>
            operator[](Subject&& subject) const
                noexcept(
                    is_parser_nothrow_castable_v<Subject> &&
                    std::is_nothrow_constructible_v<
                        skip_directive<as_parser_plain_t<Subject>, std::remove_cvref_t<Skipper>>,
                        as_parser_t<Subject>,
                        skipper_type const&
                    >
                )
            {
                return { as_parser(std::forward<Subject>(subject)), skipper_ };
            }

        private:
            skipper_type skipper_;
        };

        struct skip_gen
        {
            template <X3Subject Skipper>
            [[nodiscard]]
            static constexpr skip_gen_impl<as_parser_t<Skipper>>
            operator()(Skipper&& skipper)
                noexcept(
                    is_parser_nothrow_castable_v<Skipper> &&
                    std::is_nothrow_constructible_v<skip_gen_impl<as_parser_t<Skipper>>, as_parser_t<Skipper>>
                )
            {
                return { as_parser(std::forward<Skipper>(skipper)) };
            }

            template <typename Subject>
            [[nodiscard, deprecated("Use `x3::reskip[p]`.")]]
            /* static */ constexpr reskip_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const // MSVC 2022 bug: cannot define `static operator[]` even in C++26 mode
                noexcept(is_parser_nothrow_constructible_v<reskip_directive<as_parser_plain_t<Subject>>, Subject>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };

        struct reskip_gen
        {
            template <X3Subject Subject>
            [[nodiscard]]
            /* static */ constexpr reskip_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const // MSVC 2022 bug: cannot define `static operator[]` even in C++26 mode
                noexcept(is_parser_nothrow_constructible_v<reskip_directive<as_parser_plain_t<Subject>>, Subject>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::skip_gen skip{};
        inline constexpr detail::reskip_gen reskip{};
    }
} // boost::spirit::x3

#endif
