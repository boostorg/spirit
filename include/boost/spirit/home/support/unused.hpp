/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_UNUSED_APRIL_16_2006_0616PM)
#define BOOST_SPIRIT_UNUSED_APRIL_16_2006_0616PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/fusion/include/unused.hpp>
#include <boost/mpl/bool.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // since boost::fusion now supports exactly what we need, unused is simply
    // imported from the fusion namespace
    ///////////////////////////////////////////////////////////////////////////
    using boost::fusion::unused_type;
    using boost::fusion::unused;

    ///////////////////////////////////////////////////////////////////////////
    namespace traits
    {
        // We use this test to detect if the argument is not an unused_type
        template <typename T> struct is_not_unused : mpl::true_ {};
        template <> struct is_not_unused<unused_type> : mpl::false_ {};
    }
}}

#endif
