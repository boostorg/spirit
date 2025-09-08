/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_SUPPORT_TRAITS_CHAR_ENCODING_TRAITS_HPP
#define BOOST_SPIRIT_X3_SUPPORT_TRAITS_CHAR_ENCODING_TRAITS_HPP

#include <boost/spirit/home/x3/char/char.hpp>

#include <boost/spirit/home/x3/char/literal_char.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>

#ifdef BOOST_SPIRIT_X3_UNICODE
# include <boost/spirit/home/x3/char_encoding/unicode.hpp>
#endif

namespace boost::spirit::x3::traits
{
    namespace detail
    {
        template <typename Encoding>
        struct char_encoding_traits_impl
        {
            using encoding_type = Encoding;

            template <class... Args>
            [[nodiscard]] static constexpr auto lit(Args&&... args)
                noexcept(noexcept(Encoding::lit(std::forward<Args>(args)...)))
            {
                return Encoding::lit(std::forward<Args>(args)...);
            }

            template <class... Args>
            [[nodiscard]] static constexpr auto string(Args&&... args)
                noexcept(noexcept(Encoding::string(std::forward<Args>(args)...)))
            {
                return Encoding::string(std::forward<Args>(args)...);
            }
        };
    } // detail

    template <traits::CharLike CharT>
    struct char_encoding_traits;

    template <>
    struct char_encoding_traits<char> : detail::char_encoding_traits_impl<char_encoding::standard> {};

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
    template <>
    struct char_encoding_traits<wchar_t> : detail::char_encoding_traits_impl<char_encoding::standard_wide> {};
#endif

#ifdef BOOST_SPIRIT_X3_UNICODE
    template <>
    struct char_encoding_traits<char32_t> : detail::char_encoding_traits_impl<char_encoding::unicode> {};
#endif

} // boost::spirit::x3::traits

#endif
