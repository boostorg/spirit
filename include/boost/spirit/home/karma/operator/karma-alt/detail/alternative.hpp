//  Copyright (c) 2001-2007 Hartmut Kaiser
//  Copyright (c) 2001-2007 Joel de Guzman
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1124AM)
#define SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1124AM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/support/attribute_of.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>

namespace boost { namespace spirit { namespace karma { namespace detail
{
    template <typename Component, typename OutputIterator,
        typename Context, typename Delimiter, typename Parameter>
    struct alternative_generate_functor
    {
        typedef bool result_type; // to satisfy boost::variant visition

        template <typename Attribute, typename Got>
        struct compute_is_compatible
        {
            typedef typename
                Attribute::types
            types; // mpl sequence of types in the variant

            typedef typename
                mpl::begin<types>::type
            begin; // iterator to the first element

            typedef typename
                mpl::end<types>::type
            end; // iterator to the last element

            typedef typename
                mpl::find_if<
                    types,
                    is_same<mpl::_1, Got> // $$$ fix this
                >::type
            iter;

            typedef typename mpl::distance<begin, iter>::type index;
            typedef typename mpl::not_<is_same<iter, end> >::type type;
            enum { value = type::value };
        };

        template <typename Got>
        struct compute_is_compatible<unused_type, Got> : mpl::false_ {};

        template <typename Attribute, typename Component_, typename Context_>
        struct is_compatible :
            compute_is_compatible<
                typename traits::attribute_of<
                    karma::domain, Component_, Context_>::type,
                Attribute>
        {
        };

        alternative_generate_functor(Component const& component_,
            OutputIterator& sink_, Context& ctx_,
            Delimiter const& d, Parameter const& p)
        : component(component_), sink(sink_), ctx(ctx_), delim(d), param(p)
        {
        }

        template <typename Attribute>
        bool call(Attribute const& actual_attribute, mpl::true_) const
        {
            typedef is_compatible<Attribute, Component, Context> is_compatible;
            typedef typename is_compatible::index index;
            typedef typename Component::elements_type elements;

            typedef typename
                fusion::result_of::value_at<elements, index>::type
            child_component_type;

            typedef typename child_component_type::director director;
            return director::generate(
                fusion::at<index>(component.elements),
                sink, ctx, delim, actual_attribute);
        }

        template <typename Attribute>
        bool call(Attribute const& actual_attribute, mpl::false_) const
        {
            return false;
        }

        template <typename Attribute>
        bool operator()(Attribute const& actual_attribute) const
        {
            typedef mpl::bool_<
                is_compatible<Attribute, Component, Context>::value>
            is_compatible;

            return call(actual_attribute, is_compatible());
        }

        Component const& component;
        OutputIterator& sink;
        Context& ctx;
        Delimiter const& delim;
        Parameter const& param;
    };

}}}}

#endif
