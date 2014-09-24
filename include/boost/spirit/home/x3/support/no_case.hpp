/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_SUPPORT_NO_CASE_SEPT_24_2014_1125PM)
#define BOOST_SPIRIT_X3_SUPPORT_NO_CASE_SEPT_24_2014_1125PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace x3
{
    struct no_case_tag {};

    template <typename Context>
    struct has_no_case : 
        is_same< decltype( x3::get<x3::no_case_tag>(boost::declval<Context>()))
               , x3::no_case_tag >::type
    {};

}}}

#endif
