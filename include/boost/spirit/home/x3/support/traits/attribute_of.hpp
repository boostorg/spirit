/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2013 Agustin Berge
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ATTRIBUTE_OF_JAN_7_2012_0914AM)
#define BOOST_SPIRIT_X3_ATTRIBUTE_OF_JAN_7_2012_0914AM

#include <type_traits>

namespace boost::spirit::x3::traits
{
    ///////////////////////////////////////////////////////////////////////////
    // Get the attribute type of a component. By default, this gets the
    // Component's attribute_type typedef or instantiates a nested attribute
    // metafunction. Components may specialize this if such an attribute_type
    // is not readily available (e.g. expensive to compute at compile time).
    ///////////////////////////////////////////////////////////////////////////
    template <typename Component, typename Context, typename Enable = void>
    struct attribute_of;

    template <typename Component, typename Context, typename Enable = void>
    using attribute_of_t = typename attribute_of<Component, Context>::type;

    template <typename Component, typename Context>
        requires requires {
            typename Component::attribute_type;
        }
    struct attribute_of<Component, Context>
    {
        using type = typename Component::attribute_type;
    };

    template <typename Component, typename Context>
        requires requires {
            typename Component::template attribute<Context>::type;
        }
    struct attribute_of<Component, Context>
    {
        using type = typename Component::template attribute<Context>::type;
    };

    template <typename Component, typename Context>
        requires Component::is_pass_through_unary
    struct attribute_of<Component, Context>
    {
        static_assert(requires {
            typename Component::subject_type;
        });
        using type = typename attribute_of<typename Component::subject_type, Context>::type;
    };

} // boost::spirit::x3::traits

#endif
