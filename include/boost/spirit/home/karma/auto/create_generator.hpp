//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_CREATE_NOV_21_2009_0340PM)
#define BOOST_SPIRIT_KARMA_CREATE_NOV_21_2009_0340PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/auto/meta_create.hpp>
#include <boost/proto/deep_copy.hpp>

namespace boost { namespace spirit { namespace result_of
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct create_generator
    {
        typedef spirit::meta_create<karma::domain, T> creator_type;

        typedef typename proto::result_of::deep_copy<
            typename creator_type::type
        >::type type;
    };
}}}

namespace boost { namespace spirit { namespace karma
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    typename result_of::create_generator<T>::type
    create_generator()
    {
        typedef typename result_of::create_generator<T>::creator_type creator_type;
        return proto::deep_copy(creator_type::call());
    }
}}}

#endif
