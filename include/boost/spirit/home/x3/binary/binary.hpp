/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_BINARY_MAY_08_2007_0808AM)
#define BOOST_SPIRIT_X3_BINARY_MAY_08_2007_0808AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <cstdint>


#include <boost/endian/conversion.hpp>
#include <boost/endian/arithmetic.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/mpl/or.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/config.hpp>

namespace boost { namespace spirit { namespace x3
{
#if 0
    ///////////////////////////////////////////////////////////////////////
    template <typename Encoding, typename Attribute>
    struct get_info<literal_char<Encoding, Attribute>>
    {
        typedef std::string result_type;
        std::string operator()(literal_char<Encoding, Attribute> const& p) const
        {
            return '\'' + to_utf8(Encoding::toucs4(p.ch)) + '\'';
        }
    };
    template <BOOST_SCOPED_ENUM(boost::endian::endianness) bits>
        struct what;

        template <>
        struct what<boost::endian::endianness::native>
        {
            static std::string is()
            {
                return "native-endian binary";
            }
        };

        template <>
        struct what<boost::endian::endianness::little>
        {
            static char const* is()
            {
                return "little-endian binary";
            }
        };

        template <>
        struct what<boost::endian::endianness::big>
        {
            static char const* is()
            {
                return "big-endian binary";
            }
        };
    }
#endif
    ///////////////////////////////////////////////////////////////////////////
    template <typename V, typename T
      , BOOST_SCOPED_ENUM(boost::endian::order) endian, std::size_t bits>
    struct binary_lit_parser
      : parser<binary_lit_parser<V, T, endian, bits> >
    {
        static bool const has_attribute = false;

        binary_lit_parser(V n_)
          : n(n_) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, unused_type, Attribute& attr_param) const
        {
            x3::skip_over(first, last, context);

            // Even if the endian types are not pod's (at least not in the
            // definition of C++03) it seems to be safe to assume they are
            // (but in C++0x the endian types _are_ PODs).
            // This allows us to treat them as a sequence of consecutive bytes.
            boost::endian::endian_arithmetic<endian, T, bits> attr_;

#if defined(BOOST_MSVC)
// warning C4244: 'argument' : conversion from 'const int' to 'foo', possible loss of data
#pragma warning(push)
#pragma warning(disable: 4244)
#endif
            attr_ = n;
#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

            unsigned char* bytes = reinterpret_cast<unsigned char*>(&attr_);

            Iterator it = first;
            for (unsigned int i = 0; i < sizeof(attr_); ++i)
            {
                if (it == last || *bytes++ != static_cast<unsigned char>(*it++))
                    return false;
            }

            first = it;
            x3::traits::move_to(attr_, attr_param);
            return true;
        }

        V n;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, BOOST_SCOPED_ENUM(boost::endian::order) endian, std::size_t bits>
    struct any_binary_parser : parser<any_binary_parser<T, endian, bits > >
    {

        typedef T attribute_type;
        static bool const has_attribute =
            !is_same<unused_type, attribute_type>::value;

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, unused_type, Attribute& attr_param) const
        {
            x3::skip_over(first, last, context);

            attribute_type attr_;
            unsigned char* bytes = reinterpret_cast<unsigned char*>(&attr_);

            Iterator it = first;
            for (unsigned int i = 0; i < sizeof(attr_); ++i)
            {
                std::cout <<"Hey"<<std::endl;
                if (it == last)
                    return false;
                *bytes++ = *it++;
            }

            first = it;
            x3::traits::move_to(endian::conditional_reverse< endian, endian::order::native>(attr_), attr_param);
            return true;
        }

        template <typename V>
        binary_lit_parser< V, T, endian, bits> operator()(V n) const
        {
            return {n};
        }
    };

#define BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(name, endiantype, attrtype, bits)                  \
    typedef any_binary_parser< attrtype, boost::endian::order::endiantype, bits > name##type; \
    name##type const name = name##type();


    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(byte_, native, uint_least8_t, 8)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(word, native, uint_least16_t, 16)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(big_word, big, uint_least16_t, 16)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(little_word, little, uint_least16_t, 16)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(dword, native, uint_least32_t, 32)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(big_dword, big, uint_least32_t, 32)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(little_dword, little, uint_least32_t, 32)
#ifdef BOOST_HAS_LONG_LONG
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(qword, native, uint_least64_t, 64)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(big_qword, big, uint_least64_t, 64)
    BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE(little_qword, little, uint_least64_t, 64)
#endif

#undef BOOST_SPIRIT_MAKE_BINARY_PRIMITIVE
#if 0
#define BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE(name, endiantype, bits)    \
    template<typename Modifiers>                                              \
    struct make_primitive<tag::name, Modifiers>                               \
      : make_binary_parser<detail::floating_point<bits>,                      \
        boost::endian::endianness::endiantype, bits> {};                      \
                                                                              \
    template<typename Modifiers, typename A0>                                 \
    struct make_primitive<                                                    \
        terminal_ex<tag::name, fusion::vector1<A0> >, Modifiers>              \
      : make_binary_lit_parser<A0, detail::floating_point<bits>,              \
        boost::endian::endianness::endiantype,                                \
        bits> {};                                                             \
                                                                              \
    /***/

    BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE(bin_float, native, 32)
    BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE(big_bin_float, big, 32)
    BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE(little_bin_float, little, 32)
    BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE(bin_double, native, 64)
    BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE(big_bin_double, big, 64)
    BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE(little_bin_double, little, 64)

#undef BOOST_SPIRIT_MAKE_BINARY_IEEE754_PRIMITIVE
#endif
}}}

#endif
