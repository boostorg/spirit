/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CHAR_CLASS_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_X3_CHAR_CLASS_APRIL_16_2006_1051AM

#include <boost/spirit/home/x3/char/char_parser.hpp>
#include <boost/spirit/home/x3/char/char_class_tags.hpp>
#include <boost/spirit/home/x3/char/detail/cast_char.hpp>

#include <boost/spirit/home/x3/char_encoding/detail/encoding_warning.hpp>
#include <boost/spirit/home/x3/char_encoding/standard.hpp>
#include <boost/spirit/home/x3/char_encoding/standard_wide.hpp>
#include <boost/spirit/home/x3/char_encoding/ascii.hpp>
#include <boost/spirit/home/x3/char_encoding/iso8859_1.hpp>

namespace boost::spirit::x3
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Encoding>
    struct char_class_base
    {
        using char_type = typename Encoding::classify_type;

#define BOOST_SPIRIT_X3_CLASSIFY(name)                                          \
        template <typename Char>                                                \
        [[nodiscard]] static constexpr bool                                     \
        is(name##_tag, Char ch) noexcept                                        \
        {                                                                       \
            return Encoding::is##name                                           \
                BOOST_PREVENT_MACRO_SUBSTITUTION                                \
                    (detail::cast_char<char_type>(ch));                         \
        }                                                                       \
        /***/

        BOOST_SPIRIT_X3_CLASSIFY(char)
        BOOST_SPIRIT_X3_CLASSIFY(alnum)
        BOOST_SPIRIT_X3_CLASSIFY(alpha)
        BOOST_SPIRIT_X3_CLASSIFY(digit)
        BOOST_SPIRIT_X3_CLASSIFY(xdigit)
        BOOST_SPIRIT_X3_CLASSIFY(cntrl)
        BOOST_SPIRIT_X3_CLASSIFY(graph)
        BOOST_SPIRIT_X3_CLASSIFY(lower)
        BOOST_SPIRIT_X3_CLASSIFY(print)
        BOOST_SPIRIT_X3_CLASSIFY(punct)
        BOOST_SPIRIT_X3_CLASSIFY(space)
        BOOST_SPIRIT_X3_CLASSIFY(blank)
        BOOST_SPIRIT_X3_CLASSIFY(upper)

#undef BOOST_SPIRIT_X3_CLASSIFY
    };

    template <typename Encoding, typename Tag>
    struct char_class : char_parser<char_class<Encoding, Tag>>
    {
        using encoding = Encoding;
        using tag = Tag;
        using char_type = typename Encoding::char_type;
        using attribute_type = char_type;
        static constexpr bool has_attribute = true;

        template <typename Char, typename Context>
        [[nodiscard]] constexpr bool test(Char ch, Context const& context) const noexcept
        {
            return encoding::ischar(ch)
                && char_class_base<Encoding>::is(
                    x3::get_case_compare<Encoding>(context).get_char_class_tag(tag()), ch);
        }
    };

#define BOOST_SPIRIT_X3_CHAR_CLASS(encoding, name)                                 \
    using name##_type = char_class<char_encoding::encoding, name##_tag>;           \
    inline constexpr name##_type name{};

#define BOOST_SPIRIT_X3_CHAR_CLASSES(ns_attr, encoding)                            \
    namespace ns_attr encoding                                                             \
    {                                                                              \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, alnum)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, alpha)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, digit)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, xdigit)                               \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, cntrl)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, graph)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, lower)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, print)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, punct)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, space)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, blank)                                \
        BOOST_SPIRIT_X3_CHAR_CLASS(encoding, upper)                                \
    }

    BOOST_SPIRIT_X3_CHAR_CLASSES(, standard)

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
    BOOST_SPIRIT_X3_CHAR_CLASSES(, standard_wide)
#endif

    BOOST_SPIRIT_X3_CHAR_CLASSES([[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]], ascii)
    BOOST_SPIRIT_X3_CHAR_CLASSES([[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]], iso8859_1)

#undef BOOST_SPIRIT_X3_CHAR_CLASS
#undef BOOST_SPIRIT_X3_CHAR_CLASSES

    using alnum_type  = standard::alnum_type;
    using alpha_type  = standard::alpha_type;
    using digit_type  = standard::digit_type;
    using xdigit_type = standard::xdigit_type;
    using cntrl_type  = standard::cntrl_type;
    using graph_type  = standard::graph_type;
    using lower_type  = standard::lower_type;
    using print_type  = standard::print_type;
    using punct_type  = standard::punct_type;
    using space_type  = standard::space_type;
    using blank_type  = standard::blank_type;
    using upper_type  = standard::upper_type;

    inline constexpr auto const& alnum  = standard::alnum;
    inline constexpr auto const& alpha  = standard::alpha;
    inline constexpr auto const& digit  = standard::digit;
    inline constexpr auto const& xdigit = standard::xdigit;
    inline constexpr auto const& cntrl  = standard::cntrl;
    inline constexpr auto const& graph  = standard::graph;
    inline constexpr auto const& lower  = standard::lower;
    inline constexpr auto const& print  = standard::print;
    inline constexpr auto const& punct  = standard::punct;
    inline constexpr auto const& space  = standard::space;
    inline constexpr auto const& blank  = standard::blank;
    inline constexpr auto const& upper  = standard::upper;

} // boost::spirit::x3

#endif
