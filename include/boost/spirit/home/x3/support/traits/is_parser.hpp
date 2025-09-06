/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2014 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_IS_PARSER_MAY_20_2013_0235PM)
#define BOOST_SPIRIT_X3_IS_PARSER_MAY_20_2013_0235PM

#include <boost/spirit/home/x3/core/parser.hpp>

#include <type_traits>

namespace boost::spirit::x3::traits
{
    // is_parser<T>: metafunction that evaluates to `std::true_type` if `T`
    // models `X3Subject`, `std::false_type` otherwise
    template <typename T, typename Enable = void>
    struct [[deprecated("Use the concept `x3::X3Subject`")]]
    is_parser : std::bool_constant<X3Subject<T>>
    {};

} // boost::spirit::x3::traits

#endif
