/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_DETAIL_CHECK_ARGS_HPP_INCLUDED
#define BOOST_SPIRIT_X3_DETAIL_CHECK_ARGS_HPP_INCLUDED


#include <tuple>
#include <boost/mpl/identity.hpp>
#include <boost/spirit/home/x3/support/utility/type_traits.hpp>


namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Params, typename... As>
    struct check_args;
    
    template <typename... Ts, typename... As>
    struct check_args<std::tuple<Ts...>, As...>
      : is_callable<typename mpl::identity<void(*)(Ts...)>::type(As...)> {};
}}}}


#endif

