//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_EXTRACT_FROM_SEP_30_2009_0732AM)
#define BOOST_SPIRIT_KARMA_EXTRACT_FROM_SEP_30_2009_0732AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/phoenix/core/actor.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/container.hpp>

#include <boost/ref.hpp>
#include <boost/optional.hpp>

namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    //  This file contains attribute extraction utilities. The utilities 
    //  provided also accept spirit's unused_type; all no-ops. Compiler 
    //  optimization will easily strip these away.
    ///////////////////////////////////////////////////////////////////////////
    namespace result_of
    {
        template <typename Attribute, typename Enable/* = void*/>
        struct extract_from
        {
            typedef Attribute const& type;
        };

        template <typename Attribute>
        struct extract_from<optional<Attribute> >
        {
            typedef Attribute const& type;
        };

        template <typename Attribute>
        struct extract_from<optional<Attribute const> >
        {
            typedef Attribute const& type;
        };

        template <>
        struct extract_from<unused_type>
        {
            typedef unused_type type;
        };

        template <>
        struct extract_from<unused_type const>
        {
            typedef unused_type type;
        };
    }

    // This is the default case: the plain attribute values
    template <typename Attribute, typename Enable/* = void*/>
    struct extract_from_attribute
    {
        typedef Attribute const& type;
        static type call(Attribute const& attr)
        {
            return attr;
        }
    };

    // This handles optional attributes. 
    template <typename Attribute>
    struct extract_from_attribute<optional<Attribute> >
    {
        typedef Attribute const& type;
        static type call(optional<Attribute> const& attr)
        {
            return boost::get<Attribute>(attr);
        }
    };

    template <typename Attribute>
    struct extract_from_attribute<optional<Attribute const> >
    {
        typedef Attribute const& type;
        static type call(optional<Attribute const> const& attr)
        {
            return boost::get<Attribute const>(attr);
        }
    };

    // This handles attributes wrapped inside a boost::ref(). 
    template <typename Attribute>
    struct extract_from_attribute<reference_wrapper<Attribute> >
    {
        typedef Attribute const& type;
        static type call(reference_wrapper<Attribute> const& attr)
        {
            return attr.get();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Attribute>
    typename extract_from_attribute<Attribute>::type
    extract_from(Attribute const& attr)
    {
        return extract_from_attribute<Attribute>::call(attr);
    };

    inline unused_type extract_from(unused_type)
    {
        return unused;
    };

}}}

#endif
