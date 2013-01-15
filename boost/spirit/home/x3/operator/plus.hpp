/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_PLUS_MARCH_13_2007_0127PM)
#define SPIRIT_PLUS_MARCH_13_2007_0127PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/spirit/home/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Subject>
    struct plus : unary_parser<Subject, plus<Subject>>
    {
        typedef unary_parser<Subject, plus<Subject>> base_type;
        typedef typename traits::attribute_of<Subject>::type subject_attribute;
        typedef mpl::true_ handles_container_attribute;

        typedef typename
            traits::build_container<subject_attribute>::type
        attribute_type;

        plus(Subject const& subject)
          : base_type(subject) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            if (!detail::parse_into_container(
                this->subject, first, last, context, attr))
                return false;

            while (detail::parse_into_container(
                this->subject, first, last, context, attr))
                ;
            return true;
        }
    };

    template <typename Subject>
    inline plus<typename extension::as_parser<Subject>::value_type>
    operator+(Subject const& subject)
    {
        typedef
            plus<typename extension::as_parser<Subject>::value_type>
        result_type;

        return result_type(as_parser(subject));
    }
}}}

#endif
