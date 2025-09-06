/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_OPTIONAL_TRAITS_FEBRUARY_06_2007_1001AM)
#define BOOST_SPIRIT_X3_OPTIONAL_TRAITS_FEBRUARY_06_2007_1001AM

#include <boost/spirit/home/x3/support/unused.hpp>

#ifndef BOOST_SPIRIT_X3_USE_BOOST_OPTIONAL
# define BOOST_SPIRIT_X3_USE_BOOST_OPTIONAL 1
#endif

#if BOOST_SPIRIT_X3_USE_BOOST_OPTIONAL
# include <boost/optional/optional.hpp>
#endif

#include <optional>
#include <type_traits>

namespace boost::spirit::x3::traits
{
    template <typename T, typename Enable = void>
    struct is_optional : std::false_type {};

    template <typename T>
    constexpr bool is_optional_v = is_optional<T>::value;

#if BOOST_SPIRIT_X3_USE_BOOST_OPTIONAL
    template <typename T>
    struct [[deprecated("Use std::optional")]] is_optional<boost::optional<T>> : std::true_type {};
#endif

    template <typename T>
    struct is_optional<std::optional<T>> : std::true_type {};

    // Build a optional type from T. Return unused_type if T is unused_type.
    template <typename T>
    struct build_optional
    {
#if BOOST_SPIRIT_X3_USE_BOOST_OPTIONAL
        using type [[deprecated("Use std::optional")]] = boost::optional<T>;
#else
        using type = std::optional<T>;
#endif
    };

    template <typename T>
    using build_optional_t = typename build_optional<T>::type;

#if BOOST_SPIRIT_X3_USE_BOOST_OPTIONAL
    template <typename T>
    struct [[deprecated("Use std::optional")]] build_optional<boost::optional<T>>
    {
        using type = boost::optional<T>;
    };
#endif

    template <typename T>
    struct build_optional<std::optional<T>>
    {
        using type = std::optional<T>;
    };

    template <>
    struct build_optional<unused_type>
    {
        using type = unused_type;
    };

    // Get the optional's value_type. Handles unused_type as well.
    template <typename T>
    struct optional_value { using type = typename T::value_type; };

    template <typename T>
    using optional_value_t = typename optional_value<T>::type;

    template <>
    struct optional_value<unused_type>
    {
        using type = unused_type;
    };

    template <>
    struct optional_value<unused_type const>
    {
        using type = unused_type;
    };

} // boost::spirit::x3::traits

#endif
