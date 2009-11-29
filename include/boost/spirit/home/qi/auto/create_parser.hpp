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

namespace boost { namespace spirit { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct parser_creator
    {
        typedef spirit::meta_create<qi::domain, T> creator_type;
        typedef typename proto::result_of::deep_copy<
            typename creator_type::type
        >::type type;

        static type call()
        {
            return proto::deep_copy(creator_type::call());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    typename parser_creator<T>::type
    create_parser()
    {
        return parser_creator<T>::call();
    }
}}}

#endif
