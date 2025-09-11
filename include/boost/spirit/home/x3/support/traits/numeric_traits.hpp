/*=============================================================================
	Copyright (c) 2001-2011 Hartmut Kaiser
	Copyright (c) 2025 Nana Sakisaka

	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_NUMERIC_TRAITS_JAN_07_2011_0722AM
#define BOOST_SPIRIT_X3_NUMERIC_TRAITS_JAN_07_2011_0722AM

#include <boost/config.hpp>

#include <limits>
#include <type_traits>

namespace boost::spirit::x3::traits
{
    // Determine if T is a boolean type
    template <typename T>
    struct is_bool : std::is_same<T, bool> {};

	template <typename T>
	constexpr bool is_bool_v = is_bool<T>::value;

    template <typename T>
    struct is_bool<T const> : is_bool<T> {};

    // Determine if T is a signed integer type
    template <typename T>
    struct is_int : std::is_signed<T> {};

	template <typename T>
	constexpr bool is_int_v = is_int<T>::value;

    template <typename T>
    struct is_int<T const> : is_int<T> {};

    // Determine if T is an unsigned integer type
    template <typename T>
    struct is_uint : std::is_unsigned<T> {};

    template <typename T>
    struct is_uint<T const> : is_uint<T> {};

	template <typename T>
	constexpr bool is_uint_v = is_uint<T>::value;

    // Determine if T is a floating point type
    template <typename T>
    struct is_real : std::is_floating_point<T> {};

    template <typename T>
    struct is_real<T const> : is_real<T> {};

    template <typename T>
    constexpr bool is_real_v = is_real<T>::value;

	// --------------------------------------------------

    // Customization points for numeric operations
    template <typename T, typename Enable = void>
    struct absolute_value;

    template <typename T, typename Enable = void>
    struct is_negative;

    template <typename T, typename Enable = void>
    struct is_zero;

    template <typename T, typename Enable = void>
    struct pow10_helper;

    template <typename T, typename Enable = void>
    struct is_nan;

    template <typename T, typename Enable = void>
    struct is_infinite;

    template <typename T, typename Enable = void>
    struct check_overflow : std::bool_constant<std::numeric_limits<T>::is_bounded> {};

} // boost::spirit::x3::traits

#endif
