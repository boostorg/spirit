/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_UNUSED_APRIL_16_2006_0616PM)
#define BOOST_SPIRIT_X3_UNUSED_APRIL_16_2006_0616PM

#include <ostream>
#include <istream>
#include <boost/mpl/identity.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace x3
{
    struct unused_type
    {
        unused_type() = default;

        template <typename T>
        constexpr unused_type(T const&) noexcept
        {
        }

        template <typename T>
        constexpr unused_type const&
        operator=(T const&) const noexcept
        {
            return *this;
        }

        template <typename T>
        constexpr unused_type&
        operator=(T const&) noexcept
        {
            return *this;
        }

        // unused_type can also masquerade as an empty context (see context.hpp)

        template <typename ID>
        constexpr unused_type get(ID) const noexcept
        {
            return {};
        }
    };

    constexpr auto const unused = unused_type{};

    constexpr std::ostream& operator<<(std::ostream& out, unused_type const&)
    {
        return out;
    }

    constexpr std::istream& operator>>(std::istream& in, unused_type&)
    {
        return in;
    }
}}}

#endif
