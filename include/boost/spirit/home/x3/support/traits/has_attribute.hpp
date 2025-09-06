/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_HAS_ATTRIBUTE_JUN_6_2012_1714PM)
#define BOOST_SPIRIT_X3_HAS_ATTRIBUTE_JUN_6_2012_1714PM

#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>

#include <concepts>
#include <type_traits>

namespace boost::spirit::x3
{
   struct unused_type;
} // boost::spirit::x3

namespace boost::spirit::x3::traits
{
    // TODO: make these 'Component' depend on the concept.
    // It is currently hard to implement due to circular dependency.

    // Whether a component has an attribute. By default, this compares the
    // component attribute against unused_type. If the component provides a
    // nested constant expression has_attribute as a hint, that value is used
    // instead. Components may specialize this.
    template <typename Component, typename Context, typename Enable = void>
    struct has_attribute
    {
        static_assert(requires {
            typename attribute_of<Component, Context>::type;
        });
        static constexpr bool value = !std::is_same_v<attribute_of_t<Component, Context>, unused_type>;
    };

    template <typename Component, typename Context>
    constexpr bool has_attribute_v = has_attribute<Component, Context>::value;

    template <typename Component, typename Context>
        requires requires {
            { Component::has_attribute } -> std::same_as<bool>;
        }
    struct has_attribute<Component, Context>
        : std::bool_constant<Component::has_attribute>
    {};

    template <typename Component, typename Context>
        requires Component::is_pass_through_unary
    struct has_attribute<Component, Context>
    {
        static_assert(requires {
            typename Component::subject_type;
        });
        static constexpr bool value = has_attribute_v<typename Component::subject_type, Context>;
    };

} // boost::spirit::x3::traits

#endif
