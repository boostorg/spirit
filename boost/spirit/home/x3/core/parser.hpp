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
}}}

#endif
