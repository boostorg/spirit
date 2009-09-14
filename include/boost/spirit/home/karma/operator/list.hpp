//  Copyright (c) 2001-2009 Joel de Guzman
//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_LIST_MAY_01_2007_0229PM)
#define SPIRIT_KARMA_LIST_MAY_01_2007_0229PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/attributes.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<karma::domain, proto::tag::modulus> // enables g % d
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    template <typename Left, typename Right>
    struct list : binary_generator<list<Left, Right> >
    {
        typedef Left left_type;
        typedef Right right_type;

        typedef mpl::int_<
            left_type::properties::value | right_type::properties::value
        > properties;

        // Build a std::vector from the LHS's attribute. Note
        // that build_std_vector may return unused_type if the
        // subject's attribute is an unused_type.
        template <typename Context, typename Iterator>
        struct attribute
          : traits::build_std_vector<
                typename traits::attribute_of<Left, Context, Iterator>::type
            >
        {};

        list(Left const& left, Right const& right)
          : left(left), right(right) {}

        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx
          , Delimiter const& d, Attribute const& attr) const
        {
            typedef typename traits::result_of::iterator<
                typename add_const<Attribute>::type
            >::type iterator_type;

            iterator_type it = traits::begin(attr);
            iterator_type end = traits::end(attr);

            bool result = !traits::compare(it, end);
            if (result && left.generate(sink, ctx, d, traits::deref(it)))
            {
                for (traits::next(it); result && !traits::compare(it, end);
                     traits::next(it))
                {
                    result = right.generate(sink, ctx, d, unused) &&
                             left.generate(sink, ctx, d, traits::deref(it));
                }
                return result;
            }
            return false;
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("list",
                std::make_pair(left.what(context), right.what(context)));
        }

        Left left;
        Right right;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Elements, typename Modifiers>
    struct make_composite<proto::tag::modulus, Elements, Modifiers>
      : make_binary_composite<Elements, list>
    {};

}}}

#endif
