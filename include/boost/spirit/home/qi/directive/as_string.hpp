/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_AS_STRING_DECEMBER_6_2010_1013AM)
#define SPIRIT_AS_STRING_DECEMBER_6_2010_1013AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/skip_over.hpp>
#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/qi/detail/assign_to.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/has_semantic_action.hpp>
#include <boost/range/iterator_range.hpp>
#include <string>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_directive<qi::domain, tag::as_string> // enables as_string
      : mpl::true_ {};

    template <>
    struct use_directive<qi::domain, tag::as_wstring> // enables as_wstring
      : mpl::true_ {};
}}

namespace boost { namespace spirit { namespace qi
{
    using spirit::as_string;
    using spirit::as_string_type;
    using spirit::as_wstring;
    using spirit::as_wstring_type;

    template <typename Subject, typename Char>
    struct as_string_directive : unary_parser<as_string_directive<Subject, Char> >
    {
        typedef Subject subject_type;
        as_string_directive(Subject const& subject)
          : subject(subject) {}

        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef std::basic_string<Char> type;
        };

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper, Attribute& attr) const
        {
            qi::skip_over(first, last, skipper);
            Iterator i = first;
            std::basic_string<Char> str_attr;
            if (subject.parse(i, last, context, skipper, str_attr))
            {
                spirit::traits::assign_to(str_attr, attr);
                first = i;
                return true;
            }
            return false;
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("as_string", subject.what(context));
        }

        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Modifiers>
    struct make_directive<tag::as_string, Subject, Modifiers>
    {
        typedef as_string_directive<Subject, char> result_type;
        result_type operator()(unused_type, Subject const& subject, unused_type) const
        {
            return result_type(subject);
        }
    };

    template <typename Subject, typename Modifiers>
    struct make_directive<tag::as_wstring, Subject, Modifiers>
    {
        typedef as_string_directive<Subject, wchar_t> result_type;
        result_type operator()(unused_type, Subject const& subject, unused_type) const
        {
            return result_type(subject);
        }
    };
}}}

namespace boost { namespace spirit { namespace traits
{
    template <typename Subject, typename Char>
    struct has_semantic_action<qi::as_string_directive<Subject, Char> >
      : unary_has_semantic_action<Subject> {};
}}}

#endif
