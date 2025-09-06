/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ALTERNATIVE_DETAIL_JAN_07_2013_1245PM)
#define BOOST_SPIRIT_X3_ALTERNATIVE_DETAIL_JAN_07_2013_1245PM

#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/pseudo_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/is_variant.hpp>
#include <boost/spirit/home/x3/support/traits/tuple_traits.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/traits/variant_has_substitute.hpp>
#include <boost/spirit/home/x3/support/traits/variant_find_substitute.hpp>

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>

#include <boost/fusion/include/front.hpp>

#include <iterator>
#include <type_traits>

namespace boost::spirit::x3
{
    template <typename Left, typename Right>
    struct alternative;
} // boost::spirit::x3

namespace boost::spirit::x3::detail
{
    struct pass_variant_unused
    {
        using type = unused_type;

        template <typename T>
        [[nodiscard]] static constexpr unused_type
        call(T&) noexcept
        {
            return unused_type{};
        }
    };

    template <typename Attribute>
    struct pass_variant_used
    {
        using type = Attribute&;

        [[nodiscard]] static constexpr Attribute&
        call(Attribute& v) noexcept
        {
            return v;
        }
    };

    template <>
    struct pass_variant_used<unused_type> : pass_variant_unused {};

    template <typename Parser, typename Attribute, typename Context, typename Enable = void>
    struct pass_parser_attribute
    {
        using attribute_type = traits::attribute_of_t<Parser, Context>;
        using substitute_type = traits::variant_find_substitute_t<Attribute, attribute_type>;

        using type = std::conditional_t<
            std::is_same_v<Attribute, substitute_type>,
            Attribute&,
            substitute_type
        >;

        template <typename Attribute_>
            requires std::is_same_v<Attribute_, std::remove_reference_t<type>>
        [[nodiscard]] static constexpr Attribute_&
        call(Attribute_& attribute) noexcept
        {
            return attribute;
        }

        template <typename Attribute_>
            requires (!std::is_same_v<Attribute_, std::remove_reference_t<type>>)
        [[nodiscard]] static type
        call(Attribute_&)
            noexcept(std::is_nothrow_default_constructible_v<type>)
        {
            return type{};
        }
    };

    // Pass non-variant attributes as-is
    template <typename Parser, typename Attribute, typename Context, typename Enable = void>
    struct pass_non_variant_attribute
    {
        using type = Attribute&;

        [[nodiscard]] constexpr static Attribute&
        call(Attribute& attribute) noexcept
        {
            return attribute;
        }
    };

    // Unwrap single element sequences
    template <typename Parser, typename Attribute, typename Context>
        requires traits::is_size_one_sequence_v<Attribute>
    struct pass_non_variant_attribute<Parser, Attribute, Context>
    {
        using attr_type = typename std::remove_reference_t<
            typename fusion::result_of::front<Attribute>::type
        >;
        using pass = pass_parser_attribute<Parser, attr_type, Context>;
        using type = typename pass::type;

        template <typename Attribute_>
        [[nodiscard]] static constexpr type
        call(Attribute_& attr)
            noexcept(noexcept(pass::call(fusion::front(attr))))
        {
            return pass::call(fusion::front(attr));
        }
    };

    template <typename Parser, typename Attribute, typename Context>
        requires (!traits::is_variant_v<Attribute>)
    struct pass_parser_attribute<Parser, Attribute, Context>
        : pass_non_variant_attribute<Parser, Attribute, Context>
    {};

    template <typename Parser, typename Context>
    struct pass_parser_attribute<Parser, unused_type, Context>
        : pass_variant_unused
    {};

    template <typename Parser, typename Attribute, typename Context>
    struct pass_variant_attribute
        : std::conditional_t<
            traits::has_attribute_v<Parser, Context>,
            pass_parser_attribute<Parser, Attribute, Context>,
            pass_variant_unused
        >
    {};

    template <typename L, typename R, typename Attribute, typename Context>
    struct pass_variant_attribute<alternative<L, R>, Attribute, Context>
        : std::conditional_t<
            traits::has_attribute_v<alternative<L, R>, Context>,
            pass_variant_used<Attribute>,
            pass_variant_unused
        >
    {};

    template <typename Parser, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
    using parse_alternative_pseudo_t = traits::pseudo_attribute<
        Context, typename pass_variant_attribute<Parser, Attribute, Context>::type, It, Se
    >;

    template <typename Parser, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
    constexpr bool is_reference_pseudo_type = std::is_reference_v<typename parse_alternative_pseudo_t<Parser, It, Se, Context, Attribute>::type>;

    template <typename Parser, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        requires is_reference_pseudo_type<Parser, It, Se, Context, Attribute>
    [[nodiscard]] constexpr bool
    parse_alternative(
        Parser const& p, It& first, Se const& last,
        Context const& context, RContext& rcontext, Attribute& attribute
    ) noexcept(
        noexcept(parse_alternative_pseudo_t<Parser, It, Se, Context, Attribute>::call(
            first, last, pass_variant_attribute<Parser, Attribute, Context>::call(attribute)
        )) &&
        is_nothrow_parsable_v<
            Parser, It, Se, Context, RContext,
            typename parse_alternative_pseudo_t<Parser, It, Se, Context, Attribute>::type
        >
    )
    {
        using pass = pass_variant_attribute<Parser, Attribute, Context>;
        using pseudo = traits::pseudo_attribute<Context, typename pass::type, It, Se>;
        typename pseudo::type attr_ = pseudo::call(first, last, pass::call(attribute));
        return p.parse(first, last, context, rcontext, attr_);
    }

    template <typename Parser, std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
        requires (!is_reference_pseudo_type<Parser, It, Se, Context, Attribute>)
    [[nodiscard]] constexpr bool
    parse_alternative(
        Parser const& p, It& first, Se const& last,
        Context const& context, RContext& rcontext, Attribute& attribute
    ) noexcept(
        noexcept(parse_alternative_pseudo_t<Parser, It, Se, Context, Attribute>::call(
            first, last, pass_variant_attribute<Parser, Attribute, Context>::call(attribute)
        )) &&
        is_nothrow_parsable_v<
            Parser, It, Se, Context, RContext,
            typename parse_alternative_pseudo_t<Parser, It, Se, Context, Attribute>::type
        > &&
        noexcept(traits::move_to(
            std::declval<typename parse_alternative_pseudo_t<Parser, It, Se, Context, Attribute>::type&&>(),
            attribute
        ))
    )
    {
        using pass = pass_variant_attribute<Parser, Attribute, Context>;
        using pseudo = traits::pseudo_attribute<Context, typename pass::type, It, Se>;
        typename pseudo::type attr_ = pseudo::call(first, last, pass::call(attribute));

        if (p.parse(first, last, context, rcontext, attr_))
        {
            traits::move_to(std::move(attr_), attribute);
            return true;
        }
        return false;
    }

    template <typename Subject>
    struct alternative_helper : unary_parser<Subject, alternative_helper<Subject>>
    {
        static constexpr bool is_pass_through_unary = true;

        using unary_parser<Subject, alternative_helper<Subject>>::unary_parser;

        template <typename Iterator, typename Context, typename RContext, typename Attribute>
        [[nodiscard]] constexpr bool parse(
            Iterator& first, Iterator const& last, Context const& context, RContext& rcontext, Attribute& attr
        ) const noexcept(noexcept(detail::parse_alternative(this->subject, first, last, context, rcontext, attr)))
        {
            return detail::parse_alternative(this->subject, first, last, context, rcontext, attr);
        }
    };

    template <typename Left, typename Right, typename Context, typename RContext>
    struct parse_into_container_impl<alternative<Left, Right>, Context, RContext>
    {
        using parser_type = alternative<Left, Right>;

        template <typename Iterator, typename Attribute>
            requires traits::is_variant_v<traits::container_value_t<Attribute>>
        [[nodiscard]] static constexpr bool
        call(
            parser_type const& parser,
            Iterator& first, Iterator const& last, Context const& context, RContext& rcontext, Attribute& attribute
        )
        {
            return detail::parse_into_container(alternative_helper<Left>{parser.left}, first, last, context, rcontext, attribute)
                || detail::parse_into_container(alternative_helper<Right>{parser.right}, first, last, context, rcontext, attribute);
        }

        template <typename Iterator, typename Attribute>
            requires (!traits::is_variant_v<traits::container_value_t<Attribute>>)
        [[nodiscard]] static constexpr bool
        call(
            parser_type const& parser,
            Iterator& first, Iterator const& last,
            Context const& context, RContext& rcontext, Attribute& attribute
        )
        {
            return detail::parse_into_container(parser.left, first, last, context, rcontext, attribute)
                || detail::parse_into_container(parser.right, first, last, context, rcontext, attribute);
        }
    };

} // boost::spirit::x3::detail

#endif
