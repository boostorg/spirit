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
    template <typename DerivedT>
    class safe_bool
    {
        void dummy_func(DerivedT*) {}
        typedef void (safe_bool::*bool_type)(DerivedT*);

    public:
        operator bool_type() const
        { return static_cast<const DerivedT*>(this)->operator_bool() ?
            &safe_bool::dummy_func : 0; }

        operator bool_type()
        { return static_cast<DerivedT*>(this)->operator_bool() ?
            &safe_bool::dummy_func : 0; }
    };

}}

#endif

