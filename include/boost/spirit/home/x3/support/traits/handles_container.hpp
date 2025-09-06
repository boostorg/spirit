/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_HANDLES_CONTAINER_DEC_18_2010_0920AM)
#define BOOST_SPIRIT_X3_HANDLES_CONTAINER_DEC_18_2010_0920AM

#include <type_traits>

namespace boost::spirit::x3::traits
{
    // Whether a component handles container attributes intrinsically
    // (or whether container attributes need to be split up separately).
    // By default, this gets the Component's handles_container nested value.
    // Components may specialize this if such a handles_container is not
    // readily available (e.g. expensive to compute at compile time).
    template <typename Component, typename Context, typename Enable = void>
    struct handles_container : std::bool_constant<Component::handles_container> {};

    template <typename Component, typename Context>
    constexpr bool handles_container_v = handles_container<Component, Context>::value;

} // boost::spirit::x3::traits

#endif
