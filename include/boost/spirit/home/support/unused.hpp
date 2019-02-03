/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_UNUSED_APRIL_16_2006_0616PM)
#define BOOST_SPIRIT_UNUSED_APRIL_16_2006_0616PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <boost/mpl/bool.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // We do not import fusion ::unused_type anymore to avoid boost::fusion
    // being turned into an associate namespace for boost::spirit, as this
    // interferes with ADL in unexpected ways. We rather copy the full 
    // unused_type implementation from boost::fusion.
    ///////////////////////////////////////////////////////////////////////////
    struct unused_type
    {
        BOOST_DEFAULTED_FUNCTION(unused_type() BOOST_NOEXCEPT, {})

        template <typename T>
        BOOST_CONSTEXPR unused_type(T const&) BOOST_NOEXCEPT
        {
        }

        template <typename T>
        BOOST_CONSTEXPR unused_type const&
        operator=(T const&) const BOOST_NOEXCEPT
        {
            return *this;
        }

        template <typename T>
        BOOST_CXX14_CONSTEXPR unused_type&
        operator=(T const&) BOOST_NOEXCEPT
        {
            return *this;
        }
    };

    BOOST_CONSTEXPR unused_type const unused = unused_type();

    namespace detail
    {
        struct unused_only
        {
            BOOST_CONSTEXPR unused_only(unused_type const&) BOOST_NOEXCEPT {}
        };
    }

    template <typename Out>
    BOOST_CONSTEXPR inline Out& operator<<(Out& out, detail::unused_only const&)
    {
        return out;
    }

    template <typename In>
    BOOST_CONSTEXPR inline In& operator>>(In& in, unused_type&)
    {
        return in;
    }

    ///////////////////////////////////////////////////////////////////////////
    namespace traits
    {
        // We use this test to detect if the argument is not an unused_type
        template <typename T> struct not_is_unused : mpl::true_ {};
        template <> struct not_is_unused<unused_type> : mpl::false_ {};
    }
}}

#endif
