/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024-2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_OPTIONAL_MARCH_23_2007_1117PM)
#define BOOST_SPIRIT_X3_OPTIONAL_MARCH_23_2007_1117PM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/traits/optional_traits.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_category.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    template <typename Subject>
    struct optional : unary_parser<Subject, optional<Subject>>
    {
        using base_type = unary_parser<Subject, optional<Subject>>;
        static constexpr bool is_pass_through_unary = false;
        static constexpr bool handles_container = true;

        template <typename SubjectT>
            requires std::is_constructible_v<Subject, SubjectT>
        constexpr optional(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

        // catch-all overload
        template <
            std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext,
            typename Attribute // unconstrained
        >
        [[nodiscard]] constexpr bool
        parse(
            It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const
            noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, RContext, Attribute>)
        {
            static_assert(Parsable<Subject, It, Se, Context, RContext, Attribute>);

            // discard [[nodiscard]]
            (void)this->subject.parse(first, last, context, rcontext, attr);
            return !x3::has_expectation_failure(context);
        }

        // container attribute
        template <
            std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext,
            traits::CategorizedAttr<traits::container_attribute> Attribute
        >
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(noexcept(detail::parse_into_container(this->subject, first, last, context, rcontext, attr)))
        {
            // discard [[nodiscard]]
            (void)detail::parse_into_container(this->subject, first, last, context, rcontext, attr);
            return !x3::has_expectation_failure(context);
        }

        // optional attribute
        template <
            std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext,
            traits::CategorizedAttr<traits::optional_attribute> Attribute
        >
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            noexcept(
                std::is_nothrow_default_constructible_v<x3::traits::optional_value_t<Attribute>> &&
                is_nothrow_parsable_v<Subject, It, Se, Context, RContext, x3::traits::optional_value_t<Attribute>> &&
                noexcept(x3::traits::move_to(std::declval<x3::traits::optional_value_t<Attribute>&&>(), attr))
            )
        {
            using value_type = x3::traits::optional_value_t<Attribute>;
            value_type val; // default-initialize

            static_assert(Parsable<Subject, It, Se, Context, RContext, value_type>);
            if (this->subject.parse(first, last, context, rcontext, val))
            {
                // assign the parsed value into our attribute
                x3::traits::move_to(std::move(val), attr);
                return true;
            }

            return !x3::has_expectation_failure(context);
        }
    };

    template <X3Subject Subject>
    [[nodiscard]] constexpr optional<as_parser_plain_t<Subject>>
    operator-(Subject&& subject)
        noexcept(is_parser_nothrow_constructible_v<optional<as_parser_plain_t<Subject>>, Subject>)
    {
        return { as_parser(std::forward<Subject>(subject)) };
    }
} // boost::spirit::x3

namespace boost::spirit::x3::traits
{
    template <typename Subject, typename Context>
    struct attribute_of<x3::optional<Subject>, Context>
        : build_optional<attribute_of_t<Subject, Context>>
    {};
} // boost::spirit::x3::traits

#endif
