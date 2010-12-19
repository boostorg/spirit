//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_AS_STRING_DEC_18_0510PM)
#define SPIRIT_KARMA_AS_STRING_DEC_18_0510PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/generator.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/detail/output_iterator.hpp>
#include <boost/spirit/home/karma/detail/as_string.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/has_semantic_action.hpp>
#include <boost/spirit/home/support/handles_container.hpp>
#include <boost/spirit/home/karma/detail/attributes.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_directive<karma::domain, tag::as_string> // enables as_string
      : mpl::true_ {};

    template <>
    struct use_directive<karma::domain, tag::as_wstring> // enables as_wstring
      : mpl::true_ {};
}}

namespace boost { namespace spirit { namespace karma
{
    using spirit::as_string;
    using spirit::as_string_type;
    using spirit::as_wstring;
    using spirit::as_wstring_type;

    ///////////////////////////////////////////////////////////////////////////
    // as_string_directive allows to hook custom conversions to string into the
    // output generation process
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Char>
    struct as_string_directive 
      : unary_generator<as_string_directive<Subject, Char> >
    {
        typedef Subject subject_type;
        typedef typename subject_type::properties properties;

        as_string_directive(Subject const& subject)
          : subject(subject) {}

        template <typename Context, typename Iterator>
        struct attribute
          : traits::attribute_of<subject_type, Context, Iterator>
        {};

        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx, Delimiter const& d
          , Attribute const& attr) const
        {
            return subject.generate(sink, ctx, d, traits::as_string(attr)) &&
                   karma::delimit_out(sink, d);     // always do post-delimiting 
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("as_string", subject.what(context));
        }

        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Modifiers>
    struct make_directive<tag::as_string, Subject, Modifiers>
    {
        typedef as_string_directive<Subject, char> result_type;

        result_type operator()(unused_type, Subject const& subject
          , unused_type) const
        {
            return result_type(subject);
        }
    };

    template <typename Subject, typename Modifiers>
    struct make_directive<tag::as_wstring, Subject, Modifiers>
    {
        typedef as_string_directive<Subject, wchar_t> result_type;

        result_type operator()(unused_type, Subject const& subject
          , unused_type) const
        {
            return result_type(subject);
        }
    };
}}}

namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Char>
    struct has_semantic_action<karma::as_string_directive<Subject, Char> >
      : unary_has_semantic_action<Subject> {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Attribute, typename Char>
    struct handles_container<
            karma::as_string_directive<Subject, Char>, Attribute>
      : mpl::false_ {};   // always dereference attribute if used in sequences
}}}

#endif
