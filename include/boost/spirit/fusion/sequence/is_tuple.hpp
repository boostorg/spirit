/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_IS_TUPLE_HPP)
#define FUSION_SEQUENCE_IS_TUPLE_HPP

#include <boost/type_traits/is_base_and_derived.hpp>

namespace boost { namespace fusion
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  is_tuple metafunction
    //
    //      Given a type T, returns a value true or false if T is a
    //      tuple or not. Usage:
    //
    //          is_tuple<T>::value
    //
    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        struct sequence_root;
    }

    template <typename T>
    struct is_tuple : is_base_and_derived<sequence_root, T> {};

}} // namespace boost::fusion

#endif
