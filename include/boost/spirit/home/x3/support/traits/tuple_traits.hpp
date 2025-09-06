/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
================================================_==============================*/
#if !defined(BOOST_SPIRIT_X3_TUPLE_TRAITS_JANUARY_2012_1132PM)
#define BOOST_SPIRIT_X3_TUPLE_TRAITS_JANUARY_2012_1132PM

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/is_view.hpp>
#include <boost/fusion/include/size.hpp>

#include <type_traits>

namespace boost::spirit::x3::traits
{
    // Note that these metafunctions MUST be explicitly derived from `std::bool_constant`,
    // because they are used for tag dispatch in some locations.
    // Beware that `std::conjunction` / `std::disjunction` is not guaranteed to derive
    // from `std::bool_constant`.

    template <typename A, typename B>
    struct has_same_size
        : std::bool_constant<
            fusion::result_of::size<std::remove_cvref_t<A>>::value ==
            fusion::result_of::size<std::remove_cvref_t<B>>::value
        >
    {};

    template <typename A, typename B>
    constexpr bool has_same_size_v = has_same_size<A, B>::value;

    template <typename T, std::size_t N>
    struct has_size
        : std::bool_constant<fusion::result_of::size<std::remove_cvref_t<T>>::value == N>
    {};

    template <typename T, std::size_t N>
    constexpr bool has_size_v = has_size<T, N>::value;

    template <typename A, typename B>
    struct is_same_size_sequence
        : std::bool_constant<std::conjunction_v<
            fusion::traits::is_sequence<std::remove_cvref_t<A>>,
            fusion::traits::is_sequence<std::remove_cvref_t<B>>,
            has_same_size<A, B>
        >>
    {};

    template <typename A, typename B>
    constexpr bool is_same_size_sequence_v = is_same_size_sequence<A, B>::value;

    template <typename Seq>
    struct is_size_one_sequence
        : std::bool_constant<std::conjunction_v<
            fusion::traits::is_sequence<std::remove_cvref_t<Seq>>,
            has_size<Seq, 1>
        >>
    {};

    template <typename Seq>
    constexpr bool is_size_one_sequence_v = is_size_one_sequence<Seq>::value;

    template <typename View>
    struct is_size_one_view
        : std::bool_constant<std::conjunction_v<
            fusion::traits::is_view<std::remove_cvref_t<View>>,
            has_size<View, 1>
        >>
    {};

    template <typename View>
    constexpr bool is_size_one_view_v = is_size_one_view<View>::value;

} // boost::spirit::x3::traits

#endif
