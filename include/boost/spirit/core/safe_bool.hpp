/*=============================================================================
    Copyright (c) 2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SAFE_BOOL_HPP)
#define BOOST_SPIRIT_SAFE_BOOL_HPP

#include <boost/config.hpp>

namespace boost { namespace spirit
{
#if BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3003))

    template <typename T>
    struct safe_bool
    {
        typedef bool type;
    };

    template <typename T>
    inline bool
    make_safe_bool(bool cond)
    {
        return cond;
    }

#else

    template <typename T>
    struct safe_bool
    {
        T* dummy;
        typedef T* safe_bool::*type;
    };

    template <typename T>
    inline typename safe_bool<T>::type
    make_safe_bool(bool cond)
    {
        return cond ? &safe_bool<T>::dummy : 0;
    }

#endif

}}

#endif

