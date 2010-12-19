//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_AS_STRING_DEC_18_0644PM)
#define BOOST_SPIRIT_KARMA_AS_STRING_DEC_18_0644PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/attributes_fwd.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    //  This file contains the attribute to string conversion utility. The 
    //  utility provided also accept spirit's unused_type; all no-ops. Compiler 
    //  optimization will easily strip these away.
    ///////////////////////////////////////////////////////////////////////////

    // This is the default case: the plain attribute values
    template <typename Attribute, typename Enable/*= void*/>
    struct attribute_as_string
    {
        typedef Attribute const& type; 

        static type call(Attribute const& attr)
        {
            return attr;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Attribute>
    typename spirit::result_of::attribute_as_string<Attribute>::type
    as_string(Attribute const& attr)
    {
        return attribute_as_string<Attribute>::call(attr);
    }

    inline unused_type as_string(unused_type)
    {
        return unused;
    }
}}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace result_of
{
    template <typename Attribute>
    struct attribute_as_string
      : traits::attribute_as_string<Attribute>
    {};

    template <>
    struct attribute_as_string<unused_type>
    {
        typedef unused_type type;
    };

    template <>
    struct attribute_as_string<unused_type const>
    {
        typedef unused_type type;
    };
}}}

#endif
