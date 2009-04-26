//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_CHAR_FEB_21_2007_0543PM)
#define BOOST_SPIRIT_KARMA_CHAR_FEB_21_2007_0543PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
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
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/cons.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <typename CharEncoding>
    struct use_terminal<karma::domain
      , tag::char_code<tag::char_, CharEncoding>        // enables char_
    > : mpl::true_ {};

    template <typename CharEncoding, typename A0>
    struct use_terminal<karma::domain
      , terminal_ex<
            tag::char_code<tag::char_, CharEncoding>    // enables char_('x'), char_("x")
          , fusion::vector1<A0>
        >
    > : mpl::true_ {};

    template <typename CharEncoding>                    // enables *lazy* char_('x'), char_("x")
    struct use_lazy_terminal<
        karma::domain
      , tag::char_code<tag::char_, CharEncoding>
      , 1 // arity
    > : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, char>            // enables 'x'
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, char[2]>         // enables "x"
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, wchar_t>         // enables L'x'
      : mpl::true_ {};

    template <>
    struct use_terminal<karma::domain, wchar_t[2]>      // enables L"x"
      : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    using spirit::lit;    // lit('x') is equivalent to 'x'

    ///////////////////////////////////////////////////////////////////////////
    //
    //  any_char
    //      generates a single character from the associated parameter
    //
    //      Note: this generator has to have an associated parameter
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename CharEncoding, typename Tag>
    struct any_char
      : primitive_generator<any_char<CharEncoding, Tag> >
    {
        typedef typename CharEncoding::char_type char_type;
        typedef CharEncoding char_encoding;

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef char_type type;
        };

        // any_char has an attached parameter
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        static bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr)
        {
            return 
                karma::detail::generate_to(sink, attr, char_encoding(), Tag()) &&
                karma::delimit_out(sink, d);       // always do post-delimiting
        }

        // any_char has no attribute attached, it needs to have been
        // initialized from a direct literal
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool generate(OutputIterator&, Context&, Delimiter const&, 
            unused_type const&)
        {
            // It is not possible (doesn't make sense) to use char_ without
            // providing any attribute, as the generator doesn't 'know' what
            // character to output. The following assertion fires if this
            // situation is detected in your code.
            BOOST_MPL_ASSERT_MSG(false, char__not_usable_without_attribute, ());
            return false;
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            return info("any-char");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  literal_char
    //      generates a single character given by a literal it was initialized
    //      from
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename CharEncoding, typename Tag, bool no_attribute>
    struct literal_char
      : primitive_generator<literal_char<CharEncoding, Tag, no_attribute> >
    {
        typedef typename CharEncoding::char_type char_type;
        typedef CharEncoding char_encoding;

        literal_char(char_type ch)
          : ch (spirit::char_class::convert<char_encoding>::to(Tag(), ch)) 
        {}

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef typename mpl::if_c<
                no_attribute, unused_type, char_type>::type
            type;
        };

        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const&) const
        {
            return karma::detail::generate_to(sink, ch) &&
                   karma::delimit_out(sink, d);    // always do post-delimiting
        }

        template <typename Context>
        info what(Context const& ctx) const
        {
            return info("literal-char", char_encoding::toucs4(ch));
        }

        char_type ch;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename Modifiers, typename Encoding>
        struct basic_literal
        {
            static bool const lower =
                has_modifier<Modifiers, tag::char_code_base<tag::lower> >::value;

            static bool const upper =
                has_modifier<Modifiers, tag::char_code_base<tag::upper> >::value;

            static bool const no_attr =
                !has_modifier<Modifiers, tag::lazy_eval>::value;

            typedef literal_char<
                typename spirit::detail::get_encoding<
                    Modifiers, Encoding, lower || upper>::type
              , typename get_casetag<Modifiers, lower || upper>::type
              , no_attr>
            result_type;

            template <typename Char>
            result_type operator()(Char ch, unused_type) const
            {
                return result_type(ch);
            }

            template <typename Char>
            result_type operator()(Char const* str, unused_type) const
            {
                return result_type(str[0]);
            }
        };
    }

    // literals: 'x', "x"
    template <typename Modifiers>
    struct make_primitive<char, Modifiers>
      : detail::basic_literal<Modifiers, char_encoding::standard> {};

    template <typename Modifiers>
    struct make_primitive<char const(&)[2], Modifiers>
      : detail::basic_literal<Modifiers, char_encoding::standard> {};

    // literals: L'x', L"x"
    template <typename Modifiers>
    struct make_primitive<wchar_t, Modifiers>
      : detail::basic_literal<Modifiers, char_encoding::standard_wide> {};

    template <typename Modifiers>
    struct make_primitive<wchar_t const(&)[2], Modifiers>
      : detail::basic_literal<Modifiers, char_encoding::standard_wide> {};

    // char_
    template <typename CharEncoding, typename Modifiers>
    struct make_primitive<tag::char_code<tag::char_, CharEncoding>, Modifiers>
    {
        static bool const lower =
            has_modifier<Modifiers, tag::char_code<tag::lower, CharEncoding> >::value;

        static bool const upper =
            has_modifier<Modifiers, tag::char_code<tag::upper, CharEncoding> >::value;

        typedef any_char<
            typename spirit::detail::get_encoding<
                Modifiers, CharEncoding, lower || upper>::type
          , typename detail::get_casetag<Modifiers, lower || upper>::type
        > result_type;

        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };

    // char_(...)
    template <typename CharEncoding, typename Modifiers, typename A0>
    struct make_primitive<
        terminal_ex<
            tag::char_code<tag::char_, CharEncoding>
          , fusion::vector1<A0>
        >
      , Modifiers>
    {
        static bool const lower =
            has_modifier<Modifiers, tag::char_code<tag::lower, CharEncoding> >::value;

        static bool const upper =
            has_modifier<Modifiers, tag::char_code<tag::upper, CharEncoding> >::value;

        static bool const no_attr =
            !has_modifier<Modifiers, tag::lazy_eval>::value;

        typedef literal_char<
            typename spirit::detail::get_encoding<
                Modifiers, CharEncoding, lower || upper>::type
          , typename detail::get_casetag<Modifiers, lower || upper>::type
          , no_attr
        > result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term, unused_type) const
        {
            return result_type(fusion::at_c<0>(term.args));
        }
    };

    // char_("x")
    template <typename CharEncoding, typename Modifiers, typename Char>
    struct make_primitive<
        terminal_ex<
            tag::char_code<tag::char_, CharEncoding>
          , fusion::vector1<Char(&)[2]> // For single char strings
        >
      , Modifiers>
    {
        static bool const lower =
            has_modifier<Modifiers, tag::char_code<tag::lower, CharEncoding> >::value;

        static bool const upper =
            has_modifier<Modifiers, tag::char_code<tag::upper, CharEncoding> >::value;

        typedef literal_char<
            typename spirit::detail::get_encoding<
                Modifiers, CharEncoding, lower || upper>::type
          , typename detail::get_casetag<Modifiers, lower || upper>::type
          , false
        > result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term, unused_type) const
        {
            return result_type(fusion::at_c<0>(term.args)[0]);
        }
    };

}}}   // namespace boost::spirit::karma

#endif
