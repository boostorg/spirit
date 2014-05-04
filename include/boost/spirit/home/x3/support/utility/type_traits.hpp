/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_TYPE_TRAITS_HPP_INCLUDED
#define BOOST_SPIRIT_X3_TYPE_TRAITS_HPP_INCLUDED

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/utility/declval.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/spirit/home/x3/support/utility/sfinae.hpp>


namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template<class Sig, class = void>
    struct is_callable_impl
      : false_type
    {};

    template<class F, class... A>
    struct is_callable_impl<F(A...), typename disable_if_substitution_failure<
        typename result_of<F(A...)>::type>::type>
      : true_type
    {};
}}}}

namespace boost { namespace spirit { namespace x3
{
    template<class Sig>
    struct is_callable;

    template<class F, class... A>
    struct is_callable<F(A...)>
      : detail::is_callable_impl<F(A...)>
    {};
    
    template <typename T>
    struct remove_rvalue_reference
    {
        typedef T type;
    };
    
    template <typename T>
    struct remove_rvalue_reference<T&&>
    {
        typedef T type;
    };
    
    template <typename T>
    using unrefcv = typename remove_cv<
        typename remove_reference<T>::type>::type;
}}}


#endif

