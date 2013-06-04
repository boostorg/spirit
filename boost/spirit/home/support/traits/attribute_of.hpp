/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_ATTRIBUTE_OF_JAN_7_2012_0914AM)
#define BOOST_SPIRIT_ATTRIBUTE_OF_JAN_7_2012_0914AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mpl/identity.hpp>

namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // Get the attribute type of a component. By default, this simply gets
    // the Component's attribute_type typedef. Components may specialize
    // this if such an attribute_type is not readily available (e.g. expensive
    // to compute at compile time).
    ///////////////////////////////////////////////////////////////////////////
    template <typename Component, typename Context, typename Enable = void>
    struct attribute_of : mpl::identity<typename Component::attribute_type> {};

}}}

#endif
