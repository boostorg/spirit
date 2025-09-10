/*=============================================================================
    Copyright (c) 2015 Mario Lang
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_HOME_X3_EXTENSIONS_MATCHES_HPP)
#define BOOST_SPIRIT_HOME_X3_EXTENSIONS_MATCHES_HPP

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename Subject>
    struct matches_directive : unary_parser<Subject, matches_directive<Subject>>
    {
        using base_type = unary_parser<Subject, matches_directive<Subject>>;
        static constexpr bool has_attribute = true;
        using attribute_type = bool;

        template <typename SubjectT>
            requires std::is_constructible_v<Subject, SubjectT>
        constexpr matches_directive(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(
                is_nothrow_parsable_v<Subject, It, Se, Context, RContext, unused_type> &&
                noexcept(traits::move_to(std::declval<bool const&>(), attr))
            )
        {
            bool const matched = this->subject.parse(first, last, context, rcontext, unused);

        #if !BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
            if (x3::has_expectation_failure(context)) return false;
        #endif

            traits::move_to(matched, attr);
            return true;
        }
    };

    namespace detail
    {
        struct matches_gen
        {
            template <X3Subject Subject>
            [[nodiscard]] constexpr matches_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const
                noexcept(is_parser_nothrow_constructible_v<matches_directive<as_parser_plain_t<Subject>>, Subject>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::matches_gen matches{};
    } // cpos

} // boost::spirit::x3

#endif
