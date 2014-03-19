/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_IS_SUBSTITUTE_JAN_9_2012_1049PM)
#define BOOST_SPIRIT_X3_IS_SUBSTITUTE_JAN_9_2012_1049PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/support/traits/container_traits.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/optional/optional.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace x3 { namespace traits
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

    // compare fusion::pair to any mpl sequence
    template <typename Pair, typename T>
    struct pair_equal
	: mpl::and_< traits::has_size<T, 2>
	, is_same<typename Pair::first_type, typename mpl::at_c<T, 0>::type>
	, is_substitute<typename Pair::second_type, typename mpl::at_c<T, 1>::type>
	> {};

    template <typename T, typename Attribute, typename Enable /*= void*/>
    struct is_substitute
      : detail::is_substitute_impl<T, Attribute> {};

    // 2 element mpl tuple is compatible with fusion::map if:
    // - it's first element type is existing key in map
    // - it second element type is compatible to type stored at the key in map
    template <typename T, typename ...Pair>
    struct is_substitute<T, fusion::map<Pair...> >
	: mpl::not_equal_to<
	mpl::count_if<fusion::map<Pair...>, pair_equal<mpl::_1, T> >
	, mpl::int_<0> > {};

    template <typename T, typename Attribute>
    struct is_substitute<optional<T>, optional<Attribute>>
      : is_substitute<T, Attribute> {};
}}}}

#endif
