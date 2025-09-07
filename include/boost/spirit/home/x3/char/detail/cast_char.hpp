/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_CAST_CHAR_NOVEMBER_10_2006_0907AM
#define BOOST_SPIRIT_X3_CAST_CHAR_NOVEMBER_10_2006_0907AM

#include <type_traits>
#include <concepts>

namespace boost::spirit::x3::detail
{
    // Here's the thing... typical encodings (except ASCII) deal with unsigned
    // integers > 127 (ASCII uses only 127). Yet, most char and wchar_t are signed.
    // Thus, a char with value > 127 is negative (e.g. char 233 is -23). When you
    // cast this to an unsigned int with 32 bits, you get 4294967273!
    //
    // The trick is to cast to an unsigned version of the source char first
    // before casting to the target. {P.S. Don't worry about the code, the
    // optimizer will optimize the if-else branches}

    template <typename TargetChar, typename SourceChar>
    [[nodiscard]] constexpr TargetChar cast_char(SourceChar ch) noexcept
    {
        if constexpr (std::is_signed_v<TargetChar> != std::is_signed_v<SourceChar>)
        {
            if constexpr (std::is_signed_v<SourceChar>)
            {
                // source is signed, target is unsigned
                return TargetChar(static_cast<std::make_unsigned_t<SourceChar>>(ch));
            }
            else
            {
                // source is unsigned, target is signed
                return TargetChar(static_cast<std::make_signed_t<SourceChar>>(ch));
            }
        }
        else
        {
            // source and target has same signedness
            return TargetChar(ch); // just cast
        }
    }

    template <typename SourceChar, typename TargetChar>
    concept cast_char_viable = requires(SourceChar ch) {
        { cast_char<TargetChar>(ch) } -> std::convertible_to<TargetChar>;
    };

    template <typename SourceChar, typename TargetChar>
    concept cast_char_noexcept = requires(SourceChar ch) {
        { cast_char<TargetChar>(ch) } noexcept -> std::convertible_to<TargetChar>;
    };
} // boost::spirit::x3::detail

#endif
