/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_ASSIGN_TO_APR_16_2006_0812PM)
#define BOOST_SPIRIT_ASSIGN_TO_APR_16_2006_0812PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/attribute_category.hpp>

namespace boost { namespace spirit { namespace traits
{
    namespace detail
    {
        template <typename T, typename Attribute>
        inline void
        assign_to(T const& val, Attribute& attr, unused_attribute)
        {
        }

        template <typename T, typename Attribute>
        inline void
        assign_to(T const& val, Attribute& attr, plain_attribute)
        {
            attr = val;
        }
    }

    template <typename T, typename Attribute>
    inline void
    assign_to(T const& val, Attribute& attr)
    {
        detail::assign_to(val, attr, typename attribute_category<T>::type());
    }
}}}

#endif
