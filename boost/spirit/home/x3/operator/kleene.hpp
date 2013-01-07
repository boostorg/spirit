/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KLEENE_JANUARY_07_2007_0818AM)
#define SPIRIT_KLEENE_JANUARY_07_2007_0818AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/spirit/home/support/traits/attribute_of.hpp>
//~ #include <boost/spirit/home/x3/detail/fail_function.hpp>
//~ #include <boost/spirit/home/x3/detail/pass_container.hpp>
//~ #include <boost/spirit/home/support/traits/handles_container.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Subject>
    struct kleene : unary_parser<Subject, kleene<Subject>>
    {
        typedef unary_parser<Subject, kleene<Subject>> base_type;
        typedef typename traits::attribute_of<Subject>::type subject_attribute;

        typedef typename
            traits::build_container<subject_attribute>::type
        attribute_type;

        kleene(Subject const& subject)
          : base_type(subject) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            // synthesized attribute needs to be default constructed
            typedef typename traits::container_value<Attribute>::type value_type;
            value_type val = value_type();

            while (this->subject.parse(first, last, context, val))
            {
                // push the parsed value into our attribute
                traits::push_back(attr, val);
            }
            return true;
        }
    };

    template <typename Subject>
    inline kleene<typename extension::as_parser<Subject>::value_type>
    operator*(Subject const& subject)
    {
        typedef
            kleene<typename extension::as_parser<Subject>::value_type>
        result_type;

        return result_type(as_parser(subject));
    }
}}}

#endif
