//  Copyright (c) 2001-2007 Joel de Guzman
//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_POSITIVE_MAR_03_2007_0945PM)
#define BOOST_SPIRIT_KARMA_POSITIVE_MAR_03_2007_0945PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/container.hpp>

#include <boost/type_traits/add_const.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<karma::domain, proto::tag::unary_plus> // enables +g
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    template <typename Subject>
    struct plus : unary_generator<plus<Subject> >
    {
        typedef Subject subject_type;

        template <typename Context, typename Unused>
        struct attribute
        {
            // Build a std::vector from the subject's attribute. Note
            // that build_std_vector may return unused_type if the
            // subject's attribute is an unused_type.
            typedef typename
                traits::build_std_vector<
                    typename traits::attribute_of<Subject, Context>::type
                >::type
            type;
        };

        plus(Subject const& subject)
          : subject(subject) {}

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

            // plus fails if the parameter is empty
            if (traits::compare(it, end))
                return false;

            bool result = true;
            for (/**/; result && !traits::compare(it, end); traits::next(it))
            {
                result = subject.generate(sink, ctx, d, traits::deref(it));
            }
            return result;
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("plus", subject.what(context));
        }

        Subject subject;
    };


    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Elements, typename Modifiers>
    struct make_composite<proto::tag::unary_plus, Elements, Modifiers>
      : make_unary_composite<Elements, plus>
    {};

}}}

#endif
