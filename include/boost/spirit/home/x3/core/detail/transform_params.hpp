/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_DETAIL_TRANSFORM_PARAMS_HPP_INCLUDED
#define BOOST_SPIRIT_X3_DETAIL_TRANSFORM_PARAMS_HPP_INCLUDED

#if defined(_MSC_VER)
#pragma once
#endif

#include <tuple>
#include <boost/mpl/bool.hpp>
#include <boost/utility/declval.hpp>
#include <boost/spirit/home/x3/support/utility/sfinae.hpp>
#include <boost/spirit/home/x3/support/utility/integer_sequence.hpp>


namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename T, std::size_t N>
    struct array_wrapper
    {
        typedef T const(&const_reference)[N];
        
        array_wrapper(T const(&data)[N])
          : array_wrapper(data, make_index_sequence<N>())
        {}

        template<std::size_t... Ns>
        array_wrapper(T const(&data)[N], index_sequence<Ns...>)
          : data{data[Ns]...}
        {}
        
        operator const_reference() const
        {
            return data; 
        }

        T data[N];
    };
    
    template <typename T>
    struct wrap_param
    {
        typedef T type;
    };

    template <typename T, std::size_t N>
    struct wrap_param<T[N]>
    {
        typedef array_wrapper<T, N> type;
    };

    template <typename Subject, typename Enable, typename... Ts>
    struct transform_params
    {
        typedef std::tuple<typename wrap_param<Ts>::type...> type;
        typedef mpl::false_ tag;
        typedef bool no;
    };
    
    template <typename Subject, typename... Ts>
    struct transform_params<Subject, typename disable_if_substitution_failure<
        decltype(Subject::transform_params(declval<Ts>()...))>::type, Ts...>
    {
        typedef decltype(Subject::transform_params(declval<Ts>()...)) type;
        typedef mpl::true_ tag;
        typedef bool yes;
    };
}}}}


#endif

