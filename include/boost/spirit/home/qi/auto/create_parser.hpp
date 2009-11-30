//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_QI_CREATE_NOV_21_2009_0444PM)
#define BOOST_SPIRIT_QI_CREATE_NOV_21_2009_0444PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/auto/meta_create.hpp>
#include <boost/proto/deep_copy.hpp>

namespace boost { namespace spirit { namespace result_of
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct create_parser
      : proto::result_of::deep_copy<
            typename spirit::traits::meta_create<qi::domain, T>::type
        > {};
}}}

namespace boost { namespace spirit { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    // Main API function for parser creation from data type
    template <typename T>
    typename result_of::create_parser<T>::type
    create_parser()
    {
        return proto::deep_copy(
            spirit::traits::meta_create<qi::domain, T>::call());
    }
}}}

#endif
