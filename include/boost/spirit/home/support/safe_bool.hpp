/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman
    Copyright (c) 2001-2009 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SAFE_BOOL_HPP)
#define BOOST_SPIRIT_SAFE_BOOL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit
{
    template<class Tag>
    class safe_bool 
    {
    public:
        typedef void (safe_bool::*result_type)();
        result_type operator()(bool b) { return b ? &safe_bool::true_ : 0; }

    private:
        void true_() {}
    };

}}

#endif

