/*=============================================================================
    Copyright (c) 2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SAFE_BOOL_HPP)
#define BOOST_SPIRIT_SAFE_BOOL_HPP

namespace boost { namespace spirit
{
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
}}

#endif

