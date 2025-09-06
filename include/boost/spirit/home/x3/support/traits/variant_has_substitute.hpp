/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_VARIANT_HAS_SUBSTITUTE_APR_18_2014_925AM)
#define BOOST_SPIRIT_X3_VARIANT_HAS_SUBSTITUTE_APR_18_2014_925AM

#include <boost/spirit/home/x3/support/traits/is_substitute.hpp>
#include <boost/mpl/find.hpp> // TODO: remove this

#include <type_traits>

namespace boost::spirit::x3::traits
{
    namespace detail
    {
        template <typename Variant, typename T>
        struct variant_has_substitute_impl
        {
            // Find a type from the Variant that can be a substitute for T.
            // return true_ if one is found, else false_

            using variant_type = Variant;
            using types = typename variant_type::types;
            using end = typename mpl::end<types>::type;
            using iter_1 = typename mpl::find<types, T>::type;

            using iter = typename mpl::eval_if<
                std::is_same<iter_1, end>,
                mpl::find_if<types, traits::is_substitute<T, mpl::_1>>,
                std::type_identity<iter_1>
            >::type;

            using type = std::bool_constant<!std::is_same_v<iter, end>>;
        };
    } // detail

    template <typename Variant, typename T>
    struct variant_has_substitute
        : detail::variant_has_substitute_impl<Variant, T>::type
    {};

    template <typename Variant, typename T>
    constexpr bool variant_has_substitute_v = variant_has_substitute<Variant, T>::value;

    template <typename T>
    struct variant_has_substitute<unused_type, T> : std::true_type {};

    template <typename T>
    struct variant_has_substitute<unused_type const, T> : std::true_type {};

} // boost::spirit::x3::traits

#endif
