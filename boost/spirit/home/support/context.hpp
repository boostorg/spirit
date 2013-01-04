/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONTEXT_JAN_4_2012_1215PM)
#define BOOST_SPIRIT_CONTEXT_JAN_4_2012_1215PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/unused.hpp>
#include <boost/mpl/identity.hpp>

namespace boost { namespace spirit
{
    template <typename ID, typename T, typename Next = unused_type>
    struct context
    {
        context(T& val, Next const& next)
            : val(val), next(next) {}

        T& get(mpl::identity<ID>) const
        {
            return val;
        }

        template <typename Identity>
        decltype(std::declval<Next>().find(Identity()))
        get(Identity id) const
        {
            return next.get(id);
        }

        T& val;
        Next const& next;
    };

    template <typename ID, typename T>
    struct context<ID, T, unused_type>
    {
        context(T& val)
            : val(val) {}

        T& get(mpl::identity<ID>) const
        {
            return val;
        }

        T& val;
    };

    template <typename Tag, typename Context>
    inline auto
    get(Context const& context) -> decltype(context.get(mpl::identity<Tag>()))
    {
        return context.get(mpl::identity<Tag>());
    }
}}

#endif
