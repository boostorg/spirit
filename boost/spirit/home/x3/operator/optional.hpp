/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_OPTIONAL_MARCH_23_2007_1117PM)
#define SPIRIT_OPTIONAL_MARCH_23_2007_1117PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>
#include <boost/spirit/home/support/traits/attribute_of.hpp>
#include <boost/spirit/home/support/traits/move_to.hpp>
#include <boost/spirit/home/support/traits/optional_traits.hpp>
#include <boost/spirit/home/support/traits/attribute_category.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Subject>
    struct optional : unary_parser<Subject, optional<Subject>>
    {
        typedef unary_parser<Subject, optional<Subject>> base_type;
        typedef typename traits::attribute_of<Subject>::type subject_attribute;
        static bool const handles_container = true;
        static bool const is_pass_through_unary = true;

        typedef typename
            traits::build_optional<subject_attribute>::type
        attribute_type;

        optional(Subject const& subject)
          : base_type(subject) {}

        // Attribute is a unused_type
        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, unused_type, traits::unused_attribute) const
        {
            this->subject.parse(first, last, context, unused);
            return true;
        }

        // Attribute is a container
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr, traits::container_attribute) const
        {
            detail::parse_into_container(
                this->subject, first, last, context, attr);
            return true;
        }

        // Attribute is a tuple
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr, traits::tuple_attribute) const
        {
            this->subject.parse(first, last, context, attr);
            return true;
        }

        // Attribute is plain
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr, traits::plain_attribute) const
        {
            this->subject.parse(first, last, context, attr);
            return true;
        }

        // Attribute is an optional
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr, traits::optional_attribute) const
        {
            typedef typename
                spirit::traits::optional_value<Attribute>::type
            value_type;

            // create a local value
            value_type val = value_type();

            if (this->subject.parse(first, last, context, val))
            {
                // assign the parsed value into our attribute
                spirit::traits::move_to(val, attr);
            }
            return true;
        }

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            return parse(first, last, context, attr
              , typename traits::attribute_category<Attribute>::type());
        }
    };

    template <typename Subject>
    inline optional<typename extension::as_parser<Subject>::value_type>
    operator-(Subject const& subject)
    {
        typedef
            optional<typename extension::as_parser<Subject>::value_type>
        result_type;

        return result_type(as_parser(subject));
    }
}}}

#endif
