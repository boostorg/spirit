/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_PARSER_OCTOBER_16_2008_0254PM)
#define BOOST_SPIRIT_PARSER_OCTOBER_16_2008_0254PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/home/support/unused.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Subject, typename Action>
    struct action;

    template <typename Derived>
    struct parser
    {
        typedef Derived derived_type;

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }

        template <typename Action>
        action<Derived, Action>
        operator[](Action const& f) const
        {
            return action<Derived, Action>(this->derived(), f);
        }
    };

    namespace extension
    {
        template <typename T, typename Enable = void>
        struct as_parser {};

        template <>
        struct as_parser<unused_type>
        {
            typedef unused_type type;
            typedef unused_type value_type;
            static type call(unused_type)
            {
                return unused;
            }
        };

        template <typename Derived>
        struct as_parser<Derived
            , typename enable_if<is_base_of<parser<Derived>, Derived>>::type>
        {
            typedef Derived const& type;
            typedef Derived value_type;
            static type call(Derived const& p)
            {
                return p.derived();
            }
        };

        template <typename Derived>
        struct as_parser<parser<Derived>>
        {
            typedef Derived const& type;
            typedef Derived value_type;
            static type call(parser<Derived> const& p)
            {
                return p.derived();
            }
        };
    }

    template <typename T>
    inline typename extension::as_parser<T>::type
    as_parser(T const& x)
    {
        return extension::as_parser<T>::call(x);
    }
}}}

#endif
