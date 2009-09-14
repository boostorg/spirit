//  Copyright (c) 2001-2009 Joel de Guzman
//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_OPTIONAL_MARCH_31_2007_0852AM)
#define SPIRIT_KARMA_OPTIONAL_MARCH_31_2007_0852AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/optional.hpp>
#include <boost/type_traits/is_convertible.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_operator<karma::domain, proto::tag::negate> // enables -g
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        template <typename Attribute>
        inline bool
        optional_is_valid(boost::optional<Attribute> const& opt)
        {
            return opt;
        }

        template <typename Attribute>
        inline bool
        optional_is_valid(Attribute const& opt)
        {
            return true;
        }

        inline bool
        optional_is_valid(unused_type)
        {
            return true;
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename Attribute>
        inline Attribute const&
        optional_get(boost::optional<Attribute> const& opt)
        {
            return get(opt);
        }

        template <typename Attribute>
        inline Attribute const&
        optional_get(Attribute const& opt)
        {
            return opt;
        }

        inline unused_type
        optional_get(unused_type)
        {
            return unused;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject>
    struct optional : unary_generator<optional<Subject> >
    {
        typedef Subject subject_type;
        typedef typename subject_type::properties properties;

        // Build a boost::optional from the subject's attribute. Note
        // that boost::optional may return unused_type if the
        // subject's attribute is an unused_type.
        template <typename Context, typename Iterator = unused_type>
        struct attribute
          : traits::build_optional<
                typename traits::attribute_of<Subject, Context, Iterator>::type
            >
        {};

        optional(Subject const& subject)
          : subject(subject) {}

        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx
          , Delimiter const& d, Attribute const& attr) const
        {
            if (detail::optional_is_valid(attr)) 
                subject.generate(sink, ctx, d, detail::optional_get(attr));
            return sink_is_good(sink);
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("optional", subject.what(context));
        }

        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Elements, typename Modifiers>
    struct make_composite<proto::tag::negate, Elements, Modifiers>
      : make_unary_composite<Elements, optional> {};

}}}

#endif
