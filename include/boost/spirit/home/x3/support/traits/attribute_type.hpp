/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ATTRIBUTE_TYPE_JAN_5_2012_0358PM)
#define BOOST_SPIRIT_X3_ATTRIBUTE_TYPE_JAN_5_2012_0358PM

namespace boost::spirit::x3::traits
{
    // Retrieve the attribute type to use from the given type.
    // This is needed to extract the correct attribute type from proxy classes
    // as utilized in `FUSION_ADAPT_ADT` et. al.
    template <typename Attribute, typename Enable = void>
    struct attribute_type
    {
        using type = Attribute;
    };

    template <typename Attribute>
    using attribute_type_t = typename attribute_type<Attribute>::type;

} // boost::spirit::x3::traits

#endif
