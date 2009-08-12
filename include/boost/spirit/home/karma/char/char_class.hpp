//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_CHAR_CLASS_AUG_10_2009_0720AM)
#define BOOST_SPIRIT_KARMA_CHAR_CLASS_AUG_10_2009_0720AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/string_traits.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/char_class.hpp>
#include <boost/spirit/home/support/detail/get_encoding.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/delimit_out.hpp>
#include <boost/spirit/home/karma/auxiliary/lazy.hpp>
#include <boost/spirit/home/karma/detail/get_casetag.hpp>
#include <boost/spirit/home/karma/detail/generate_to.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit 
{ 
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    // enables alnum, alpha, graph, etc.
    template <typename CharClass, typename CharEncoding>
    struct use_terminal<karma::domain
        , tag::char_code<CharClass, CharEncoding> >
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  char_class
    //      generates a single character if it matches the given character 
    //      class
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Tag>
    struct char_class
      : primitive_generator<char_class<Tag> >
    {
        typedef typename Tag::char_encoding char_encoding;
        typedef typename char_encoding::char_type char_type;
        typedef typename Tag::char_class classification;

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef char_type type;
        };

        // char_class needs an attached attribute
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        static bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr)
        {
            using spirit::char_class::classify;
            if (!classify<char_encoding>::is(classification(), attr))
                return false;     // allow proper character class only

            return karma::detail::generate_to(sink, attr) &&
                   karma::delimit_out(sink, d);      // always do post-delimiting
        }

        // char_class shouldn't be used without any associated attribute
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool generate(OutputIterator&, Context&, Delimiter const&
          , unused_type const&)
        {
            BOOST_SPIRIT_ASSERT_MSG(false
              , char_class_not_usable_without_attribute, ());
            return false;
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            typedef spirit::char_class::what<char_encoding> what_;
            return info(what_::is(classification()));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  space
    //      generates a single character from the associated parameter
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename CharEncoding>
    struct any_space
      : primitive_generator<any_space<CharEncoding> >
    {
        typedef typename CharEncoding::char_type char_type;
        typedef CharEncoding char_encoding;

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef char_type type;
        };

        // any_space has an attached parameter
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        static bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr)
        {
            using spirit::char_class::classify;
            if (!classify<char_encoding>::is(tag::space(), attr))
                return false;     // allow whitespace only

            return karma::detail::generate_to(sink, attr) &&
                   karma::delimit_out(sink, d);      // always do post-delimiting
        }

        // any_space has no attribute attached, use single space character
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool generate(OutputIterator& sink, Context&, Delimiter const& d, 
            unused_type const&)
        {
            return karma::detail::generate_to(sink, ' ') &&
                   karma::delimit_out(sink, d);      // always do post-delimiting
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            return info("space");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////

    namespace detail
    {
        template <typename Tag, bool lower = false, bool upper = false>
        struct make_char_class : mpl::identity<Tag> {};

        template <>
        struct make_char_class<tag::alpha, true, false> 
          : mpl::identity<tag::lower> {};

        template <>
        struct make_char_class<tag::alpha, false, true> 
          : mpl::identity<tag::upper> {};

        template <>
        struct make_char_class<tag::alnum, true, false> 
          : mpl::identity<tag::lowernum> {};

        template <>
        struct make_char_class<tag::alnum, false, true> 
          : mpl::identity<tag::uppernum> {};
    }

    // enables alnum, alpha, graph, etc.
    template <typename CharClass, typename CharEncoding, typename Modifiers>
    struct make_primitive<tag::char_code<CharClass, CharEncoding>, Modifiers>
    {
        static bool const lower = 
            has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;
        static bool const upper = 
            has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

        typedef tag::char_code<
            typename detail::make_char_class<CharClass, lower, upper>::type
          , CharEncoding>
        tag_type;

        typedef char_class<tag_type> result_type;
        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };

    // space is special
    template <typename CharEncoding, typename Modifiers>
    struct make_primitive<tag::char_code<tag::space, CharEncoding>, Modifiers>
    {
        typedef any_space<CharEncoding> result_type;

        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };

}}}  // namespace boost::spirit::karma

#endif // !defined(BOOST_SPIRIT_KARMA_CHAR_FEB_21_2007_0543PM)
