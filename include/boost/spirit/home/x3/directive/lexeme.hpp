/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_LEXEME_MARCH_24_2007_0802AM)
#define BOOST_SPIRIT_X3_LEXEME_MARCH_24_2007_0802AM

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename Subject>
    struct lexeme_directive : unary_parser<Subject, lexeme_directive<Subject>>
    {
        using base_type = unary_parser<Subject, lexeme_directive<Subject>>;
        static constexpr bool is_pass_through_unary = true;
        static constexpr bool handles_container = Subject::handles_container;

        template <typename SubjectT>
            requires std::is_constructible_v<Subject, SubjectT>
        constexpr lexeme_directive(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<base_type, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

        template <typename Context>
        using pre_skip_context_t = std::remove_cvref_t<decltype(
            x3::make_context<skipper_tag>(std::declval<unused_skipper_t<Context>&>(), std::declval<Context const&>())
        )>;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires has_skipper_v<Context>
        [[nodiscard]] constexpr bool parse(
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const
            noexcept(
                noexcept(x3::skip_over(first, last, context)) &&
                is_nothrow_parsable_v<Subject, It, Se, pre_skip_context_t<Context>, RContext, Attribute>
            )
        {
            x3::skip_over(first, last, context);

            auto const& skipper = x3::get<skipper_tag>(context);
            unused_skipper_t<Context> unused_skipper(skipper);

            return this->subject.parse(
                first, last,
                x3::make_context<skipper_tag>(unused_skipper, context),
                rcontext,
                attr
            );
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires (!has_skipper_v<Context>)
        [[nodiscard]] constexpr bool parse(
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const
            noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, RContext, Attribute>)
        {
            //  no need to pre-skip if skipper is unused
            return this->subject.parse(first, last, context, rcontext, attr);
        }
    };

    namespace detail
    {
        struct lexeme_gen
        {
            template <X3Subject Subject>
            [[nodiscard]] constexpr lexeme_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const
                noexcept(is_parser_nothrow_constructible_v<lexeme_directive<as_parser_plain_t<Subject>>, Subject>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::lexeme_gen lexeme{};
    }
} // boost::spirit::x3

#endif
