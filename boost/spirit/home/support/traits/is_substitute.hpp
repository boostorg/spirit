/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_IS_SUBSTITUTE_JAN_9_2012_1049PM)
#define BOOST_SPIRIT_IS_SUBSTITUTE_JAN_9_2012_1049PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/optional/optional.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // Find out if T can be a (strong) substitute for Attribute
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Attribute, typename Enable = void>
    struct is_substitute;

    namespace detail
    {
        template <typename T, typename Attribute>
        struct value_type_is_substitute
          : is_substitute<
                typename container_value<T>::type
              , typename container_value<Attribute>::type>
        {};

        template <typename T, typename Attribute, typename Enable = void>
        struct is_substitute_impl : is_same<T, Attribute> {};

        template <typename T, typename Attribute>
        struct is_substitute_impl<T, Attribute,
            typename enable_if<
                mpl::and_<
                    fusion::traits::is_sequence<T>,
                    fusion::traits::is_sequence<Attribute>,
                    mpl::equal<T, Attribute, is_substitute<mpl::_1, mpl::_2>>
                >
            >::type>
          : mpl::true_ {};

        template <typename T, typename Attribute>
        struct is_substitute_impl<T, Attribute,
            typename enable_if<
                mpl::and_<
                    is_container<T>,
                    is_container<Attribute>,
                    value_type_is_substitute<T, Attribute>
                >
            >::type>
          : mpl::true_ {};
    }

    template <typename T, typename Attribute, typename Enable /*= void*/>
    struct is_substitute
      : detail::is_substitute_impl<T, Attribute> {};

    template <typename T, typename Attribute>
    struct is_substitute<optional<T>, optional<Attribute>>
      : is_substitute<T, Attribute> {};
}}}

#endif
