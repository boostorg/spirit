/*=============================================================================
    Copyright (c) 2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_DIRECTIVE_RAW_HPP
#define BOOST_SPIRIT_X3_DIRECTIVE_RAW_HPP

#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/traits/pseudo_attribute.hpp>

#ifndef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
# pragma message("Use of `boost::iterator_range` is deprecated in X3. #define BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE")
# include <boost/range/iterator_range_core.hpp>
#endif

#include <ranges>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    // Pseudo attribute type indicating that the parser wants the
    // iterator range pointing to the [first, last) matching characters from
    // the input iterators.
    struct raw_attribute_type {};

    template <typename Subject>
    struct raw_directive : unary_parser<Subject, raw_directive<Subject>>
    {
        using base_type = unary_parser<Subject, raw_directive<Subject>>;
        using attribute_type = raw_attribute_type;
        using subject_type = Subject;

        static constexpr bool handles_container = true;

        template <typename SubjectT>
            requires std::is_constructible_v<Subject, SubjectT>
        constexpr raw_directive(SubjectT&& subject)
            noexcept(std::is_nothrow_constructible_v<Subject, SubjectT>)
            : base_type(std::forward<SubjectT>(subject))
        {}

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, Attribute& attr) const
            // never noexcept; construction of `std::ranges::subrange` is never noexcept
        {
            static_assert(Parsable<Subject, It, Se, Context, RContext, unused_type>);

            x3::skip_over(first, last, context);
            It local_it = first;
            if (this->subject.parse(local_it, last, context, rcontext, unused))
            {
                traits::move_to(first, local_it, attr);
                first = local_it;
                return true;
            }
            return false;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext>
        [[nodiscard]] constexpr bool
        parse(It& first, Se const& last, Context const& context, RContext& rcontext, unused_type) const
            noexcept(is_nothrow_parsable_v<Subject, It, Se, Context, RContext, unused_type>)
        {
            return this->subject.parse(first, last, context, rcontext, unused);
        }
    };

    namespace detail
    {
        struct raw_gen
        {
            template <X3Subject Subject>
            [[nodiscard]] constexpr raw_directive<as_parser_plain_t<Subject>>
            operator[](Subject&& subject) const
                noexcept(is_parser_nothrow_constructible_v<raw_directive<as_parser_plain_t<Subject>>, as_parser_t<Subject>>)
            {
                return { as_parser(std::forward<Subject>(subject)) };
            }
        };
    } // detail

    inline namespace cpos
    {
        inline constexpr detail::raw_gen raw{};
    }

    namespace traits
    {
        template <typename Context, std::forward_iterator It>
        struct pseudo_attribute<Context, raw_attribute_type, It>
        {
            using attribute_type = raw_attribute_type;
        #ifdef BOOST_SPIRIT_X3_NO_BOOST_ITERATOR_RANGE
            using type = std::ranges::subrange<It>;
        #else
            using type = boost::iterator_range<It>;
        #endif

            [[nodiscard]] static constexpr type call(It& first, std::sentinel_for<It> auto const& last, raw_attribute_type)
            {
                return { first, last };
            }
        };
    }
} // boost::spirit::x3

#endif
