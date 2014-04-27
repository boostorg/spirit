/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_SPIRIT_X3_DETAIL_ATTRIBUTE_HELPER_HPP_INCLUDED
#define BOOST_SPIRIT_X3_DETAIL_ATTRIBUTE_HELPER_HPP_INCLUDED


#include <tuple>
#include <boost/spirit/home/x3/support/unused.hpp>


namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Attribute>
    struct void_to_unused
    {
        typedef Attribute type;
    };
    
    template <>
    struct void_to_unused<void>
    {
        typedef unused_type type;
    };
    
    template <typename Attribute>
    struct decompose_attribute
    {
        typedef typename
            void_to_unused<Attribute>::type
        result_type;
        
        typedef std::tuple<> params_type;
    };
    
    template <typename Attribute, typename... Ts>
    struct decompose_attribute<Attribute(Ts...)>
    {
        typedef typename
            void_to_unused<Attribute>::type
        result_type;
        
        typedef std::tuple<Ts...> params_type;
    };
}}}}


#endif

