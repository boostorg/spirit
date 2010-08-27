//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_ADAPT_CLASS_ATTRIBUTES_JAN_27_2010_1219PM)
#define BOOST_SPIRIT_ADAPT_CLASS_ATTRIBUTES_JAN_27_2010_1219PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/fusion/include/adapt_class.hpp>
#include <boost/utility/enable_if.hpp>

///////////////////////////////////////////////////////////////////////////////
// customization points allowing to use adapted classes with spirit
namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, int N>
    struct is_container<fusion::extension::access::class_member_proxy<T, N> >
      : is_container<
          typename fusion::extension::access::class_member_proxy<T, N>::lvalue>
    {};

    template <typename T, int N>
    struct container_value<fusion::extension::access::class_member_proxy<T, N> >
      : container_value<
          typename fusion::extension::access::class_member_proxy<T, N>::lvalue>
    {};

    template <typename T, int N, typename Val>
    struct push_back_container<fusion::extension::access::class_member_proxy<T, N>, Val
      , typename enable_if<is_reference<
            typename fusion::extension::access::class_member_proxy<T, N>::lvalue
        > >::type>
    {
        static bool call(fusion::extension::access::class_member_proxy<T, N>& p
          , Val const& val)
        {
            typedef typename 
                fusion::extension::access::class_member_proxy<T, N>::lvalue 
            type;
            return push_back(type(p), val);
        }
    };

    template <typename T, int N>
    struct container_iterator<fusion::extension::access::class_member_proxy<T, N> >
      : container_iterator<
            typename fusion::extension::access::class_member_proxy<T, N>::lvalue>
    {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, int N, typename Val>
    struct assign_to_attribute_from_value<
        fusion::extension::access::class_member_proxy<T, N>, Val>
    {
        static void 
        call(Val const& val
          , fusion::extension::access::class_member_proxy<T, N>& attr)
        {
            attr = val;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, int N>
    struct attribute_type<fusion::extension::access::class_member_proxy<T, N> > 
      : fusion::extension::access::class_member_proxy<T, N> 
    {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, int N, typename Attribute, typename Domain>
    struct transform_attribute<
        fusion::extension::access::class_member_proxy<T, N>, Attribute, Domain
      , typename disable_if<is_reference<
            typename fusion::extension::access::class_member_proxy<T, N>::lvalue
        > >::type>
    {
        typedef Attribute type;

        static Attribute 
        pre(fusion::extension::access::class_member_proxy<T, N>& val) 
        { 
            return val; 
        }
        static void 
        post(fusion::extension::access::class_member_proxy<T, N>& val
          , Attribute const& attr) 
        {
            val = attr;
        }
        static void 
        fail(fusion::extension::access::class_member_proxy<T, N>&) 
        {
        }
    };

    template <typename T, int N, typename Attribute, typename Domain>
    struct transform_attribute<
        fusion::extension::access::class_member_proxy<T, N>, Attribute, Domain
      , typename enable_if<is_reference<
            typename fusion::extension::access::class_member_proxy<T, N>::lvalue
        > >::type>
    {
        typedef Attribute& type;

        static Attribute& 
        pre(fusion::extension::access::class_member_proxy<T, N>& val) 
        { 
            return val; 
        }
        static void 
        post(fusion::extension::access::class_member_proxy<T, N>&, Attribute const&) 
        {
        }
        static void 
        fail(fusion::extension::access::class_member_proxy<T, N>&) 
        {
        }
    };

    template <typename T, int N>
    struct clear_value<fusion::extension::access::class_member_proxy<T, N> >
    {
        static void call(fusion::extension::access::class_member_proxy<T, N>& val)
        {
            typedef typename 
                fusion::extension::access::class_member_proxy<T, N>::lvalue 
            type;
            clear(type(val));
        }
    };
}}}

#endif
