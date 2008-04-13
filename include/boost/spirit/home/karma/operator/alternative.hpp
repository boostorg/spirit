//  Copyright (c) 2001-2008 Hartmut Kaiser
//  Copyright (c) 2001-2007 Joel de Guzman
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1117AM)
#define SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1117AM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/karma/operator/detail/alternative.hpp>
#include <boost/spirit/home/support/attribute_transform.hpp>
#include <boost/spirit/home/support/as_variant.hpp>
#include <boost/spirit/home/support/detail/what_function.hpp>
#include <boost/spirit/home/support/algorithm/any.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/variant.hpp>

namespace boost { namespace spirit { namespace karma
{
    struct alternative
    {
        template <typename T>
        struct transform_child : mpl::identity<T> {};

        template <typename All, typename Filtered>
        struct build_container
        {
            // Ok, now make a variant over the attribute_sequence. It's
            // a pity that make_variant_over does not support forward MPL
            // sequences. We use our own conversion metaprogram (as_variant).
            typedef typename
                as_variant<Filtered>::type
            type;
        };

        template <typename Component, typename Context, typename Iterator>
        struct attribute :
            build_fusion_sequence<alternative, Component, Iterator, Context>
        {
        };

        template <typename Component, typename OutputIterator,
            typename Context, typename Delimiter, typename Parameter>
        static bool
        generate(Component const& component, OutputIterator& sink,
            Context& ctx, Delimiter const& d, Parameter const& param)
        {
            typedef detail::alternative_generate_functor<
                Component, OutputIterator, Context, Delimiter, Parameter>
            functor;

            functor f(component, sink, ctx, d, param);
            return boost::apply_visitor(f, param);
        }

        template <typename Component, typename OutputIterator,
            typename Context, typename Delimiter>
        static bool
        generate(Component const& component, OutputIterator& sink,
            Context& ctx, Delimiter const& d, unused_type)
        {
            typedef typename
                fusion::result_of::value_at_c<
                    typename Component::elements_type, 0>::type
            child_component_type;

            typedef typename child_component_type::director director;
            return director::generate(
                fusion::at_c<0>(component.elements),
                sink, ctx, d, unused);
        }

        template <typename Component>
        static std::string what(Component const& component)
        {
            std::string result = "alternatives[";
            fusion::for_each(component.elements,
                spirit::detail::what_function(result));
            result += "]";
            return result;
        }
    };
}}}

#endif
