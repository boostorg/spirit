/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_PARSE_INTO_CONTAINER_JAN_15_2013_0957PM)
#define BOOST_SPIRIT_X3_PARSE_INTO_CONTAINER_JAN_15_2013_0957PM

#include <boost/spirit/home/x3/support/traits/container_traits.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/pseudo_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/handles_container.hpp>
#include <boost/spirit/home/x3/support/traits/has_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/is_substitute.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>

#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/back.hpp>

// TODO: remove Boost.Variant usage
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <ranges>
#include <iterator>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3::detail
{
    template <typename Attribute, typename Value>
    struct saver_visitor;

    // save to associative fusion container where Key is simple type
    template <typename Key, typename Enable = void>
    struct save_to_assoc_attr
    {
        template <typename Value, typename Attribute>
        static constexpr void call(Key const&, Value&& value, Attribute& attr)
            noexcept(noexcept(traits::move_to(std::move(value), fusion::at_key<Key>(attr))))
        {
            static_assert(std::is_rvalue_reference_v<Value&&>);
            traits::move_to(std::move(value), fusion::at_key<Key>(attr));
        }
    };

    // save to associative fusion container where Key
    // is variant over possible keys
    template <typename... Ts>
    struct save_to_assoc_attr<variant<Ts...>>
    {
        template <typename Value, typename Attribute>
        static constexpr void call(variant<Ts...> const& key, Value&& value, Attribute& attr)
        {
            static_assert(std::is_rvalue_reference_v<Value&&>);
            boost::apply_visitor(saver_visitor<Attribute, Value>(attr, std::move(value)), key);
        }
    };

    template <typename Attribute, typename Value>
    struct saver_visitor : boost::static_visitor<void>
    {
        constexpr saver_visitor(Attribute& attr, Value&& value) noexcept
            : attr(attr)
            , value(std::move(value))
        {};

        Attribute& attr;
        Value&& value;

        template <typename Key>
        constexpr void operator()(Key const& key) const
            noexcept(noexcept(save_to_assoc_attr<Key>::call(key, std::move(value), attr)))
        {
            save_to_assoc_attr<Key>::call(key, std::move(value), attr);
        }
    };

    template <typename Parser, typename Container, typename Context>
    struct parser_accepts_container
        : traits::is_substitute<traits::attribute_of_t<Parser, Context>, Container>
    {};

    template <typename Parser, typename Container, typename Context>
    constexpr bool parser_accepts_container_v = parser_accepts_container<Parser, Container, Context>::value;

    template <typename Parser>
    struct parse_into_container_base_impl
    {
        // Parser has attribute (synthesize; Attribute is a container)
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires (!parser_accepts_container_v<Parser, Attribute, Context>)
        [[nodiscard]] static constexpr bool
        call_synthesize(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) // never noexcept (requires container insertion)
        {
            // synthesized attribute needs to be value initialized
            using value_type = traits::container_value_t<Attribute>;
            value_type val; // default-initialize

            static_assert(Parsable<Parser, It, Se, Context, RContext, value_type>);
            if (!parser.parse(first, last, context, rcontext, val))
            {
                return false;
            }

            // push the parsed value into our attribute
            traits::push_back(attr, std::move(val));
            return true;
        }

        // Parser has attribute (synthesize; Attribute is a container)
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires parser_accepts_container_v<Parser, Attribute, Context>
        [[nodiscard]] static constexpr bool
        call_synthesize(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, RContext, Attribute>)
        {
            static_assert(Parsable<Parser, It, Se, Context, RContext, Attribute>);
            return parser.parse(first, last, context, rcontext, attr);
        }

        // Parser has attribute && it is NOT fusion sequence
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires
                traits::has_attribute_v<Parser, Context> &&
                (!fusion::traits::is_sequence<Attribute>::value)
        [[nodiscard]] static constexpr bool
        call(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        )
        {
            // TODO: reduce call stack while keeping maintainability
            return parse_into_container_base_impl::call_synthesize(parser, first, last, context, rcontext, attr);
        }

        // Parser has attribute && it is fusion sequence (NOT associative)
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires
                traits::has_attribute_v<Parser, Context> &&
                fusion::traits::is_sequence<Attribute>::value &&
                (!fusion::traits::is_associative<Attribute>::value)
        [[nodiscard]] static constexpr bool
        call(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) noexcept(noexcept(parse_into_container_base_impl::call_synthesize(parser, first, last, context, rcontext, fusion::front(attr))))
        {
            static_assert(traits::has_size_v<Attribute, 1>, "Expecting a single element fusion sequence");
            // TODO: reduce call stack while keeping maintainability
            return parse_into_container_base_impl::call_synthesize(parser, first, last, context, rcontext, fusion::front(attr));
        }

        // Parser has attribute && it is fusion sequence (associative)
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires
                traits::has_attribute_v<Parser, Context> &&
                fusion::traits::is_sequence<Attribute>::value &&
                fusion::traits::is_associative<Attribute>::value
        [[nodiscard]] static constexpr bool
        call(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) // never noexcept (requires container insertion)
        {
            using attribute_type = traits::attribute_of_t<Parser, Context>;
            static_assert(traits::has_size_v<attribute_type, 2>, "To parse directly into fusion map, parser must produce 2 element attr");

            attribute_type attr_; // default-initialize
            static_assert(Parsable<Parser, It, Se, Context, RContext, attribute_type>);
            if (!parser.parse(first, last, context, rcontext, attr_))
            {
                return false;
            }

            // Use the type of first element of attribute as key
            using key = std::remove_reference_t<
                typename fusion::result_of::front<attribute_type>::type
            >;

            save_to_assoc_attr<key>::call(std::move(fusion::front(attr_)), std::move(fusion::back(attr_)), attr);
            return true;
        }

        // Parser has no attribute (pass unused)
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
            requires (!traits::has_attribute_v<Parser, Context>)
        [[nodiscard]] static constexpr bool
        call(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& /* attr */
        ) noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, RContext, unused_type>)
        {
            static_assert(Parsable<Parser, It, Se, Context, RContext, unused_type>);
            return parser.parse(first, last, context, rcontext, unused);
        }
    };

    template <typename Parser, typename Context, typename RContext, typename Enable = void>
    struct parse_into_container_impl : parse_into_container_base_impl<Parser> {};


    template <typename Parser, typename Context, typename RContext>
        requires traits::handles_container_v<Parser, Context>
    struct parse_into_container_impl<Parser, Context, RContext>
    {
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        static constexpr bool pass_attibute_as_is = std::disjunction_v<
            parser_accepts_container<Parser, Attribute, Context>,

            std::negation<traits::is_substitute< // parser attribute is substitute for container value?
                traits::pseudo_attribute_t<
                    Context,
                    traits::attribute_of_t<Parser, Context>,
                    It, Se
                >,
                traits::container_value_t<Attribute>
            >>
        >;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
            requires (!pass_attibute_as_is<It, Se, Attribute>)
        [[nodiscard]] static constexpr bool
        call(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) noexcept(noexcept(parse_into_container_base_impl<Parser>::call(
            parser, first, last, context, rcontext, attr
        )))
        {
            return parse_into_container_base_impl<Parser>::call(
                parser, first, last, context, rcontext, attr
            );
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se>
            requires pass_attibute_as_is<It, Se, unused_type>
        [[nodiscard]] static constexpr bool
        call(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, unused_type
        ) noexcept(is_nothrow_parsable_v<Parser, It, Se, Context, RContext, unused_type>)
        {
            static_assert(Parsable<Parser, It, Se, Context, RContext, unused_type>);
            return parser.parse(first, last, context, rcontext, unused);
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
            requires pass_attibute_as_is<It, Se, Attribute>
        [[nodiscard]] static constexpr bool
        call(
            Parser const& parser, It& first, Se const& last,
            Context const& context, RContext& rcontext, Attribute& attr
        ) // never noexcept (requires container insertion)
        {
            static_assert(Parsable<Parser, It, Se, Context, RContext, Attribute>);
            if (traits::is_empty(attr))
            {
                return parser.parse(first, last, context, rcontext, attr);
            }

            Attribute rest; // default-initialize
            if (!parser.parse(first, last, context, rcontext, rest))
            {
                return false;
            }
            traits::append(
                attr,
                std::make_move_iterator(std::ranges::begin(rest)),
                std::make_move_iterator(std::ranges::end(rest))
            );
            return true;
        }
    };

    template <
        typename Parser, std::forward_iterator It, std::sentinel_for<It> Se,
        typename Context, typename RContext, typename Attribute
    >
    [[nodiscard]] constexpr bool
    parse_into_container(
        Parser const& parser, It& first, Se const& last,
        Context const& context, RContext& rcontext, Attribute& attr
    ) noexcept(noexcept(parse_into_container_impl<Parser, Context, RContext>::call(
        parser, first, last, context, rcontext, attr
    )))
    {
        return parse_into_container_impl<Parser, Context, RContext>::call(
            parser, first, last, context, rcontext, attr
        );
    }

} // boost::spirit::x3::detail

#endif
