//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_LEX_UNIQUE_ID_MAY_24_2009_0313PM)
#define BOOST_SPIRIT_LEX_UNIQUE_ID_MAY_24_2009_0313PM

#if defined(_MSC_VER)
#pragma once
#endif

namespace boost { namespace spirit { namespace lex { namespace lexertl
{ 
    ///////////////////////////////////////////////////////////////////////////
    //  The next_id template needs to be specialized for any non-default token 
    //  id type used by a custom token type. It needs to expose a function
    //  'static Idtype get()' returning the next available unique id each time 
    //  it is called.
    template <typename Idtype>
    struct unique_id;

    ///////////////////////////////////////////////////////////////////////////
    //  Default specialization for the next_id template returning the next 
    //  available token id.
    template <>
    struct unique_id<std::size_t>
    {
        static std::size_t get()
        {
            static std::size_t unique_id_ = 0;
            return unique_id_++;
        }
    };

}}}}

#endif

