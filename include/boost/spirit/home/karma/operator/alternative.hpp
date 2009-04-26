//  Copyright (c) 2001-2009 Hartmut Kaiser
//  Copyright (c) 2001-2009 Joel de Guzman
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1117AM)
#define SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1117AM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/karma/detail/alternative_function.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/detail/what_function.hpp>
#include <boost/fusion/include/any.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/for_each.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<karma::domain, proto::tag::bitwise_or>  // enables |
      : mpl::true_ {};

    template <>
    struct flatten_tree<karma::domain, proto::tag::bitwise_or>  // flattens |
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    template <typename Elements>
    struct alternative : nary_generator<alternative<Elements> >
    {
        template <typename Context, typename Unused = unused_type>
        struct attribute
        {
            // Put all the element attributes in a tuple
            typedef typename traits::build_attribute_sequence<
                Elements, Context, mpl::identity
            >::type all_attributes;

            // Ok, now make a variant over the attribute sequence. Note that
            // build_variant makes sure that 1) all attributes in the variant
            // are unique 2) puts the unused attribute, if there is any, to
            // the front and 3) collapses single element variants, variant<T>
            // to T.
            typedef typename traits::build_variant<all_attributes>::type type;
        };

        alternative(Elements const& elements)
          : elements(elements) {}

        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx
          , Delimiter const& d, Attribute const& attr) const
        {
            typedef detail::alternative_generate_functor<
                OutputIterator, Context, Delimiter, Attribute
            > functor;

            // f return true if *any* of the parser succeeds
            functor f (sink, ctx, d, attr);
            return fusion::any(elements, f);
        }

        template <typename Context>
        info what(Context& context) const
        {
            info result("alternative");
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
    struct make_composite<proto::tag::bitwise_or, Elements, Modifiers>
      : make_nary_composite<Elements, alternative>
    {};

}}}

#endif
