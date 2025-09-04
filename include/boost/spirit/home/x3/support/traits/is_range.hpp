/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_IS_RANGE_DEC_06_2017_1900PM)
#define BOOST_SPIRIT_X3_IS_RANGE_DEC_06_2017_1900PM

#include <boost/range/range_fwd.hpp> // TODO: remove this

#include <iterator>
#include <ranges>
#include <type_traits>

namespace boost::spirit::x3::traits
{
    template <typename T, typename Enable = void>
    struct is_range : std::false_type {};

    template <typename T>
    constexpr bool is_range_v = is_range<T>::value;

    template <typename T>
    struct [[deprecated("Use std::ranges::subrange")]]
        is_range<boost::iterator_range<T>> : std::true_type {};

    template <std::input_or_output_iterator It, std::sentinel_for<It> Se, std::ranges::subrange_kind Kind>
    struct is_range<std::ranges::subrange<It, Se, Kind>> : std::true_type {};

} // boost::spirit::x3::traits

#endif
