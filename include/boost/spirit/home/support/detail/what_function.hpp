/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_WHAT_FUNCTION_APRIL_22_2007_0236PM)
#define SPIRIT_WHAT_FUNCTION_APRIL_22_2007_0236PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <string>
#include <boost/spirit/home/support/info.hpp>

namespace boost { namespace spirit { namespace detail
{
    template <typename Context>
    struct what_function
    {
        what_function(info& what, Context& context)
          : what(what), context(context)
        {
            what.value = std::list<info>();
        }

        template <typename Component>
        void operator()(Component const& component) const
        {
            get<std::list<info>&>(what.value).push_back(component.what(context));
        }

        info& what;
        Context& context;
    };
}}}

#endif
