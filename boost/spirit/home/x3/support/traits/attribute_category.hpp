/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ATTRIBUTE_CATEGORY_JAN_4_2012_1150AM)
#define BOOST_SPIRIT_X3_ATTRIBUTE_CATEGORY_JAN_4_2012_1150AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mpl/identity.hpp>
#include <boost/fusion/include/copy.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/spirit/home/x3/support/traits/is_variant.hpp>
#include <boost/spirit/home/x3/support/traits/container_traits.hpp>

namespace boost { namespace spirit { namespace x3
{
   struct unused_type;
}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    struct unused_attribute {};
    struct plain_attribute {};
    struct container_attribute {};
    struct tuple_attribute {};
    struct variant_attribute {};
    struct optional_attribute {};

    template <typename T, typename Enable = void>
    struct attribute_category
        : mpl::identity<plain_attribute> {};

    template <>
    struct attribute_category<unused_type>
        : mpl::identity<unused_attribute> {};

    template <>
    struct attribute_category<unused_type const>
        : mpl::identity<unused_attribute> {};

    template <typename T>
    struct attribute_category<T,
        typename enable_if<fusion::traits::is_sequence<T>>::type>
        : mpl::identity<tuple_attribute> {};

    template <typename T>
    struct attribute_category<T,
        typename enable_if<traits::is_variant<T>>::type>
        : mpl::identity<variant_attribute> {};

    template <typename T>
    struct attribute_category<T,
        typename enable_if<traits::is_container<T>>::type>
        : mpl::identity<container_attribute> {};

}}}}

#endif
