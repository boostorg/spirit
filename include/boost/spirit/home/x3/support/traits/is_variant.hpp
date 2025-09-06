/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_IS_VARIANT_JAN_10_2012_0823AM)
#define BOOST_SPIRIT_X3_IS_VARIANT_JAN_10_2012_0823AM

#include <boost/variant/variant_fwd.hpp> // TODO: remove this

#include <type_traits>

namespace boost::spirit::x3::traits
{
    // TODO: define a legit concept for determining variant-like types

    template <typename T, typename Enabled = void>
    struct is_variant : std::false_type {};

    template <typename T>
    constexpr bool is_variant_v = is_variant<T>::value;

    // By declaring a nested struct named `adapted_variant_tag` in
    // your class, you tell spirit that it is regarded as a variant type.
    // The minimum required interface for such a variant is that it has
    // constructors for various types supported by your variant and a
    // typedef 'types' which is an mpl sequence of the contained types.
    // Note (2025): The above spec is obsolete and will change in the near future.
    //
    // This is an intrusive interface. For a non-intrusive interface,
    // specialize the is_variant trait.
    template <typename T>
        requires requires {
            typename T::adapted_variant_tag;
        }
    struct is_variant<T> : std::true_type
    {};

    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct is_variant<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>>
        : std::true_type
    {};
} // boost::spirit::x3::traits

#endif
