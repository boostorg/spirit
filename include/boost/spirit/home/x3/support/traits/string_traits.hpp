/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2010 Bryce Lelbach
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
================================================_==============================*/
#if !defined(BOOST_SPIRIT_X3_STRING_TRAITS_OCTOBER_2008_1252PM)
#define BOOST_SPIRIT_X3_STRING_TRAITS_OCTOBER_2008_1252PM

#if defined(BOOST_SPIRIT_UNICODE) && !defined(BOOST_SPIRIT_X3_UNICODE)
# error "`BOOST_SPIRIT_UNICODE` has no effect on X3. #define `BOOST_SPIRIT_X3_UNICODE`"
#endif

#if defined(BOOST_SPIRIT_X3_NO_STANDARD_WIDE) && !defined(BOOST_SPIRIT_X3_NO_STANDARD_WIDE)
# error "`BOOST_SPIRIT_X3_NO_STANDARD_WIDE` has no effect on X3. #define `BOOST_SPIRIT_X3_NO_STANDARD_WIDE`"
#endif

#include <string>
#include <string_view>
#include <type_traits>
#include <concepts>

namespace boost::spirit::x3::char_encoding
{
    struct standard;

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
    struct standard_wide;
#endif

#ifdef BOOST_SPIRIT_X3_UNICODE
    struct unicode;
#endif
} // boost::spirit::x3::char_encoding

namespace boost::spirit::x3::traits
{
    template <typename T>
    concept CharLike =
        std::same_as<std::remove_cvref_t<T>, char> ||
        std::same_as<std::remove_cvref_t<T>, wchar_t> ||
        std::same_as<std::remove_cvref_t<T>, char8_t> ||
        std::same_as<std::remove_cvref_t<T>, char16_t> ||
        std::same_as<std::remove_cvref_t<T>, char32_t>;

    // Spirit has historically converted "c" to 'c'.
    //
    // While we think it's still useful to retain the conversion,
    // we need to avoid further conversion to `std::basic_string_view`,
    // which leads to performance overhead. This trait enables
    // detection of such arrays.
    //
    // Note that the status quo introduces ambiguity in determining
    // {'c', '\0'} and {'c', 'd'}, but we're not aware of any practical
    // usage of non-null-terminated character array in the context of
    // DSL on parser combinator.
    //
    // However, if compelling use cases emerge, we may revise these
    // semantics. Versioned as `X3` for forward compatibility.
    template <typename T, typename CharT>
    concept X3VagueArrayOf2Chars =
        std::same_as<std::remove_all_extents_t<std::remove_cvref_t<T>>, CharT> &&
        std::is_bounded_array_v<std::remove_cvref_t<T>> &&
        std::extent_v<std::remove_cvref_t<T>> == 2;

    // Main utility to guide `char_`, `lit` and `string` to be
    // resolved into either `x3::literal_char` or `x3::literal_string`.
    //
    // This may also be used in other codes which require the same
    // semantics.
    template <typename T, typename CharT>
    concept CppStringLike =
        // This avoids converting `CharT[2]` to `std::basic_string_view`.
        (!X3VagueArrayOf2Chars<T, CharT>) &&
        // All other types that are *naturally* convertible to `std::basic_string_view`.
        std::convertible_to<std::decay_t<T>, std::basic_string_view<CharT>>;

    // Mixing incompatible character types is semantically wrong.
    // Don't do that. It may even lead to security vulnerabilities.
    template <typename T, typename ExpectedCharT>
    concept CharIncompatibleWith =
        CharLike<T> &&
        !std::same_as<std::remove_cvref_t<T>, ExpectedCharT>;

    // Mixing incompatible character types is semantically wrong.
    // Don't do that. It may even lead to security vulnerabilities.
    template <typename T, typename ExpectedCharT>
    concept StringLikeIncompatibleWith =
        CharLike<std::remove_const_t<std::remove_pointer_t<std::decay_t<T>>>> &&
        !std::convertible_to<T, std::basic_string_view<ExpectedCharT>>;

    namespace detail {
        template <CharLike CharT> struct char_encoding_for_impl;
        template <> struct char_encoding_for_impl<char> { using type = char_encoding::standard; };

    #ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
        template <> struct char_encoding_for_impl<wchar_t> { using type = char_encoding::standard_wide; };
    #endif

    #ifdef BOOST_SPIRIT_X3_UNICODE
        template <> struct char_encoding_for_impl<char8_t> { using type = char_encoding::unicode; };
        template <> struct char_encoding_for_impl<char16_t> { using type = char_encoding::unicode; };
        template <> struct char_encoding_for_impl<char32_t> { using type = char_encoding::unicode; };
    #endif
    } // detail

    template <class T>
    using maybe_owning_string = std::conditional_t<
        std::is_pointer_v<std::decay_t<T>>,
        std::basic_string_view<std::remove_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
        std::remove_cvref_t<T>
    >;

    template <CharLike CharT>
    using char_encoding_for = typename detail::char_encoding_for_impl<CharT>::type;

} // boost::spirit::x3::traits

#endif
