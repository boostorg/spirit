/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2001-2012 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_MAKE_ATTRIBUTE_JAN_8_2012_0721PM)
#define BOOST_SPIRIT_MAKE_ATTRIBUTE_JAN_8_2012_0721PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mpl/if.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/spirit/home/support/unused.hpp>

namespace boost { namespace spirit { namespace traits
{
    template <typename Attribute, typename ActualAttribute>
    struct make_attribute
    {
        typedef typename remove_const<Attribute>::type attribute_type;
        typedef typename remove_const<ActualAttribute>::type actual_attribute_type;

        typedef typename
            mpl::if_<
                is_same<actual_attribute_type, unused_type>
              , attribute_type
              , ActualAttribute&>::type
        type;

        typedef typename
            mpl::if_<
                is_same<actual_attribute_type, unused_type>
              , attribute_type
              , ActualAttribute>::type
        value_type;

        static Attribute call(unused_type)
        {
             // synthesize the attribute/parameter
            return attribute_type();
        }

        template <typename T>
        static T& call(T& value)
        {
            return value; // just pass the one provided
        }
    };

    template <typename Attribute, typename ActualAttribute>
    struct make_attribute<Attribute&, ActualAttribute>
      : make_attribute<Attribute, ActualAttribute> {};

    template <typename Attribute, typename ActualAttribute>
    struct make_attribute<Attribute const&, ActualAttribute>
      : make_attribute<Attribute const, ActualAttribute> {};

    template <typename ActualAttribute>
    struct make_attribute<unused_type, ActualAttribute>
    {
        typedef unused_type type;
        typedef unused_type value_type;
        static unused_type call(unused_type)
        {
            return unused;
        }
    };
}}}

#endif
