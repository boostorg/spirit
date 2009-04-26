//  Copyright (c) 2001-2009 Hartmut Kaiser
//  Copyright (c) 2001-2009 Joel de Guzman
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_SEQUENCE_FEB_28_2007_0247PM)
#define SPIRIT_KARMA_SEQUENCE_FEB_28_2007_0247PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/detail/fail_function.hpp>
#include <boost/spirit/home/karma/detail/pass_container.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/detail/what_function.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/algorithm/any_if.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/sequence_base_id.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/not.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<karma::domain, proto::tag::shift_left> // enables <<
      : mpl::true_ {};

    template <>
    struct flatten_tree<karma::domain, proto::tag::shift_left> // flattens <<
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    template <typename Elements>
    struct sequence : nary_generator<sequence<Elements> >
    {
        sequence(Elements const& elements)
          : elements(elements) {}

        typedef Elements elements_type;
        struct sequence_base_id;

        template <typename Context, typename Unused>
        struct attribute
        {
            // Put all the element attributes in a tuple
            typedef typename traits::build_attribute_sequence<
                Elements, Context, mpl::identity>::type
            all_attributes;

            // Now, build a fusion vector over the attributes. Note
            // that build_fusion_vector 1) removes all unused attributes
            // and 2) may return unused_type if all elements have
            // unused_type(s).
            typedef typename
                traits::build_fusion_vector<all_attributes>::type
            type_;

            // Finally, strip single element vectors into its
            // naked form: vector1<T> --> T
            typedef typename
                traits::strip_single_element_vector<type_>::type
            type;
        };

        // standard case. Attribute is a fusion tuple
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate_impl(OutputIterator& sink, Context& ctx
          , Delimiter const& d, Attribute& attr_, mpl::false_) const
        {
            typedef detail::fail_function<
                OutputIterator, Context, Delimiter> fail_function;
            typedef traits::attribute_not_unused<Context> predicate;

            // wrap the attribute in a tuple if it is not a tuple
            typename traits::wrap_if_not_tuple<Attribute>::type attr(attr_);

            // return false if *any* of the generators fail
            return !spirit::any_if(elements, attr, fail_function(sink, ctx, d)
              , predicate());
        }

        // Special case when Attribute is an stl container
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate_impl(OutputIterator& sink, Context& ctx
          , Delimiter const& d, Attribute const& attr_, mpl::true_) const
        {
            // return false if *any* of the generators fail
            typedef detail::fail_function<
                OutputIterator, Context, Delimiter> fail_function;

            return !fusion::any(elements, detail::make_pass_container(
                fail_function(sink, ctx, d), attr_));
        }

        // main generate function. Dispatches to generate_impl depending
        // on the Attribute type.
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx, Delimiter const& d
          , Attribute const& attr) const
        {
            return generate_impl(sink, ctx, d, attr
                , traits::is_container<Attribute>());
        }

        template <typename Context>
        info what(Context& context) const
        {
            info result("sequence");
            fusion::for_each(elements,
                spirit::detail::what_function<Context>(result, context));
            return result;
        }

        Elements elements;
    };


    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Elements, typename Modifiers>
    struct make_composite<proto::tag::shift_left, Elements, Modifiers>
      : make_nary_composite<Elements, sequence>
    {};

}}} 

#endif
