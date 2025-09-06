/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_VARIANT_FIND_SUBSTITUTE_APR_18_2014_930AM)
#define BOOST_SPIRIT_X3_VARIANT_FIND_SUBSTITUTE_APR_18_2014_930AM

#include <boost/spirit/home/x3/support/traits/is_substitute.hpp>
#include <boost/mpl/find.hpp> // TODO: remove this

namespace boost::spirit::x3::traits
{
    template <typename Variant, typename T>
    struct variant_find_substitute
    {
        // Get the type from the Variant that can be a substitute for T.
        // If none is found, just return T

        using variant_type = Variant;
        using types = typename variant_type::types;
        using end = typename mpl::end<types>::type;

        using iter_1 = typename mpl::find<types, T>::type;

        using iter = typename mpl::eval_if<
            std::is_same<iter_1, end>,
            mpl::find_if<types, traits::is_substitute<T, mpl::_1> >,
            std::type_identity<iter_1>
        >::type;

        using type = typename mpl::eval_if<
            std::is_same<iter, end>,
            std::type_identity<T>,
            mpl::deref<iter>
        >::type;
    };

    template <typename Variant, typename T>
    using variant_find_substitute_t = typename variant_find_substitute<Variant, T>::type;

    template <typename Variant>
    struct variant_find_substitute<Variant, Variant>
    {
        using type = Variant;
    };

} // boost::spirit::x3::traits

#endif
