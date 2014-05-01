/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_DETAIL_EVAL_HPP_INCLUDED
#define BOOST_SPIRIT_X3_DETAIL_EVAL_HPP_INCLUDED

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/type_traits/decay.hpp>
#include <boost/spirit/home/x3/support/traits/is_parser.hpp>
#include <boost/spirit/home/x3/support/utility/unrefcv.hpp>
#include <boost/spirit/home/x3/support/utility/is_callable.hpp>


namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename T, typename Context, typename = void>
    struct eval_impl
    {
        static T&& apply(T&& val, Context const&)
        {
            return std::forward<T>(val);
        }
    };

    template <typename F, typename Context>
    struct eval_impl<F, Context,
        typename enable_if_c<!traits::is_parser<typename decay<F>::type>::value
            && is_callable<F(Context const&)>::value, void>::type>
    {
        static auto apply(F&& f, Context const& ctx)->decltype(f(ctx))
        {
            return f(ctx);
        }
    };
    
    template <typename T, typename Context>
    inline auto eval(T&& val, Context const& ctx)->
        decltype(eval_impl<T, Context>::apply(std::forward<T>(val), ctx))
    {
        return eval_impl<T, Context>::apply(std::forward<T>(val), ctx);
    }
    
    template <typename T, typename Context>
    using result_of_eval =
        unrefcv_t<decltype(detail::eval(declval<T>(), declval<Context>()))>;
}}}}


#endif

