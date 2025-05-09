/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2017 wanghan02
    Copyright (c) 2024 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SEQUENCE_JAN_06_2013_1015AM)
#define BOOST_SPIRIT_X3_SEQUENCE_JAN_06_2013_1015AM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/operator/detail/sequence.hpp>
#include <boost/spirit/home/x3/directive/expect.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_of_binary.hpp>
#include <boost/spirit/home/x3/support/meta.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/expectation.hpp>

#include <boost/fusion/include/deque_fwd.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct sequence : binary_parser<Left, Right, sequence<Left, Right>>
    {
        typedef binary_parser<Left, Right, sequence<Left, Right>> base_type;

        constexpr sequence(Left const& left, Right const& right)
            : base_type(left, right) {}

        template <typename Iterator, typename Context, typename RContext>
        bool parse(
            Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, unused_type) const
        {
            Iterator const save = first;

            if (this->left.parse(first, last, context, rcontext, unused)
                && this->right.parse(first, last, context, rcontext, unused))
                return true;

        #if !BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE
            if (has_expectation_failure(context))
            {
                // don't rollback iterator (mimicking exception-like behavior)
                return false;
            }
        #endif

            first = save;
            return false;
        }

        template <typename Iterator, typename Context
          , typename RContext, typename Attribute>
        bool parse(
            Iterator& first, Iterator const& last
          , Context const& context, RContext& rcontext, Attribute& attr) const
        {
            return detail::parse_sequence(*this, first, last, context, rcontext, attr
              , typename traits::attribute_category<Attribute>::type());
        }
    };

    template <typename Left, typename Right>
    constexpr sequence<
        typename extension::as_parser<Left>::value_type
      , typename extension::as_parser<Right>::value_type>
    operator>>(Left const& left, Right const& right)
    {
        return { as_parser(left), as_parser(right) };
    }

    template <typename Left, typename Right>
    constexpr auto operator>(Left const& left, Right const& right)
      -> decltype(left >> expect[right])
    {
        return left >> expect[right];
    }
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Attribute>
    struct types_of_sequence_init : type_sequence<Attribute> {};
    template <>
    struct types_of_sequence_init<unused_type> : type_sequence<> {};
    template <>
    struct types_of_sequence_init<unused_type const> : type_sequence<> {};

    template <typename P, typename C>
    struct get_types_of_sequence
      : types_of_sequence_init<typename traits::attribute_of<P, C>::type> {};

    template <typename L, typename R, typename C>
    struct get_types_of_sequence<x3::sequence<L, R>, C>
      : get_types_of_sequence<L, C>::template extend<get_types_of_sequence<R, C>> {};

    template <template <typename...> class A, typename T, int = T::size>
    struct type_sequence_to_attribute { using type = typename T::template transfer_to<A>; };
    template <template <typename...> class A, typename T>
    struct type_sequence_to_attribute<A, T, 1> : T::template transfer_to<type_identity> {};
    template <template <typename...> class A, typename T>
    struct type_sequence_to_attribute<A, T, 0> { using type = unused_type; };

    template <template <typename...> class A, typename P, typename C>
    using attribute_of_sequence = type_sequence_to_attribute<A,
                                    typename get_types_of_sequence<P, C>::type>;
}}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    template <typename Left, typename Right, typename Context>
    struct attribute_of<x3::sequence<Left, Right>, Context>
        : x3::detail::attribute_of_sequence<fusion::deque, x3::sequence<Left, Right>, Context> {};
}}}}

#endif
