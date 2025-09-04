/*=============================================================================
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_CORE_PARSE_RESULT_HPP
#define BOOST_SPIRIT_X3_CORE_PARSE_RESULT_HPP

#include <boost/spirit/home/x3/support/expectation.hpp>

#include <optional>
#include <iterator>
#include <ranges>
#include <string_view>
#include <type_traits>

namespace boost::spirit::x3
{
    template <std::forward_iterator It, std::sentinel_for<It> Se = It>
    struct parse_result
    {
        static_assert(
            BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE == 0,
            "Using this class requires `#define BOOST_SPIRIT_X3_THROW_EXPECTATION_FAILURE 0`"
        );

        using iterator_type = It;
        using sentinel_type = Se;

        // Strictly equivalent to the logical conjunction of the return
        // values of the `.parse()` functions of all underlying parsers.
        bool ok = false;

        // Represents the failure of `x3::expect[p]` and `a > b`.
        // Has value if and only if `ok` is `false` and any of
        // the underlying parsers have encountered expectation
        // failure.
        std::optional<expectation_failure<It>> expect_failure;

        // Represents the remaining subrange of the input, after
        // applications of all (partially) successful attempts on
        // underlying parsers.
        std::ranges::subrange<It, Se> remainder;

        // Convenient accessor, returns the string view of `remainder`.
        [[nodiscard]] constexpr std::basic_string_view<std::iter_value_t<It>>
        remainder_str() const
            requires requires {
                typename std::basic_string_view<std::iter_value_t<It>>;
                requires std::is_constructible_v<std::basic_string_view<std::iter_value_t<It>>, std::ranges::subrange<It, Se> const&>;
            }
        {
            return std::basic_string_view<std::iter_value_t<It>>{remainder};
        }

        [[nodiscard]] constexpr bool completed() const noexcept
        {
            // Cases:
            // (1) The input was empty, and
            //     (1-a) The grammar accepts empty input and succeeded => remainder.empty() &&  ok
            //     (1-b) The grammar rejects empty input and failed    => remainder.empty() && !ok
            // (2) The input was NOT empty, and
            //     (2-a) The grammar comsumed all input and succeeded                             =>  remainder.empty() &&  ok
            //     (2-b) The grammar consumed some input and (partially) succeeded                => !remainder.empty() &&  ok
            //     (2-c) The grammar consumed all input and failed (possibly via semantic action) =>  remainder.empty() && !ok
            //     (2-d) The grammar consumed some input and failed                               => !remainder.empty() && !ok

            // `completed()` is `true` if and only if the parse result is
            // either 1-a or 2-a; `false` otherwise. Note that 2-b is
            // intentionally considered out of scope.
            //
            // This definition eliminates the need to double-check both
            // the resulting `bool` and iterators on the user's part,
            // which had been the long-standing burden of Spirit since
            // the Qi era.
            return ok && remainder.empty();
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return this->completed();
        }
    };

    template <std::ranges::forward_range R>
    using parse_result_for = parse_result<std::ranges::iterator_t<R>, std::ranges::sentinel_t<R>>;
} // boost::spirit::x3

#endif
