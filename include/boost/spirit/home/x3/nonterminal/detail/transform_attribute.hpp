/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_NONTERMINAL_DETAIL_TRANSFORM_ATTRIBUTE_HPP
#define BOOST_SPIRIT_X3_NONTERMINAL_DETAIL_TRANSFORM_ATTRIBUTE_HPP

#include <boost/spirit/home/x3/support/traits/transform_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>

#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    namespace detail
    {
        struct parser_id;
    } // detail

    template <typename Exposed, typename Transformed>
        requires std::is_assignable_v<Exposed&, Transformed&&>
    struct default_transform_attribute
    {
        using type = Transformed;

        static constexpr Transformed pre(Exposed&)
            noexcept(std::is_nothrow_default_constructible_v<Transformed>)
        {
            return Transformed{};
        }

        template <typename TransformedT>
        static constexpr void post(Exposed& val, TransformedT&& attribute)
            noexcept(noexcept(traits::move_to(std::forward<TransformedT>(attribute), val)))
        {
            traits::move_to(std::forward<TransformedT>(attribute), val);
        }
    };

    // handle case where no transformation is required as the types are the same
    template <typename Attribute>
    struct default_transform_attribute<Attribute, Attribute>
    {
        using type = Attribute&;
        static constexpr Attribute& pre(Attribute& val) noexcept { return val; }
        static constexpr void post(Attribute&, Attribute const&) noexcept {}
    };

    // main specialization for x3
    template <typename Exposed, typename Transformed, typename Enable = void>
        requires std::is_assignable_v<Exposed&, Transformed&&>
    struct transform_attribute
        : default_transform_attribute<Exposed, Transformed> {};

    // unused_type needs some special handling as well
    template <>
    struct transform_attribute<unused_type, unused_type>
    {
        using type = unused_type;
        static constexpr unused_type pre(unused_type) noexcept { return unused; }
        static constexpr void post(unused_type, unused_type) noexcept {}
    };

    template <>
    struct transform_attribute<unused_type const, unused_type>
        : transform_attribute<unused_type, unused_type> {};

    template <typename Attribute>
    struct transform_attribute<unused_type, Attribute>
        : transform_attribute<unused_type, unused_type> {};

    template <typename Attribute>
    struct transform_attribute<unused_type const, Attribute>
        : transform_attribute<unused_type, unused_type> {};

    template <typename Attribute>
    struct transform_attribute<Attribute, unused_type>
        : transform_attribute<unused_type, unused_type> {};

    template <typename Attribute>
    struct transform_attribute<Attribute const, unused_type>
        : transform_attribute<unused_type, unused_type> {};

} // boost::spirit::x3

///////////////////////////////////////////////////////////////////////////////
namespace boost::spirit::x3::traits
{
    template <typename Exposed, typename Transformed>
        requires std::is_assignable_v<Exposed&, Transformed&&>
    struct transform_attribute<Exposed, Transformed, x3::detail::parser_id>
        : x3::transform_attribute<Exposed, Transformed>
    {
        static_assert(!std::is_reference_v<Exposed>, "Exposed cannot be a reference type");
        static_assert(!std::is_reference_v<Transformed>, "Transformed cannot be a reference type");
    };
} // boost::spirit::x3::traits

#endif
