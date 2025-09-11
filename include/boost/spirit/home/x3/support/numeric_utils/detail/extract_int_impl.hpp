/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011 Jan Frederick Eick
    Copyright (c) 2011 Christopher Jefferson
    Copyright (c) 2006 Stephen Nutt
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_DETAIL_EXTRACT_INT_APRIL_17_2006_0816AM
#define BOOST_SPIRIT_X3_DETAIL_EXTRACT_INT_APRIL_17_2006_0816AM

#include <boost/spirit/home/x3/support/unused.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_type.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/traits/numeric_traits.hpp>
#include <boost/spirit/home/x3/support/traits/char_encoding_traits.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/comparison/less.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/seq/elem.hpp>

#include <limits>
#include <iterator>
#include <utility>
#include <type_traits>

#ifndef BOOST_SPIRIT_X3_NUMERICS_LOOP_UNROLL
# define BOOST_SPIRIT_X3_NUMERICS_LOOP_UNROLL 3
#endif

namespace boost::spirit::x3::detail
{
    // The maximum radix digits that can be represented without
    // overflow:
    //
    //         template<typename T, unsigned Radix>
    //         struct digits_traits::value;
    //
    template <typename T, unsigned Radix>
    struct digits_traits;

    template <int Digits, unsigned Radix>
    struct digits2_to_n;

// lookup table for log2(x) : 2 <= x <= 36
#define BOOST_SPIRIT_X3_LOG2 (#error)(#error)                                   \
        (1000000)(1584960)(2000000)(2321920)(2584960)(2807350)                  \
        (3000000)(3169920)(3321920)(3459430)(3584960)(3700430)                  \
        (3807350)(3906890)(4000000)(4087460)(4169920)(4247920)                  \
        (4321920)(4392310)(4459430)(4523560)(4584960)(4643850)                  \
        (4700430)(4754880)(4807350)(4857980)(4906890)(4954190)                  \
        (5000000)(5044390)(5087460)(5129280)(5169925)

#undef BOOST_PP_LOCAL_MACRO
#define BOOST_PP_LOCAL_MACRO(Radix)                                             \
    template <int Digits> struct digits2_to_n<Digits, Radix>                    \
    {                                                                           \
        static constexpr int value = static_cast<int>(                          \
            (Digits * 1000000) /                                                \
                BOOST_PP_SEQ_ELEM(Radix, BOOST_SPIRIT_X3_LOG2));                \
    };

#define BOOST_PP_LOCAL_LIMITS (2, 36)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_PP_LOCAL_MACRO

#undef BOOST_SPIRIT_X3_LOG2

    template <typename T, unsigned Radix>
    struct digits_traits : digits2_to_n<std::numeric_limits<T>::digits, Radix>
    {
        static_assert(std::numeric_limits<T>::radix == 2, "Radix should be 2");
    };

    template <typename T>
    struct digits_traits<T, 10>
    {
        static constexpr int value = std::numeric_limits<T>::digits10;
    };

    // Traits class for radix specific number conversion
    template <unsigned Radix>
    struct radix_traits
    {
        template <typename Char>
        [[nodiscard]] static constexpr bool is_valid(Char ch) noexcept
        {
            return (ch >= '0' && ch <= (Radix > 10 ? '9' : static_cast<Char>('0' + Radix -1)))
                || (Radix > 10 && ch >= 'a' && ch <= static_cast<Char>('a' + Radix -10 -1))
                || (Radix > 10 && ch >= 'A' && ch <= static_cast<Char>('A' + Radix -10 -1));
        }

        template <typename Char>
        [[nodiscard]] static constexpr unsigned digit(Char ch)
            noexcept(noexcept(traits::char_encoding_traits<Char>::encoding_type::tolower(ch)))
        {
            return (Radix <= 10 || (ch >= '0' && ch <= '9'))
                ? ch - '0'
                : traits::char_encoding_traits<Char>::encoding_type::tolower(ch) - 'a' + 10;
        }
    };

    // Accumulator policies for extracting integer from a positive number.
    template <unsigned Radix>
    struct positive_accumulator
    {
        template <typename T, typename Char>
        static constexpr void unchecked_add(T& n, Char ch)
            noexcept(noexcept(radix_traits<Radix>::digit(ch)))
        {
            n = n * T(Radix) + T(radix_traits<Radix>::digit(ch));
        }

        template <typename T, typename Char>
        [[nodiscard]] static constexpr bool checked_add(T& n, Char ch)
            noexcept(noexcept(radix_traits<Radix>::digit(ch)))
        {
            // Ensure n *= Radix will not overflow
            T const max = (std::numeric_limits<T>::max)();
            T const val = max / Radix;
            if (n > val) return false;

            T tmp = n * Radix;

            // Ensure n += digit will not overflow
            int const digit = radix_traits<Radix>::digit(ch);
            if (tmp > max - digit) return false;

            n = tmp + static_cast<T>(digit);
            return true;
        }
    };

    // Accumulator policies for extracting integer from a negative number.
    template <unsigned Radix>
    struct negative_accumulator
    {
        template <typename T, typename Char>
        static constexpr void unchecked_add(T& n, Char ch)
            noexcept(noexcept(radix_traits<Radix>::digit(ch)))
        {
            n = n * T(Radix) - T(radix_traits<Radix>::digit(ch));
        }

        template <typename T, typename Char>
        [[nodiscard]] static constexpr bool checked_add(T& n, Char ch)
            noexcept(noexcept(radix_traits<Radix>::digit(ch)))
        {
            // Ensure n *= Radix will not underflow
            T const min = (std::numeric_limits<T>::min)();
            T const val = min / T(Radix);
            if (n < val) return false;

            T tmp = n * Radix;

            // Ensure n -= digit will not underflow
            int const digit = radix_traits<Radix>::digit(ch);
            if (tmp < min + digit) return false;

            n = tmp - static_cast<T>(digit);
            return true;
        }
    };

    // Common code for `extract_int::parse` specializations
    template <unsigned Radix, typename Accumulator, int MaxDigits>
    struct int_extractor
    {
        template <typename T>
        static constexpr bool need_check_overflow =
            (
                (MaxDigits < 0) ||
                (MaxDigits > digits_traits<T, Radix>::value)
            ) &&
            traits::check_overflow<T>::value;

        template <typename Char, typename T>
            requires need_check_overflow<T>
        [[nodiscard]] static constexpr bool
        call(Char ch, std::size_t count, T& n)
        {
            constexpr std::size_t overflow_free = digits_traits<T, Radix>::value - 1;

            if (count < overflow_free)
            {
                Accumulator::unchecked_add(n, ch);
            }
            else
            {
                if (!Accumulator::checked_add(n, ch))
                {
                    return false; // overflow/underflow
                }
            }
            return true;
        }

        template <typename Char, typename T>
            requires (!need_check_overflow<T>)
        [[nodiscard]] static constexpr bool
        call(Char ch, std::size_t /*count*/, T& n)
        {
            Accumulator::unchecked_add(n, ch);
            return true;
        }

        template <typename Char>
        [[nodiscard]] static constexpr bool
        call(Char /*ch*/, std::size_t /*count*/, unused_type&)
        {
            return true;
        }
    };

    // End of loop checking: check if the number of digits
    // being parsed exceeds `MaxDigits`. Note: if `MaxDigits == -1`
    // we don't do any checking.
    template <int MaxDigits>
    struct check_max_digits
    {
        [[nodiscard]] static constexpr bool
        call(std::size_t count) noexcept
        {
            return count < MaxDigits; // bounded
        }
    };

    template <>
    struct check_max_digits<-1>
    {
        [[nodiscard]] static constexpr bool
        call(std::size_t /*count*/) noexcept
        {
            return true; // unbounded
        }
    };

    // extract_int: main code for extracting integers
#define BOOST_SPIRIT_X3_NUMERIC_INNER_LOOP(z, x, data)                          \
        if (!check_max_digits<MaxDigits>::call(count + leading_zeros)           \
            || it == last)                                                      \
            break;                                                              \
        ch = *it;                                                               \
        if (!radix_check::is_valid(ch) || !extractor::call(ch, count, val))     \
            break;                                                              \
        ++it;                                                                   \
        ++count;

    template <
        typename T, unsigned Radix, unsigned MinDigits, int MaxDigits,
        typename Accumulator = positive_accumulator<Radix>,
        bool Accumulate = false
    >
    struct extract_int
    {
#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(push)
# pragma warning(disable: 4127)   // conditional expression is constant
# pragma warning(disable: 4459)   // declaration hides global declaration
#endif
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse_main(It& first, Se const& last, Attribute& attr)
            // TODO: noexcept
        {
            using radix_check = radix_traits<Radix>;
            using extractor = int_extractor<Radix, Accumulator, MaxDigits>;
            using char_type = std::iter_value_t<It>;

            It it = first;
            std::size_t leading_zeros = 0;
            if constexpr (!Accumulate)
            {
                // skip leading zeros
                while (it != last && *it == '0' && leading_zeros < MaxDigits)
                {
                    ++it;
                    ++leading_zeros;
                }
            }

            using attribute_type = traits::attribute_type_t<Attribute>;
            attribute_type val = Accumulate ? attr : attribute_type(0);
            std::size_t count = 0;
            char_type ch;

            while (true)
            {
                BOOST_PP_REPEAT(BOOST_SPIRIT_X3_NUMERICS_LOOP_UNROLL, BOOST_SPIRIT_X3_NUMERIC_INNER_LOOP, _)
            }

            if (count + leading_zeros >= MinDigits)
            {
                traits::move_to(std::move(val), attr);
                first = it;
                return true;
            }
            return false;
        }
#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(pop)
#endif

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        parse(It& first, Se const& last, unused_type)
            noexcept(
                std::is_nothrow_constructible_v<T, int> &&
                noexcept(extract_int::parse_main(first, last, std::declval<T&>()))
            )
        {
            T n(0); // must calculate value to detect over/underflow
            return extract_int::parse_main(first, last, n);
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse(It& first, Se const& last, Attribute& attr)
            noexcept(noexcept(extract_int::parse_main(first, last, attr)))
        {
            return extract_int::parse_main(first, last, attr);
        }
    };
#undef BOOST_SPIRIT_X3_NUMERIC_INNER_LOOP

    // extract_int: main code for extracting integers
    // common case where MinDigits == 1 and MaxDigits = -1
#define BOOST_SPIRIT_X3_NUMERIC_INNER_LOOP(z, x, data)                          \
        if (it == last)                                                         \
            break;                                                              \
        ch = *it;                                                               \
        if (!radix_check::is_valid(ch))                                         \
            break;                                                              \
        if (!extractor::call(ch, count, val))                                   \
            return false;                                                       \
        ++it;                                                                   \
        ++count;

    template <typename T, unsigned Radix, typename Accumulator, bool Accumulate>
    struct extract_int<T, Radix, 1, -1, Accumulator, Accumulate>
    {
#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(push)
# pragma warning(disable: 4127)   // conditional expression is constant
# pragma warning(disable: 4459)   // declaration hides global declaration
#endif
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse_main(It& first, Se const& last, Attribute& attr)
            // TODO: noexcept
        {
            using radix_check = radix_traits<Radix>;
            using extractor = int_extractor<Radix, Accumulator, -1>;
            using char_type = std::iter_value_t<It>;

            It it = first;
            std::size_t count = 0;
            if constexpr (!Accumulate)
            {
                // skip leading zeros
                while (it != last && *it == '0')
                {
                    ++it;
                    ++count;
                }

                if (it == last)
                {
                    if (count == 0) return false; // must have at least one digit
                    attr = 0;
                    first = it;
                    return true;
                }
            }

            using attribute_type = traits::attribute_type_t<Attribute>;
            attribute_type val = Accumulate ? attr : attribute_type(0);

            char_type ch = *it;
            if (!radix_check::is_valid(ch) || !extractor::call(ch, 0, val))
            {
                if (count == 0) return false; // must have at least one digit
                traits::move_to(std::move(val), attr);
                first = it;
                return true;
            }

            count = 0;
            ++it;
            while (true)
            {
                BOOST_PP_REPEAT(BOOST_SPIRIT_X3_NUMERICS_LOOP_UNROLL, BOOST_SPIRIT_X3_NUMERIC_INNER_LOOP, _)
            }

            traits::move_to(std::move(val), attr);
            first = it;
            return true;
        }
#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(pop)
#endif

        template <std::forward_iterator It, std::sentinel_for<It> Se>
        [[nodiscard]] static constexpr bool
        parse(It& first, Se const& last, unused_type)
            noexcept(
                std::is_nothrow_constructible_v<T, int> &&
                noexcept(extract_int::parse_main(first, last, std::declval<T&>()))
            )
        {
            T n(0); // must calculate value to detect over/underflow
            return extract_int::parse_main(first, last, n);
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute>
        [[nodiscard]] static constexpr bool
        parse(It& first, Se const& last, Attribute& attr)
            noexcept(noexcept(extract_int::parse_main(first, last, attr)))
        {
            return extract_int::parse_main(first, last, attr);
        }
    };

#undef BOOST_SPIRIT_X3_NUMERIC_INNER_LOOP
} // boost::spirit::x3::detail

#endif
